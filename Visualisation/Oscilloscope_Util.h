#pragma once
#ifndef GUID_0E298148_DB72_4D1B_AA9C_C4FAADC17248
#define GUID_0E298148_DB72_4D1B_AA9C_C4FAADC17248
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
#include "Visualisation/Oscilloscope.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/TrigCache.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <cmath>
#include <utility>
#include <functional>
//--------------------------------------

namespace Visualisation::Oscilloscope {
    //**************************************************************************
    // Util
    //**************************************************************************
    struct Util final {
        //--------------------------------------------------

        using sample_type     = typename IOscilloscope::sample_type;
        using coord_type      = typename IOscilloscope::coord_type;
        using size_type       = typename IOscilloscope::size_type;
        using dimensions_type = typename IOscilloscope::dimensions_type;
        using config_type     = typename IOscilloscope::config_type;

        //--------------------------------------------------

        inline static constexpr const size_type RadialSampleCount{ 360 };

        //==================================================
        // These functions assume the passed lambdas will be
        // inlined; if they are not, the functions should be
        // replaced as the costs will quickly mount up
        // (especially for larger displays)
        template <typename SamplesT, typename LambdaT>
        static void Draw(const SamplesT& samples,
                         const dimensions_type& canvasSize,
                         sample_type scaleFactor,
                         LambdaT fnDraw) noexcept {
            assert(!samples.empty());
            const auto fHeight = static_cast<float>(canvasSize.cy);
            coord_type nX = 0;
            for (auto fSample : samples) {
                fSample = .5f + fSample * scaleFactor * .5f;
                const auto fY = std::round(fHeight * fSample);
                const auto nY = static_cast<coord_type>(fY);
                nX = fnDraw(fSample, nX, nY);
            }
        }

        //--------------------------------------------------

        template <typename SamplesT, typename LambdaT>
        static void DrawRadial(const SamplesT& samples,
                               const dimensions_type& canvasSize,
                               sample_type scaleFactor,
                               LambdaT fnDraw) noexcept {
            assert(!samples.empty());
            const auto fHalfWidth = static_cast<float>(canvasSize.cx) * 0.5f;
            const auto fHalfHeight = static_cast<float>(canvasSize.cy) * 0.5f;

            const auto fMaxDiameter = static_cast<sample_type>(std::min(canvasSize.cx, canvasSize.cy));
            const auto fTargetRadius = (fMaxDiameter - (fMaxDiameter * .1f)) * .5f; //< 1% margin
            const auto fRadiusFactorX = fTargetRadius / static_cast<float>(canvasSize.cx);
            const auto fRadiusFactorY = fTargetRadius / static_cast<float>(canvasSize.cy);

            const auto fScaleX = scaleFactor * fHalfWidth * fRadiusFactorX;
            const auto fScaleY = scaleFactor * fHalfHeight * fRadiusFactorY;

            // Parametric Circle Equation:
            //      x = radius * cos(theta)
            //      y = radius * sin(theta)
            for (size_type i = 0; i < samples.size(); ++i) {
                const auto fSample = samples[i];
                const auto sincos = s_TrigCache.sincos(i, samples.size());
                const auto fX = sincos.cosine * fScaleX;
                const auto fY = sincos.sine * fScaleY;
                fnDraw(fSample, fX, fY);
            }
        }
        //==================================================

    private:
        using trig_cache_type =
            ::util::trig_cache<sample_type, RadialSampleCount>;
        inline static trig_cache_type s_TrigCache{};
    }; // struct Oscilloscope
} // namespace Visualisation::Oscilloscope

#endif // GUID_0E298148_DB72_4D1B_AA9C_C4FAADC17248
