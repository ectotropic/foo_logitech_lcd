#pragma once
#ifndef GUID_CE8CF0A0_7342_4449_B1F9_D9F77187BEC5
#define GUID_CE8CF0A0_7342_4449_B1F9_D9F77187BEC5
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
#include "Config/Config_General.h"
//--------------------------------------

namespace foobar::Config {
    //**************************************************************************
    // GeneralConfig
    //**************************************************************************
    struct GeneralConfig final {
    public:
        using native_config_type = typename ::Config::GeneralConfig::config_type;

    public:
        static native_config_type load();

        static void save(const native_config_type& config);

        static void set_default(native_config_type& config) noexcept;
    }; // struct GeneralConfig final
} // namespace foobar::Config

#endif // GUID_CE8CF0A0_7342_4449_B1F9_D9F77187BEC5
