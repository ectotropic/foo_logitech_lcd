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
#include "Visualisation/SpectrumAnalyser.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/SpectrumAnalyser_Basic.h"
#include "Visualisation/SpectrumAnalyser_Block.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
//--------------------------------------

namespace Visualisation::SpectrumAnalyser {
    //**************************************************************************
    // ISpectrumAnalyser
    //**************************************************************************
    ISpectrumAnalyser::pointer_type ISpectrumAnalyser::MakeVisualisation(visualisation_type eType,
                                                                         const config_type& config,
                                                                         const dimensions_type& dim,
                                                                         bool bMonochrome) {
        if (bMonochrome || config.m_Color.m_Palette.Primary == config.m_Color.m_Palette.Secondary) {
            switch (eType) {
                case SpectrumAnalyserType::Mono:
                    return std::make_shared<Mono>(config, dim);
                case SpectrumAnalyserType::Stereo:
                    return std::make_shared<Stereo>(config, dim);
                case SpectrumAnalyserType::MonoBlock:
                    return std::make_shared<Block::Mono>(config, dim);
                case SpectrumAnalyserType::StereoBlock:
                    return std::make_shared<Block::Stereo>(config, dim);
            }
        } else {
            switch (eType) {
                case SpectrumAnalyserType::Mono:
                    return std::make_shared<MonoGradient>(config, dim);
                case SpectrumAnalyserType::Stereo:
                    return std::make_shared<StereoGradient>(config, dim);
                case SpectrumAnalyserType::MonoBlock:
                    return std::make_shared<Block::MonoGradient>(config, dim);
                case SpectrumAnalyserType::StereoBlock:
                    return std::make_shared<Block::StereoGradient>(config, dim);
            }
        }
        return pointer_type{};
    }

    //--------------------------------------------------------------------------

    ISpectrumAnalyser::ISpectrumAnalyser(visualisation_type eType,
                                         const config_type& config,
                                         const dimensions_type& dim) noexcept :
        IVisualisation{ Mode(), static_cast<size_type>(eType), dim },
        m_Config{ config } {
        m_Config.m_Peak.m_bEnable = m_Config.m_Peak.m_bEnable && (m_Config.m_Peak.m_fDecayRate > 0.f);
    }
} // namespace Visualisation::SpectrumAnalyser
