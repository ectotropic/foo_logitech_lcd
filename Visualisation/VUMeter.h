#pragma once
#ifndef GUID_517EE129_308D_4488_96DC_16979B04A109
#define GUID_517EE129_308D_4488_96DC_16979B04A109
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
#include "Config/Config_VUMeter.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
//--------------------------------------

namespace Visualisation::VUMeter {
    //**************************************************************************
    // IVUMeter
    //**************************************************************************
    class IVUMeter : public IVisualisation {
    private:
        using base_class = IVisualisation;
        using this_class = IVUMeter;

    public:
        using base_class::size_type;
        using base_class::dimensions_type;
        using base_class::pointer_type;

        using visualisation_type = ::VUMeterType;
        using config_type        = ::Config::VUMeterConfig;

        using sample_type    = typename DecibelParams::dB_type;
        using peak_type      = typename DecibelParams::peak_type;
        using transform_type = typename DecibelParams::transform_type;

        using coord_type = ::GLint;

    public:
        static pointer_type MakeVisualisation(visualisation_type eType,
                                              const config_type& config,
                                              const dimensions_type& dim,
                                              bool bMonochrome);

        static constexpr auto Mode() noexcept { return VisualisationMode::VUMeter; }

    protected:
        IVUMeter(visualisation_type eType,
                 const config_type& config,
                 const dimensions_type& dim) noexcept :
            IVisualisation{ Mode(), eType, dim },
            m_Config(config) {}

    protected:
        constexpr const auto& Config() const noexcept { return m_Config; }

    protected:
        config_type m_Config{};
    }; // class IVUMeter
} // namespace Visualisation::VUMeter

#endif // GUID_517EE129_308D_4488_96DC_16979B04A109
