#pragma once
#ifndef GUID_9839D973_A609_44DB_9EEB_2FE52C926084
#define GUID_9839D973_A609_44DB_9EEB_2FE52C926084
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
#include "Config/Config_SpectrumAnalyser.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
//--------------------------------------

namespace Visualisation::SpectrumAnalyser {
    //**************************************************************************
    // ISpectrumAnalyser
    //**************************************************************************
    class ISpectrumAnalyser : public IVisualisation {
    private:
        using base_class = IVisualisation;
        using this_class = ISpectrumAnalyser;

    public:
        using base_class::mode_type;
        using base_class::size_type;
        using base_class::dimensions_type;
        using base_class::pointer_type;

        using visualisation_type = SpectrumAnalyserType;
        using config_type        = Config::SpectrumAnalyserConfig;

        using sample_type    = typename SpectrumParams::sample_type;
        using peak_type      = typename SpectrumParams::peak_type;
        using transform_type = typename SpectrumParams::transform_type;

        using coord_type = GLint;

    public:
        static pointer_type MakeVisualisation(visualisation_type eType,
                                              const config_type& config,
                                              const dimensions_type& dim,
                                              bool bMonochrome);

        static constexpr auto Mode() noexcept { return mode_type::SpectrumAnalyser; }

    protected:
        ISpectrumAnalyser(visualisation_type eType,
                          const config_type& config,
                          const dimensions_type& dim) noexcept;

    protected:
        constexpr auto& Config() const noexcept { return m_Config; }

    private:
        config_type m_Config{};
    }; // class ISpectrumAnalyser
} // namespace Visualisation::SpectrumAnalyser

#endif // GUID_9839D973_A609_44DB_9EEB_2FE52C926084
