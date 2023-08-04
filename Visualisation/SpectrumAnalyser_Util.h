#pragma once
#ifndef GUID_AAC6C562_778B_49B1_801D_8CA9E5E493D3
#define GUID_AAC6C562_778B_49B1_801D_8CA9E5E493D3
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
#include "Visualisation/SpectrumAnalyser.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <cmath>
#include <utility>
#include <functional>
//--------------------------------------

namespace Visualisation::SpectrumAnalyser {
    //**************************************************************************
    // Util
    //**************************************************************************
    struct Util final {
        using sample_type     = typename ISpectrumAnalyser::sample_type;
        using coord_type      = typename ISpectrumAnalyser::coord_type;
        using size_type       = typename ISpectrumAnalyser::size_type;
        using dimensions_type = typename ISpectrumAnalyser::dimensions_type;
        using config_type     = typename ISpectrumAnalyser::config_type;
        using mode_type       = SpectrumAnalyserMode;

        //----------------------------------------------------------------------
        // Transformer
        //----------------------------------------------------------------------
        struct Transformer final {
            Transformer(const config_type& config) noexcept :
                m_fPreScale { config.m_fPreScale },
                m_fPostScale{ config.m_fPostScale },
                m_fOffset   { config.m_fOffset },
                m_eTransform{ config.m_SpectrumMode } {}

            sample_type operator()(size_type /*channel*/,
                                   sample_type fVal) const noexcept {
                if (fVal == 0) { return 0; }

                constexpr const auto fHalf{ static_cast<sample_type>(.5) };
                constexpr const auto fMax{  static_cast<sample_type>(1.) };
                constexpr const auto fMin{  static_cast<sample_type>(0.) };

                // Equations here chosen for visual output
                // and not to represent any specific
                // normalisation.
                // If using "Linear", "PreScale" is treated
                // as a simple scale factor (hence PreScale
                // is always applied).
                //
                // Target output range is [0,1]
                auto fTransformed = m_fPreScale * fVal;
                switch (m_eTransform) {
                    case mode_type::Linear:
                        break;

                    case mode_type::NonLinear1: {
                        fTransformed = std::sqrt(fTransformed);
                        break;
                    }

                    case mode_type::NonLinear2: {
                        fTransformed = std::log10(fTransformed);
                        fTransformed += m_fOffset;
                        fTransformed *= m_fPostScale;
                        break;
                    }

                    case mode_type::NonLinear3: {
                        fTransformed = fTransformed * fTransformed;
                        fTransformed = std::log10(fTransformed * fTransformed);
                        fTransformed += m_fOffset;
                        fTransformed *= m_fPostScale;
                        break;
                    }

                    case mode_type::NonLinear4: {
                        fTransformed = fTransformed * fTransformed * fHalf;
                        fTransformed = std::log10(fTransformed);
                        fTransformed += m_fOffset;
                        fTransformed *= m_fPostScale;
                        break;
                    }

                    HintNoDefault();
                }

                if (fTransformed >= fMax) { return fMax; }
                if (fTransformed <= fMin) { return fMin; }

                return fTransformed;
            }

            const sample_type m_fPreScale { 0 };
            const sample_type m_fPostScale{ 0 };
            const sample_type m_fOffset   { 0 };
            const mode_type   m_eTransform{ mode_type::NonLinear4 };
        }; // struct Transformer final

        //==================================================
        // These functions assume the passed lambdas will be
        // inlined; if they are not, the functions should be
        // replaced as the costs will quickly mount up
        // (especially for larger displays)
        template <typename SamplesT, typename LambdaT>
        static void Draw(const SamplesT& samples,
                         const dimensions_type& canvasSize,
                         LambdaT fnDraw) noexcept {
            assert(!samples.empty());
            const auto fHeight = static_cast<sample_type>(canvasSize.cy);
            coord_type nX = 0;
            for (const auto fSample : samples) {
                const auto fY = std::round(fSample * fHeight);
                const auto nY = static_cast<coord_type>(fY);
                nX = fnDraw(fSample, nX, nY);
            }
        } // static void Draw(...)

        //--------------------------------------------------

        template <typename SamplesT, typename LambdaT>
        static void Draw(const SamplesT& samplesL,
                         const SamplesT& samplesR,
                         const dimensions_type& canvasSize,
                         LambdaT fnDraw) noexcept {
            assert(!samplesL.empty()); assert(!samplesR.empty());
            assert(samplesL.size() == samplesR.size());
            const auto count = std::min(samplesL.size(), samplesR.size());
            const auto fHeight = static_cast<sample_type>(canvasSize.cy);
            const auto fHalfHeight = fHeight * .5;
            coord_type nXL = 0, nXR = 0;
            for (size_type i = 0; i < count; ++i) {
                {
                    const auto fSampleL = samplesL[i];
                    const auto fYL = std::round(fSampleL * fHalfHeight + fHalfHeight);
                    const auto nYL = static_cast<coord_type>(fYL);
                    nXL = fnDraw(fSampleL, nXL, nYL);
                }

                {
                    const auto fSampleR = samplesR[i];
                    const auto fYR = std::round(fSampleR * fHalfHeight + fHalfHeight);
                    const auto nYR = canvasSize.cy - static_cast<coord_type>(fYR);
                    nXR = fnDraw(fSampleR, nXR, nYR);
                }
            }
        }
        //==================================================
    }; // struct Util final
} // namespace Visualisation::SpectrumAnalyser

#endif // GUID_AAC6C562_778B_49B1_801D_8CA9E5E493D3
