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
#include "foobar/Config/foobar_config_spectrum_analyser.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using native_config_type = typename foobar::Config::SpectrumAnalyserConfig::native_config_type;
    using native_array_type  = typename foobar::Config::SpectrumAnalyserConfig::native_array_type;
    using native_enum_type   = typename foobar::Config::SpectrumAnalyserConfig::native_enum_type;

    //---------------------------------------

    inline static constexpr const auto cSpectrumAnalyserConfigCount  { native_enum_type::count()   };
    inline static constexpr const auto vSpectrumAnalyserConfigVersion{ native_config_type::Version };
    inline static constexpr const auto nSpectrumAnalyserConfigSize   { sizeof(native_config_type)  };

    //---------------------------------------

    using cfg_id_type           = GUID;

    using cfg_spectrum_analyser = cfg_struct_t<native_config_type>;
    using cfg_pages             = std::array<cfg_spectrum_analyser, cSpectrumAnalyserConfigCount>;

    //**************************************************************************
    // Storage
    //**************************************************************************
    static cfg_pages cfg_SpectrumAnalyserConfig{
        //---------------------------------------
        // native_enum_type::Mono
        cfg_spectrum_analyser{
            cfg_id_type{ 0x966a6928, 0x42dd, 0x40d0, { 0xa2, 0x35, 0x08, 0xe0, 0xd3, 0x35, 0x53, 0xfa } },
            native_config_type{ native_enum_type::Mono }
        },

        //---------------------------------------
        // native_enum_type::MonoBlock
        cfg_spectrum_analyser{
            cfg_id_type{ 0x8601d444, 0x0ebc, 0x48e9, { 0xbd, 0x7f, 0xff, 0x4c, 0x1e, 0xf4, 0x9f, 0x43 } },
            native_config_type{ native_enum_type::MonoBlock }
        },

        //---------------------------------------
        // native_enum_type::Stereo
        cfg_spectrum_analyser{
            cfg_id_type{ 0xeadca867, 0x5f29, 0x427b, { 0x8b, 0x67, 0x1b, 0x1b, 0xef, 0xc2, 0x26, 0x72 } },
            native_config_type{ native_enum_type::Stereo }
        },

        //---------------------------------------
        // native_enum_type::StereoBlock
        cfg_spectrum_analyser{
            cfg_id_type{ 0x6610f858, 0xb1df, 0x4b49, { 0x96, 0x85, 0xe3, 0xd1, 0x97, 0x4a, 0xca, 0xfb } },
            native_config_type{ native_enum_type::StereoBlock }
        },
    }; // cfg_track_details_array_t cfg_SpectrumAnalyserConfig

    //-----------------------------------------------------

    cfg_uint cfg_SpectrumAnalyserConfigVersion{
        cfg_id_type{ 0x72232baa, 0xf103, 0x4b3e, { 0x83, 0x47, 0x7e, 0xf5, 0xce, 0x44, 0x1d, 0x9b } },
        vSpectrumAnalyserConfigVersion
    };

    cfg_uint cfg_SpectrumAnalyserConfigSize{
        cfg_id_type{ 0xbcca8e12, 0xb8cf, 0x4ff0, { 0x82, 0xd8, 0x80, 0xfc, 0xe5, 0x5f, 0xb9, 0xad } },
        nSpectrumAnalyserConfigSize
    };
} // namespace <anonymous>

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // SpectrumAnalyserConfig
    //**************************************************************************
    SpectrumAnalyserConfig::native_config_type SpectrumAnalyserConfig::load(native_enum_type type) {
        native_config_type native{};
        if (cfg_SpectrumAnalyserConfigVersion == vSpectrumAnalyserConfigVersion &&
            cfg_SpectrumAnalyserConfigSize    == nSpectrumAnalyserConfigSize) {
            foobar::Config::cfg_load(native, cfg_SpectrumAnalyserConfig[type]);
        } else {
            // Convert config to newer version when possible
            // ...
            // Otherwise:
            SPDLOG_ERROR("Error loading Spectrum Analyser config, version/size mismatch: expected v{}/{}b, got v{}/{}b. Config has been reset.",
                         vSpectrumAnalyserConfigVersion,
                         nSpectrumAnalyserConfigSize,
                         cfg_SpectrumAnalyserConfigVersion.get_value(),
                         cfg_SpectrumAnalyserConfigSize.get_value());
            set_default(type, native);
            foobar::Config::cfg_save(cfg_SpectrumAnalyserConfig[type], native);
        }
        return native;
    }

    //--------------------------------------------------------------------------

    SpectrumAnalyserConfig::native_array_type SpectrumAnalyserConfig::load() {
        native_array_type native{};
        for (const auto i : native_enum_type{}) { native[i] = load(i); }
        return native;
    }

    //--------------------------------------------------------------------------

    void SpectrumAnalyserConfig::save(native_enum_type type,
                             const native_config_type& native) {
        foobar::Config::cfg_save(cfg_SpectrumAnalyserConfigVersion, vSpectrumAnalyserConfigVersion);
        foobar::Config::cfg_save(cfg_SpectrumAnalyserConfigSize,    nSpectrumAnalyserConfigSize);
        foobar::Config::cfg_save(cfg_SpectrumAnalyserConfig[type],  native);
    }

    //--------------------------------------------------------------------------

    void SpectrumAnalyserConfig::save(const native_array_type& native) {
        for (const auto i : native_enum_type{}) { save(i, native[i]); }
    }

    //--------------------------------------------------------------------------

    void SpectrumAnalyserConfig::set_default(typename native_enum_type /*type*/,
                                             native_config_type& native) noexcept {
        native = native_config_type{};
    }

    //--------------------------------------------------------------------------

    void SpectrumAnalyserConfig::set_default(native_array_type& config) noexcept {
        for (const auto i : native_enum_type{}) { set_default(i, config[i]); }
    }
} // namespace foobar::Config
