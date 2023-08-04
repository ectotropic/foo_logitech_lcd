#pragma once
#ifndef GUID_2C8FD489_6A62_4313_94FA_1E9BBF9CF4FC
#define GUID_2C8FD489_6A62_4313_94FA_1E9BBF9CF4FC
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

#include "Config/Config_SpectrumAnalyser.h"
#include "Config/Config_Oscilloscope.h"
#include "Config/Config_VUMeter.h"
#include "Config/Config_TrackDetails.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/Singleton.h"
//--------------------------------------

//--------------------------------------
//
#include <functional>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>
//--------------------------------------

namespace Config {
    //**************************************************************************
    // ConfigData
    //**************************************************************************
    struct ConfigData {
        GeneralConfig::config_type          m_GeneralConfig{};

        SpectrumAnalyserConfig::array_type  m_SpectrumAnalyser{};
        OscilloscopeConfig::array_type      m_Oscilloscope{};
        VUMeterConfig::array_type           m_VUMeter{};
        TrackDetailsConfig::array_type      m_TrackDetails{};
    }; // struct ConfigData

    //**************************************************************************
    // IConfigManager
    //**************************************************************************
    class IConfigManager : public ::util::singleton<IConfigManager> {
    public:
        using fnUpdateCallback        = std::function<void(void)>;
        using fnUpdateCallbackPtr     = std::shared_ptr<fnUpdateCallback>;
        using fnUpdateCallbackWeakPtr = typename fnUpdateCallbackPtr::weak_type;
        using UpdateCallbacks         = std::vector<fnUpdateCallbackWeakPtr>;
        using CallbackHandle          = fnUpdateCallbackPtr;

    protected:
        ~IConfigManager() = default;
        IConfigManager() = default;

    public:
        IConfigManager(const IConfigManager& other) noexcept = default;

        IConfigManager(ConfigData&& config) noexcept :
            m_Config{ std::forward<ConfigData>(config) } {}

        IConfigManager(IConfigManager&& other) noexcept :
            m_Config{ std::move(other.m_Config) },
            m_UpdateCallbacks{ std::move(other.m_UpdateCallbacks) } {}

        IConfigManager& operator=(const ConfigData& config) noexcept {
            m_Config = config;
            ConfigUpdated();
            return *this;
        }

        IConfigManager& operator=(ConfigData&& config) noexcept {
            m_Config = std::move(config);
            ConfigUpdated();
            return *this;
        }

        IConfigManager& operator=(const IConfigManager& other) noexcept {
            return IConfigManager::operator=(other.m_Config);
        }

        IConfigManager& operator=(IConfigManager&& other) noexcept {
            m_Config = std::move(other.m_Config);
            m_UpdateCallbacks = std::move(other.m_UpdateCallbacks);
            ConfigUpdated();
            return *this;
        }

        void RegisterUpdateCallback(fnUpdateCallbackPtr pCallback) {
            m_UpdateCallbacks.push_back(fnUpdateCallbackWeakPtr{ pCallback });
        }

        [[nodiscard]]
        auto RegisterUpdateCallback(fnUpdateCallback callback) {
            auto pCallback = std::make_shared<fnUpdateCallback>(callback);
            RegisterUpdateCallback(pCallback);
            return pCallback;
        }

        void UnregisterUpdateCallback(fnUpdateCallbackPtr pCallback) {
            const auto it = std::remove_if(m_UpdateCallbacks.begin(),
                                           m_UpdateCallbacks.end(),
                                           [&](auto pCB) noexcept { return pCB.lock() == pCallback; });
            m_UpdateCallbacks.erase(it, m_UpdateCallbacks.end());
        }

        void RemoveObsoleteCallbacks() {
            const auto it = std::remove_if(m_UpdateCallbacks.begin(),
                                           m_UpdateCallbacks.end(),
                                           [](auto pCB) noexcept { return pCB.expired(); });
            m_UpdateCallbacks.erase(it, m_UpdateCallbacks.end());
        }

        void ConfigUpdated() noexcept {
            for (const auto i : TrackDetailsConfig::enum_type{}) {
                m_Config.m_TrackDetails[i].m_Text.m_bRestrictedLineCount = !m_Config.m_GeneralConfig.m_bExpertMode;
            }

            Save();

            try {
                bool bHaveObsolete{ false };
                for (auto pWeakCallback : m_UpdateCallbacks) {
                    if (auto pCallback = pWeakCallback.lock()) {
                        (*pCallback)();
                    } else {
                        bHaveObsolete = true;
                    }
                }

                if (bHaveObsolete) {
                    RemoveObsoleteCallbacks();
                }
            } catch (...) {
                assert(false);
            }
        }

