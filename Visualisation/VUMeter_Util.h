#pragma once
#ifndef GUID_B516B9BE_8D37_4B71_9A79_1C02E4D371E0
#define GUID_B516B9BE_8D37_4B71_9A79_1C02E4D371E0
/*******************************************************************************
 *******************************************************************************
 * Copyright (c) 2009-2023 ectotropic (ectotropic@gmail.com,                   *
 *                                     https://github.com/ectotropic)          *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation, either version 2.1 of the License, or (at     *
 * your option) any later version.                                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
 * for more details.                                                           *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************
 ******************************************************************************/

//--------------------------------------
//
#include "Visualisation/VUMeter.h"
//--------------------------------------

namespace Visualisation::VUMeter {
    //**************************************************************************
    // Util
    //**************************************************************************
    struct Util final {
        using sample_type = typename IVUMeter::sample_type;
        using coord_type = typename IVUMeter::coord_type;
        using size_type = typename IVUMeter::size_type;
        using dimensions_type = typename IVUMeter::dimensions_type;
        using config_type = typename IVUMeter::config_type;

        //----------------------------------------------------------------------
        // Transformer
        //----------------------------------------------------------------------
        struct Transformer final {
        public:
            Transformer(const config_type& config) noexcept :
                m_fOffset{
                    config.m_Range[0].m_fMin,
                    config.m_Range[1].m_fMin
                },
                m_fScale {
                    config.m_Range[0].m_fMax - config.m_Range[0].m_fMin,
                    config.m_Range[1].m_fMax - config.m_Range[1].m_fMin
                } {}

            sample_type operator()(size_type channel, sample_type fVal) const noexcept {
                // Value is in dB and likely to be negative with 0 dB the
                // maximum value. The minimum value could be very large,
                // but meaningful values are likely to be greater than
                // -100 dB. Common physical VU Meters target values
                // with a minimum of -20 dB, although other values
                // are also used (e.g. -42 dB).
                //
                // The default range used here is [-40, 0] which is converted
                // to [0,1] (note that smaller and larger values will be
                // kept, but will be outside the [0,1] range).
                if (channel == static_cast<size_type>(-1)) { //< Combined value
                    const auto fVal0{ (fVal - m_fOffset[0]) / m_fScale[0] };
                    const auto fVal1{ (fVal - m_fOffset[1]) / m_fScale[1] };
                    return (fVal0 + fVal1) / static_cast<sample_type>(2.);
                } else {
                    const auto index = channel % 2;
                    return (fVal - m_fOffset[index]) / m_fScale[index];
                }
            }

        private:
            const sample_type m_fOffset[2]{ static_cast<sample_type>(40.), static_cast<sample_type>(40.) };
            const sample_type  m_fScale[2]{ static_cast<sample_type>(40.), static_cast<sample_type>(40.) };
        }; // struct Transformer final
    }; // struct Util final
} // namespace Visualisation::VUMeter

#endif // GUID_B516B9BE_8D37_4B71_9A79_1C02E4D371E0
