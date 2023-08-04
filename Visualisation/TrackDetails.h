#pragma once
#ifndef GUID_785A8A45_4CFD_4DC6_BCB2_447F7C1CA37A
#define GUID_785A8A45_4CFD_4DC6_BCB2_447F7C1CA37A
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
#include "Config/Config_TrackDetails.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
//--------------------------------------

//--------------------------------------
//
#include "Config/Config_Manager.h"
//--------------------------------------

namespace Visualisation::Text {
    //**************************************************************************
    // ITrackDetails
    //**************************************************************************
    class ITrackDetails : public IVisualisation {
    private:
        using base_class = IVisualisation;
        using this_class = ITrackDetails;

    public:
        using base_class::size_type;
        using base_class::dimensions_type;
        using base_class::pointer_type;

        using page_type   = ::TrackDetailsType;
        using config_type = Config::TrackDetailsConfig;

        using string_type = std::wstring;

        using coord_type = ::INT;

    public:
        static pointer_type MakeVisualisation(page_type ePage,
                                              const config_type& config,
                                              const dimensions_type& dim,
                                              bool bMonochrome);

        static constexpr auto Mode() noexcept { return VisualisationMode::TrackDetails; }

    protected:
        ITrackDetails(page_type ePage,
                      const config_type& config,
                      const dimensions_type& dim,
                      VisualisationFlags flags) noexcept :
            IVisualisation{ Mode(), ePage, dim, flags },
            m_Config{ config } {}

    protected:
        constexpr const auto& Config() const noexcept { return m_Config; }

        constexpr auto GetPage() const noexcept { return page_type::to_enum(base_class::GetIndex()); }

    private: //Copy of config
        config_type m_Config{};
    }; // class ITrackDetails
} // namespace Visualisation::Text

#endif // GUID_785A8A45_4CFD_4DC6_BCB2_447F7C1CA37A