#pragma once
#ifndef GUID_A0FC715C_6F74_4B34_B433_4BB30095B146
#define GUID_A0FC715C_6F74_4B34_B433_4BB30095B146
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
#include "Config/Config_Manager.h"
//--------------------------------------

namespace foobar::Config {
    //**************************************************************************
    // ConfigManager
    //**************************************************************************
    class ConfigManager final :
        public ::Config::IConfigManager {
    public:
        ConfigManager(singleton_constructor_tag /*tag*/) noexcept {}

    public:
        static void initialise();

        virtual void Load() noexcept override;
        virtual void Save() noexcept override;

    protected:
        virtual void SetDefault(config_data_type& config) const noexcept override;
    }; // class ConfigManager final
} // namespace foobar::Config

#endif // GUID_A0FC715C_6F74_4B34_B433_4BB30095B146
