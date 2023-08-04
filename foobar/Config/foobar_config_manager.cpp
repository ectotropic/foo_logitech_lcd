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
#include "foobar/Config/foobar_config_manager.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/Config/foobar_config_common.h"
#include "foobar/Config/foobar_config_general.h"
#include "foobar/Config/foobar_config_spectrum_analyser.h"
#include "foobar/Config/foobar_config_oscilloscope.h"
#include "foobar/Config/foobar_config_vu_meter.h"
#include "foobar/Config/foobar_config_track_details.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using cfg_id_type = GUID;

    inline static constexpr const std::uint32_t vfoobarConfigVersion{ 1 };

    //**************************************************************************
    // FoobarConfigVersion
    //**************************************************************************
    static constexpr const cfg_id_type id_foobarConfigVersion{ 0x1fe0044, 0x5933, 0x4e3a, { 0x85, 0x5c, 0x40, 0xd7, 0xbb, 0x2b, 0x24, 0x24 } };
    static cfg_int_t<std::uint32_t> cfg_foobarConfigVersion{ id_foobarConfigVersion, vfoobarConfigVersion };

    //**************************************************************************
    //
    //**************************************************************************
    template <typename... ArgPackT>
    void foobar_complain(ArgPackT&& ...args) noexcept {
        try {
            ::console::complain(std::forward<ArgPackT>(args)...);
        } catch (...) {
            assert(false);
        }
    }

    //--------------------------------------------------------------------------

    template <typename... ArgPackT>
    void foobar_error(ArgPackT&& ...args) noexcept {
        try {
            ::console::error(std::forward<ArgPackT>(args)...);
        } catch (...) {
            assert(false);
        }
    }
}

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // ConfigManager
    //**************************************************************************
    void ConfigManager::initialise() {
        IConfigManager::initialise<ConfigManager>();
        IConfigManager::instance()->Load();
    }

    //--------------------------------------------------------------------------

    void ConfigManager::Load() noexcept {
        try {
            if (cfg_foobarConfigVersion != vfoobarConfigVersion) {
                // convert to newer version...
                SPDLOG_ERROR("Config version mismatch: expected {}, got {}.",
                             vfoobarConfigVersion,
                             cfg_foobarConfigVersion.get_value());
            }

            m_Config.m_GeneralConfig = GeneralConfig::load();

            m_Config.m_SpectrumAnalyser = SpectrumAnalyserConfig::load();
            m_Config.m_Oscilloscope = OscilloscopeConfig::load();
            m_Config.m_VUMeter = VUMeterConfig::load();
            m_Config.m_TrackDetails = TrackDetailsConfig::load();

            for (const auto i : TrackDetailsConfig::native_enum_type{}) {
                m_Config.m_TrackDetails[i].m_Text.m_bRestrictedLineCount = !m_Config.m_GeneralConfig.m_bExpertMode;
            }
        } catch (std::exception const& e) {
            ::foobar_complain("[foo_logitech_lcd] Failed to load configuration!", e);
        } catch (...) {
            ::foobar_error("[foo_logitech_lcd] Failed to load configuration!");
        }
    }

    //-------------------------------------------------------------------------

    void ConfigManager::Save() noexcept {
        try {
            SpectrumAnalyserConfig::save(m_Config.m_SpectrumAnalyser);
            OscilloscopeConfig::save(m_Config.m_Oscilloscope);
            VUMeterConfig::save(m_Config.m_VUMeter);
            TrackDetailsConfig::save(m_Config.m_TrackDetails);
            GeneralConfig::save(m_Config.m_GeneralConfig);
        } catch (std::exception const& e) {
            ::foobar_complain("[foo_logitech_lcd] Failed to load configuration!", e);
        } catch (...) {
            ::foobar_error("[foo_logitech_lcd] Failed to load configuration!");
        }
    }

    //-------------------------------------------------------------------------

    void ConfigManager::SetDefault(config_data_type& config) const noexcept {
        try {
            SpectrumAnalyserConfig::set_default(config.m_SpectrumAnalyser);
            OscilloscopeConfig::set_default(config.m_Oscilloscope);
            VUMeterConfig::set_default(config.m_VUMeter);
            TrackDetailsConfig::set_default(config.m_TrackDetails);
            GeneralConfig::set_default(config.m_GeneralConfig);
        } catch (std::exception const& e) {
            ::foobar_complain("[foo_logitech_lcd] Failed set default configuration!", e);
        } catch (...) {
            ::foobar_error("[foo_logitech_lcd] Failed set default configuration!");
        }
    }

} // namespace foobar::Config
