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
#include "foobar/Config/foobar_config_oscilloscope.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using native_config_type = typename foobar::Config::OscilloscopeConfig::native_config_type;
    using native_array_type  = typename foobar::Config::OscilloscopeConfig::native_array_type;
    using native_enum_type   = typename foobar::Config::OscilloscopeConfig::native_enum_type;

    //---------------------------------------

    inline static constexpr const auto cOscilloscopeConfigCount  { native_enum_type::count()   };
    inline static constexpr const auto vOscilloscopeConfigVersion{ native_config_type::Version };
    inline static constexpr const auto nOscilloscopeConfigSize   { sizeof(native_config_type)  };

    //---------------------------------------

    using cfg_id_type      = GUID;

    using cfg_oscilloscope = cfg_struct_t<native_config_type>;
    using cfg_pages        = std::array<cfg_oscilloscope, cOscilloscopeConfigCount>;

    //**************************************************************************
    // Storage
    //**************************************************************************
    static cfg_pages cfg_OscilloscopeConfig{
        //---------------------------------------
        // native_enum_type::DotMono
        cfg_oscilloscope{
            cfg_id_type{ 0x830eddab, 0x3734, 0x449a, { 0x8b, 0xd5, 0xa1, 0x7d, 0x9e, 0xcc, 0xa8, 0x94 } },
            native_config_type{ native_enum_type::DotMono }
        },

        //---------------------------------------
        // native_enum_type::DotStereo
        cfg_oscilloscope{
            cfg_id_type{ 0x555ba44f, 0xcfef, 0x42c3, { 0x89, 0x5f, 0x67, 0x27, 0xab, 0x66, 0x5c, 0x8e } },
            native_config_type{ native_enum_type::DotStereo }
        },

        //---------------------------------------
        // native_enum_type::LineMono
        cfg_oscilloscope{
            cfg_id_type{ 0x6d403ec3, 0x0d11, 0x470c, { 0x8b, 0x23, 0x43, 0xec, 0xb0, 0x7e, 0xf0, 0x7c } },
            native_config_type{ native_enum_type::LineMono }
        },

        //---------------------------------------
        // native_enum_type::LineStereo
        cfg_oscilloscope{
            cfg_id_type{ 0x9a1455dc, 0xa40d, 0x42ca, { 0xb0, 0x33, 0x72, 0x39, 0xb0, 0xa0, 0xc4, 0x07 } },
            native_config_type{ native_enum_type::LineStereo }
         },

        //---------------------------------------
        // native_enum_type::CircleMono
        cfg_oscilloscope{
            cfg_id_type{ 0x6e5588d8, 0xdfbb, 0x4ef9, { 0x81, 0x03, 0xf8, 0xec, 0x83, 0x45, 0x6e, 0xec } },
            native_config_type{ native_enum_type::CircleMono }
        },

        //---------------------------------------
        // native_enum_type::CircleStereo
        /*
        cfg_oscilloscope{
            cfg_id_type{ 0x69740976, 0x0f7e, 0x4584, { 0xb3, 0x0a, 0x53, 0xd0, 0x5d, 0x4d, 0x19, 0x11 } },
            native_config_type{ native_enum_type::CircleStereo }
        },
        */

        //---------------------------------------
        // native_enum_type::StarBurstMono
        cfg_oscilloscope{
            cfg_id_type{ 0x3f82f839, 0x0ef5, 0x474b, { 0x8f, 0xb2, 0xf7, 0xd5, 0x8e, 0x35, 0xa0, 0xb6 } },
            native_config_type{ native_enum_type::StarBurstMono }
        },

        //---------------------------------------
        // native_enum_type::StarBurstStereo
        /*
        cfg_oscilloscope_t{
            cfg_id_type{ 0xbf174102, 0x7795, 0x46c0, { 0x81, 0xc5, 0x88, 0x1d, 0xa4, 0xc1, 0x8b, 0xbd } },
            native_config_type{ native_enum_type::StarBurstStereo }
        },
        */
    }; // static cfg_pages cfg_OscilloscopeConfig

    //-----------------------------------------------------

    cfg_size_t cfg_OscilloscopeConfigVersion{
        cfg_id_type{ 0x72232baa, 0xf103, 0x4b3e, { 0x83, 0x47, 0x7e, 0xf5, 0xce, 0x44, 0x1d, 0x9b } },
        vOscilloscopeConfigVersion
    };

    cfg_size_t cfg_OscilloscopeConfigSize{
        cfg_id_type{ 0xbcca8e12, 0xb8cf, 0x4ff0, { 0x82, 0xd8, 0x80, 0xfc, 0xe5, 0x5f, 0xb9, 0xad } },
        nOscilloscopeConfigSize
    };
} // namespace <anonymous>

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // OscilloscopeConfig
    //**************************************************************************
    OscilloscopeConfig::native_config_type OscilloscopeConfig::load(native_enum_type type) {
        native_config_type native{};
        if (cfg_OscilloscopeConfigVersion == vOscilloscopeConfigVersion &&
            cfg_OscilloscopeConfigSize    == nOscilloscopeConfigSize) {
            foobar::Config::cfg_load(native, cfg_OscilloscopeConfig[type]);
        } else {
            // Convert config to newer version when possible
            // ...
            // Otherwise:
            SPDLOG_ERROR("Error loading Oscilloscope config, version/size mismatch: expected v{}/{}b, got v{}/{}b. Config has been reset.",
                         vOscilloscopeConfigVersion,
                         nOscilloscopeConfigSize,
                         cfg_OscilloscopeConfigVersion.get_value(),
                         cfg_OscilloscopeConfigSize.get_value());
            set_default(type, native);
            foobar::Config::cfg_save(cfg_OscilloscopeConfig[type], native);
        }
        return native;
    }

    //--------------------------------------------------------------------------

    OscilloscopeConfig::native_array_type OscilloscopeConfig::load() {
        native_array_type native{};
        for (const auto i : native_enum_type{}) { native[i] = load(i); }
        return native;
    }

    //--------------------------------------------------------------------------

    void OscilloscopeConfig::save(typename native_enum_type type,
                                  const native_config_type& native) {
        foobar::Config::cfg_save(cfg_OscilloscopeConfigVersion, vOscilloscopeConfigVersion);
        foobar::Config::cfg_save(cfg_OscilloscopeConfigSize,    nOscilloscopeConfigSize);
        foobar::Config::cfg_save(cfg_OscilloscopeConfig[type],  native);
    }

    //--------------------------------------------------------------------------

    void OscilloscopeConfig::save(const native_array_type& native) {
        for (const auto i : native_enum_type{}) { save(i, native[i]); }
    }

    //--------------------------------------------------------------------------

    void OscilloscopeConfig::set_default(typename native_enum_type /*type*/,
                                         native_config_type& native) noexcept {
        native = native_config_type{};
    }

    //--------------------------------------------------------------------------

    void OscilloscopeConfig::set_default(native_array_type& config) noexcept {
        for (const auto i : native_enum_type{}) { set_default(i, config[i]); }
    }
} // namespace foobar::Config
