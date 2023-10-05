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
#include "foobar/foobar_sdk.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/Config/foobar_config_vu_meter.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using native_config_type = typename foobar::Config::VUMeterConfig::native_config_type;
    using native_array_type  = typename foobar::Config::VUMeterConfig::native_array_type;
    using native_enum_type   = typename foobar::Config::VUMeterConfig::native_enum_type;

    //------------------------------------------------------

    inline static constexpr const auto cVUMeterConfigCount  { native_enum_type::count()   };
    inline static constexpr const auto vVUMeterConfigVersion{ native_config_type::Version };
    inline static constexpr const auto nVUMeterConfigSize   { sizeof(native_config_type)  };

    //------------------------------------------------------

    using cfg_id_type = GUID;

    using cfg_vumeter = cfg_struct_t<native_config_type>;
    using cfg_pages   = std::array<cfg_vumeter, cVUMeterConfigCount>;

    //**************************************************************************
    // Storage
    //**************************************************************************
    static cfg_pages cfg_VUMeterConfig{
        //---------------------------------------
        // native_enum_type::Meter1
        cfg_vumeter{
            cfg_id_type{ 0xd9389971, 0x67ad, 0x420e, { 0xb4, 0x97, 0xa1, 0x94, 0x47, 0xcf, 0xc0, 0xd0 } },
            native_config_type{ native_enum_type::Meter1 }
        },

        //---------------------------------------
        // native_enum_type::Meter2
        cfg_vumeter{
            cfg_id_type{ 0x3b8c106e, 0x8960, 0x4b1d, { 0xa2, 0x8c, 0x5e, 0x40, 0x3e, 0x41, 0x6c, 0x63 } },
            native_config_type{ native_enum_type::Meter2 }
        },

        //---------------------------------------
        // native_enum_type::Image
        cfg_vumeter{
            cfg_id_type{ 0x56c49495, 0x4208, 0x4794, { 0x8f, 0xab, 0x14, 0xb9, 0x11, 0xa1, 0x95, 0x7f } },
            native_config_type{ native_enum_type::Image }
        },
    };

    //------------------------------------------------------

    cfg_size_t cfg_VUMeterConfigVersion{
        cfg_id_type{ 0xc85d1012, 0xee65, 0x4eec, { 0x8b, 0xde, 0x6f, 0x33, 0x42, 0xc6, 0x14, 0x09 } },
        vVUMeterConfigVersion
    };

    cfg_size_t cfg_VUMeterConfigSize{
        cfg_id_type{ 0x54e83096, 0xb11e, 0x43cc, { 0x9b, 0x43, 0xf8, 0x92, 0x76, 0x61, 0x6f, 0x4b } },
        nVUMeterConfigSize
    };
} // namespace <anonymous>

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // VUMeterConfig
    //**************************************************************************
    VUMeterConfig::native_config_type VUMeterConfig::load(native_enum_type type) {
        native_config_type native{};
        if (cfg_VUMeterConfigVersion == vVUMeterConfigVersion &&
            cfg_VUMeterConfigSize    == nVUMeterConfigSize) {
            foobar::Config::cfg_load(native, cfg_VUMeterConfig[type]);
        } else {
            // Convert config to newer version when possible
            // ...
            // Otherwise:
            SPDLOG_ERROR("Error loading VU Meter config, version/size mismatch: expected v{}/{}b, got v{}/{}b. Config has been reset.",
                         vVUMeterConfigVersion,
                         nVUMeterConfigSize,
                         cfg_VUMeterConfigVersion.get_value(),
                         cfg_VUMeterConfigSize.get_value());
            set_default(type, native);
            foobar::Config::cfg_save(cfg_VUMeterConfig[type], native);
        }
        return native;
    }

    //------------------------------------------------------

    VUMeterConfig::native_array_type VUMeterConfig::load() {
        native_array_type native{};
        for (const auto i : native_enum_type{}) { native[i] = load(i); }
        return native;
    }

    //------------------------------------------------------

    void VUMeterConfig::save(native_enum_type type,
                             const native_config_type& native) {
        foobar::Config::cfg_save(cfg_VUMeterConfigVersion, vVUMeterConfigVersion);
        foobar::Config::cfg_save(cfg_VUMeterConfigSize,    nVUMeterConfigSize);
        foobar::Config::cfg_save(cfg_VUMeterConfig[type],  native);
    }

    //------------------------------------------------------

    void VUMeterConfig::save(const native_array_type& native) {
        for (const auto i : native_enum_type{}) { save(i, native[i]); }
    }

    //------------------------------------------------------

    void VUMeterConfig::set_default(typename native_enum_type /*type*/,
                                    native_config_type& native) noexcept {
        native = native_config_type{};
    }

    //------------------------------------------------------

    void VUMeterConfig::set_default(native_array_type& config) noexcept {
        for (const auto i : native_enum_type{}) { set_default(i, config[i]); }
    }
} // namespace foobar::Config
