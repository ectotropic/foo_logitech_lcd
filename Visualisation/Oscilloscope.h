#pragma once
#ifndef GUID_0FBAACAB_7E2A_4A0C_814F_9DB4FC0B535D
#define GUID_0FBAACAB_7E2A_4A0C_814F_9DB4FC0B535D
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
#include "Visualisation/Visualisation.h"
#include "Config/Config_Oscilloscope.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
//--------------------------------------

namespace Visualisation::Oscilloscope {
    //**************************************************************************
    // IOscilloscope
    //**************************************************************************
    class IOscilloscope : public IVisualisation {
    private:
        using base_class = IVisualisation;
        using this_class = IOscilloscope;

    public:
        using base_class::mode_type;
        using base_class::size_type;
        using base_class::dimensions_type;
        using base_class::pointer_type;

        using visualisation_type = OscilloscopeType;
        using config_type        = Config::OscilloscopeConfig;

        using sample_type    = typename WaveformParams::sample_type;
        using peak_type      = typename WaveformParams::peak_type;
        using transform_type = typename WaveformParams::transform_type;

        using coord_type = GLint;

    public:
        static pointer_type MakeVisualisation(visualisation_type eType,
                                              const config_type& config,
                                              const dimensions_type& dim,
                                              bool bMonochrome);

        static constexpr auto Mode() noexcept { return mode_type::Oscilloscope; }

    protected:
        IOscilloscope(visualisation_type eType,
                      const config_type& config,
                      const dimensions_type& dim) noexcept :
            IVisualisation{ Mode(), static_cast<size_type>(eType), dim },
            m_Config{ config } {}

    protected:
        constexpr auto& Config() const noexcept { return m_Config; }

    private:
        config_type m_Config{};
    }; // IOscilloscope
} // namespace Visualisation::Oscilloscope

#endif // GUID_0FBAACAB_7E2A_4A0C_814F_9DB4FC0B535D
