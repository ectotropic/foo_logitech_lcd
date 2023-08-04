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
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI_Font.h"
//--------------------------------------

namespace Config {
    //**************************************************************************
    // SpectrumAnalyserConfig
    //**************************************************************************
    const SpectrumAnalyserConfig::config_type& SpectrumAnalyserConfig::get(enum_type type) {
        return IConfigManager::instance()->GetSpectrumAnalyserConfig(type);
    }

    const SpectrumAnalyserConfig::array_type& SpectrumAnalyserConfig::get() {
        return IConfigManager::instance()->GetSpectrumAnalyserConfig();
    }

    //**************************************************************************
    // OscilloscopeConfig
    //**************************************************************************
    const OscilloscopeConfig::config_type& OscilloscopeConfig::get(enum_type type) {
        return IConfigManager::instance()->GetOscilloscopeConfig(type);
    }

    const OscilloscopeConfig::array_type& OscilloscopeConfig::get() {
        return IConfigManager::instance()->GetOscilloscopeConfig();
    }

    //**************************************************************************
    // VUMeterConfig
    //**************************************************************************
    const VUMeterConfig::config_type& VUMeterConfig::get(enum_type type) {
        return IConfigManager::instance()->GetVUMeterConfig(type);
    }

    const VUMeterConfig::array_type& VUMeterConfig::get() {
        return IConfigManager::instance()->GetVUMeterConfig();
    }

    //**************************************************************************
    // TrackDetailsConfig::TextConfig
    //**************************************************************************
    TrackDetailsConfig::TextConfig::TextConfig() noexcept {
        ZeroMemory(&m_Font, sizeof(LOGFONT));
        WinAPIVerify(Windows::GDI::GetSystemFont(Windows::GDI::SYS_FONT_MESSAGE, 10, &m_Font));
    }

    //**************************************************************************
    // TrackDetailsConfig
    //**************************************************************************
    const TrackDetailsConfig::config_type& TrackDetailsConfig::get(enum_type type) {
        return IConfigManager::instance()->GetTrackDetailsConfig(type);
    }

    const TrackDetailsConfig::array_type& TrackDetailsConfig::get() {
        return IConfigManager::instance()->GetTrackDetailsConfig();
    }

    //**************************************************************************
    // GeneralConfig
    //**************************************************************************
    const GeneralConfig::config_type& GeneralConfig::get() {
        return IConfigManager::instance()->GetGeneralConfig();
    }
} // namespace Config
