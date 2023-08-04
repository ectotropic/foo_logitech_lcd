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
#include "Visualisation/TrackDetails.h"
#include "Visualisation/TrackDetails_Basic.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
//--------------------------------------

namespace Visualisation::Text {
    //**************************************************************************
    // ITrackDetails
    //**************************************************************************
    ITrackDetails::pointer_type ITrackDetails::MakeVisualisation(page_type ePage,
                                                                 const config_type& config,
                                                                 const dimensions_type& dim,
                                                                 bool bMonochrome) {
        return std::make_shared<Visualisation::Text::TrackDetails>(ePage, config, dim, bMonochrome);
    }
} // namespace Visualisation::Text