        auto GetDefault() const noexcept {
            Config::ConfigData config{};
            SetDefault(config);
            return config;
        }

        const auto& GetConfig() const noexcept { return m_Config; }

        void SetConfig(const ConfigData& config) noexcept {
            m_Config = config;
            ConfigUpdated();
        }

        void SetConfig(ConfigData&& config) noexcept {
            m_Config = std::move(config);
            ConfigUpdated();
        }

        const auto& GetGeneralConfig() const noexcept { return m_Config.m_GeneralConfig; }

        void SetGeneralConfig(GeneralConfig::config_type config) noexcept {
            m_Config.m_GeneralConfig = config;
            ConfigUpdated();
        }

        using spectrum_analyser_enum_type = typename SpectrumAnalyserConfig::enum_type;
        const auto& GetSpectrumAnalyserConfig()                                 const noexcept { return m_Config.m_SpectrumAnalyser; }
        const auto& GetSpectrumAnalyserConfig(spectrum_analyser_enum_type type) const noexcept { return m_Config.m_SpectrumAnalyser[type]; }

        void SetSpectrumAnalyserConfig(SpectrumAnalyserConfig::array_type config) noexcept {
            m_Config.m_SpectrumAnalyser = config;
            ConfigUpdated();
        }
        void SetSpectrumAnalyserConfig(spectrum_analyser_enum_type type,
                                       SpectrumAnalyserConfig::config_type config) noexcept {
            m_Config.m_SpectrumAnalyser[type] = config;
            ConfigUpdated();
        }

        using oscilloscope_enum_type = typename OscilloscopeConfig::enum_type;
        const auto& GetOscilloscopeConfig()                            const noexcept { return m_Config.m_Oscilloscope; }
        const auto& GetOscilloscopeConfig(oscilloscope_enum_type type) const noexcept { return m_Config.m_Oscilloscope[type]; }

        void SetOscilloscopeConfig(OscilloscopeConfig::array_type config) noexcept {
            m_Config.m_Oscilloscope = config;
            ConfigUpdated();
        }
        void SetOscilloscopeConfig(oscilloscope_enum_type type,
                                   OscilloscopeConfig::config_type config) noexcept {
            m_Config.m_Oscilloscope[type] = config;
            ConfigUpdated();
        }

        using vumeter_enum_type = typename VUMeterConfig::enum_type;
        const auto& GetVUMeterConfig()                       const noexcept { return m_Config.m_VUMeter; }
        const auto& GetVUMeterConfig(vumeter_enum_type type) const noexcept { return m_Config.m_VUMeter[type]; }

        void SetVUMeterConfig(VUMeterConfig::array_type config) noexcept {
            m_Config.m_VUMeter = config;
            ConfigUpdated();
        }
        void SetVUMeterConfig(vumeter_enum_type type,
                              VUMeterConfig::config_type config) noexcept {
            m_Config.m_VUMeter[type] = config;
            ConfigUpdated();
        }

        using track_details_enum_type = typename TrackDetailsConfig::enum_type;
        const auto& GetTrackDetailsConfig()                             const noexcept { return m_Config.m_TrackDetails; }
        const auto& GetTrackDetailsConfig(track_details_enum_type type) const noexcept { return m_Config.m_TrackDetails[type]; }

        void SetTrackDetailsConfig(TrackDetailsConfig::array_type config) noexcept {
            m_Config.m_TrackDetails = config;
            ConfigUpdated();
        }
        void SetTrackDetailsConfig(track_details_enum_type type,
                                   TrackDetailsConfig::config_type config) noexcept {
            m_Config.m_TrackDetails[type] = config;
            ConfigUpdated();
        }

    public:
        virtual void Load() noexcept = 0;
        virtual void Save() noexcept = 0;

    protected:
        using config_data_type = ConfigData;
        virtual void SetDefault(config_data_type& /*config*/) const noexcept {

        }

    protected:
        config_data_type m_Config{};

    private:
        UpdateCallbacks m_UpdateCallbacks{};
    }; // class IConfigManager
} // namespace Config

#endif // GUID_2C8FD489_6A62_4313_94FA_1E9BBF9CF4FC
