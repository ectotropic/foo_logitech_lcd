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
#include "CommonHeaders.h"
#include "Visualisation/Oscilloscope.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/Oscilloscope_Basic.h"
#include "Visualisation/Oscilloscope_Radial.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
//--------------------------------------

namespace Visualisation::Oscilloscope {
    //**************************************************************************
    // IOscilloscope
    //**************************************************************************
    IOscilloscope::pointer_type IOscilloscope::MakeVisualisation(visualisation_type eType,
                                                                 const config_type& config,
                                                                 const dimensions_type& dim,
                                                                 bool bMonochrome) {
        if (bMonochrome || config.m_Color.m_Palette.Primary == config.m_Color.m_Palette.Secondary) {
            switch (eType) {
                case OscilloscopeType::DotMono:
                    return std::make_shared<DotMono>(config, dim);
                case OscilloscopeType::DotStereo:
                    return std::make_shared<DotStereo>(config, dim);
                case OscilloscopeType::LineMono:
                    return std::make_shared<LineMono>(config, dim);
                case OscilloscopeType::LineStereo:
                    return std::make_shared<LineStereo>(config, dim);
                case OscilloscopeType::CircleMono:
                    return std::make_shared<Radial::CircleMono>(config, dim);
                case OscilloscopeType::StarBurstMono:
                    return std::make_shared<Radial::StarBurstMono>(config, dim);
            }
        } else {
            switch (eType) {
                case OscilloscopeType::DotMono:
                    return std::make_shared<DotMonoGradient>(config, dim);
                case OscilloscopeType::DotStereo:
                    return std::make_shared<DotStereoGradient>(config, dim);
                case OscilloscopeType::LineMono:
                    return std::make_shared<LineMonoGradient>(config, dim);
                case OscilloscopeType::LineStereo:
                    return std::make_shared<LineStereoGradient>(config, dim);
                case OscilloscopeType::CircleMono:
                    return std::make_shared<Radial::CircleMonoGradient>(config, dim);
                case OscilloscopeType::StarBurstMono:
                    return std::make_shared<Radial::StarBurstMonoGradient>(config, dim);
            }
        }
        return pointer_type{};
    }
} // namespace Visualisation::Oscilloscope
