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
#include "Visualisation/VUMeter.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/VUMeter_Basic.h"
#include "Visualisation/VUMeter_Image.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
//--------------------------------------

namespace Visualisation::VUMeter {
    //**************************************************************************
    // IVUMeter
    //**************************************************************************
    IVUMeter::pointer_type IVUMeter::MakeVisualisation(visualisation_type eType,
                                                       const config_type& config,
                                                       const dimensions_type& dim,
                                                       bool bMonochrome) {
        const auto bGradient = !bMonochrome && config.m_Color.m_Palette.Primary != config.m_Color.m_Palette.Secondary;
        pointer_type pVis;
        switch (eType) {
            case VUMeterType::Meter1:
                if (bGradient) {
                    pVis = std::make_shared<HorizontalSplit::StereoGradient>(config, dim);
                } else {
                    pVis = std::make_shared<HorizontalSplit::Stereo>(config, dim);
                }
                break;
            case VUMeterType::Meter2:
                if (bGradient) {
                    pVis = std::make_shared<VerticalSplit::StereoGradient>(config, dim);
                } else {
                    pVis = std::make_shared<VerticalSplit::Stereo>(config, dim);
                }
                break;
            case VUMeterType::Image:
                pVis = std::make_shared<ImageVU>(config, dim);
                break;
        }
        return pVis;
    }
} // namespace Visualisation::VUMeter
