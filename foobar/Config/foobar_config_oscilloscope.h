#pragma once
#ifndef GUID_6338991C_6D29_483C_A3A9_D49A6AD29509
#define GUID_6338991C_6D29_483C_A3A9_D49A6AD29509
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
#include "Config/Config_Oscilloscope.h"
//--------------------------------------

namespace foobar::Config {
    //**************************************************************************
    // OscilloscopeConfig
    //**************************************************************************
    struct OscilloscopeConfig final {
    public:
        using native_config_type = typename ::Config::OscilloscopeConfig::config_type;
        using native_array_type  = typename native_config_type::array_type;
        using native_enum_type   = typename native_config_type::enum_type;

    public:
        static native_config_type load(native_enum_type type);
        static native_array_type  load();

        static void save(native_enum_type type,
                         const native_config_type& config);
        static void save(const native_array_type& config);

        static void set_default(native_enum_type type,
                                native_config_type& config) noexcept;
        static void set_default(native_array_type& config) noexcept;
    }; // struct OscilloscopeConfig final
} // namespace foobar::Config

#endif // GUID_6338991C_6D29_483C_A3A9_D49A6AD29509
