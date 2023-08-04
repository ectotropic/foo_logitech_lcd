#pragma once
#ifndef GUID_1433B3B8_96DB_47EC_862F_D2F6A927901F
#define GUID_1433B3B8_96DB_47EC_862F_D2F6A927901F
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

namespace Visualisation::VUMeter::HorizontalSplit {
    //**************************************************************************
    // Stereo
    //**************************************************************************
    class Stereo : public IVUMeter {
    private:
        using base_class = IVUMeter;
        using this_class = Stereo;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Meter1;
        }

    public:
        Stereo(const config_type& config,
               const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class Stereo final

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    class StereoGradient : public IVUMeter {
    private:
        using base_class = IVUMeter;
        using this_class = StereoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Meter1;
        }

    public:
        StereoGradient(const config_type& config,
                       const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class StereoGradient final
} // namespace Visualisation::VUMeter::HorizontalSplit

//==============================================================================

namespace Visualisation::VUMeter::VerticalSplit {
    //**************************************************************************
    // Stereo
    //**************************************************************************
    class Stereo : public IVUMeter {
    private:
        using base_class = IVUMeter;
        using this_class = Stereo;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Meter2;
        }

    public:
        Stereo(const config_type& config,
               const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class Stereo final

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    class StereoGradient : public IVUMeter {
    private:
        using base_class = IVUMeter;
        using this_class = StereoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Meter2;
        }

    public:
        StereoGradient(const config_type& config,
                       const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;

    }; // class StereoGradient final
} // namespace Visualisation::VUMeter::VerticalSplit

#endif // GUID_1433B3B8_96DB_47EC_862F_D2F6A927901F
