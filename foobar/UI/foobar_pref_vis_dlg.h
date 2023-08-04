#pragma once
#ifndef GUID_C7D4FED1_69B1_408A_B450_8E28A37493D8
#define GUID_C7D4FED1_69B1_408A_B450_8E28A37493D8
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
#include "Windows/UI/ATL_PreferencesDialog.h"
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include <type_traits>
//--------------------------------------

//******************************************************************************
// SVisPrefConfig
//******************************************************************************
struct SVisPrefConfig final {
    struct SCanvasConfig {
        bool bColor { false };
        int  nWidth { -1 };
        int  nHeight{ -1 };
    };

    using canvas_type = SCanvasConfig;
    using config_type = ::Config::ConfigData;

    SVisPrefConfig() = default;

    constexpr SVisPrefConfig(const canvas_type& canvas,
                             config_type* p_config) noexcept :
        Canvas { canvas },
        pConfig{ p_config } {};

    constexpr SVisPrefConfig(const canvas_type& canvas,
                             config_type& config) noexcept :
        Canvas { canvas },
        pConfig{ &config } {};

    constexpr SVisPrefConfig(bool bColor,
                             int nWidth,
                             int nHeight,
                             config_type* p_config) noexcept :
        Canvas { bColor, nWidth, nHeight },
        pConfig{ p_config } {};

    constexpr SVisPrefConfig(bool bColor,
                             int nWidth,
                             int nHeight,
                             config_type& config) noexcept :
        Canvas { bColor, nWidth, nHeight },
        pConfig{ &config } {};

    canvas_type  Canvas{};
    config_type* pConfig{ nullptr };
}; // struct SVisPrefConfig final

//==============================================================================

namespace foobar::UI {
    //**************************************************************************
    // CSequentialEnumHelperT
    //**************************************************************************
    template <typename EnumT>
    class CSequentialEnumHelperT {
    public:
        using enumType = EnumT;
        using dataType = DWORD_PTR;

        decltype(auto) begin() const noexcept { return enumType::begin(); }
        decltype(auto) end  () const noexcept { return enumType::end(); }

        decltype(auto) to_string(enumType e) const noexcept {
            return enumType::to_string(e);
        }

        decltype(auto) next(enumType e) const noexcept {
            return e + 1;
        }

        decltype(auto) to_enum(dataType d) const noexcept {
            return enumType::to_enum(d);
        }

        decltype(auto) to_data(enumType e) const noexcept {
            return static_cast<dataType>(enumType::to_int(e));
        }

    }; // class CSequentialEnumHelperT

    //**************************************************************************
    // CVisualisationPrefsDlgT
    //**************************************************************************
    template <typename VisualisationTypeT>
    class ATL_NO_VTABLE CVisualisationPrefsDlgT :
        public Windows::UI::CPreferencesDialog {
    private: // Internal Types
        using baseClass = Windows::UI::CPreferencesDialog;
        using thisClass = CVisualisationPrefsDlgT<VisualisationTypeT>;

    private: // Type trait helpers
        template <class OtherT, typename EnableT = bool>
        using enable_if_osc =
            std::enable_if<std::is_same_v<OtherT, ::OscilloscopeType>, EnableT>;

        template <class OtherT, typename EnableT = bool>
        using enable_if_osc_t =
            typename enable_if_osc<OtherT, EnableT>::type;

        template <class OtherT, typename EnableT = bool>
        using enable_if_spec =
            std::enable_if<std::is_same_v<OtherT, ::SpectrumAnalyserType>, EnableT>;

        template <class OtherT, typename EnableT = bool>
        using enable_if_spec_t =
            typename enable_if_spec<OtherT, EnableT>::type;

        template <class OtherT, typename EnableT = bool>
        using enable_if_vu =
            std::enable_if<std::is_same_v<OtherT, ::VUMeterType>, EnableT>;

        template <class OtherT, typename EnableT = bool>
        using enable_if_vu_t =
            typename enable_if_vu<OtherT, EnableT>::type;

        template <class OtherT, typename EnableT = bool>
        using enable_if_track_details =
            std::enable_if<std::is_same_v<OtherT, ::TrackDetailsType>, EnableT>;

        template <class OtherT, typename EnableT = bool>
        using enable_if_track_details_t =
            typename enable_if_track_details<OtherT, EnableT>::type;

    private:
        thisClass(thisClass&)            = delete; // No Copy
        thisClass& operator=(thisClass&) = delete; // No Copy

    protected: // Type Aliases
        using visualisation_mode = VisualisationMode;
        using visualisation_type = VisualisationTypeT;

    protected: // Construction
        CVisualisationPrefsDlgT() = default;

        // Using 'Windows::UI::CDialogImpl' makes constructors with
        // arguments difficult to use, so use this function
        // instead. All derived classes must call this to
        // initialise the data.
        template <class DerivedT>
        static auto MakeDialog(LPCTSTR szName,
                               visualisation_mode eMode,
                               visualisation_type eType,
                               const SVisPrefConfig& config) {
            ATLASSERT(szName && szName[0]);
            DerivedT::Ptr dialog{ new DerivedT{} };
            dialog->m_szName = szName;
            dialog->m_eMode  = eMode;
            dialog->m_eType  = eType;
            dialog->m_Config = config;
            return dialog;
        }

    private: // Config access helpers

        template <typename TypeT, enable_if_osc_t<TypeT> = true>
        constexpr auto& VisConfigT() const noexcept {
            return Config().m_Oscilloscope[m_eType];
        }

        template <typename TypeT, enable_if_spec_t<TypeT> = true>
        constexpr auto& VisConfigT() const noexcept {
            return Config().m_SpectrumAnalyser[m_eType];
        }

        template <typename TypeT, enable_if_vu_t<TypeT> = true>
        constexpr auto& VisConfigT() const noexcept {
            return Config().m_VUMeter[m_eType];
        }

        template <typename TypeT, enable_if_track_details_t<TypeT> = true>
        constexpr auto& VisConfigT() const noexcept {
            return Config().m_TrackDetails[m_eType];
        }

    public: // Accessors
        virtual ~CVisualisationPrefsDlgT() {}

        constexpr auto Name() const noexcept { return m_szName; }
        constexpr auto Mode() const noexcept { return m_eMode; }
        constexpr auto Type() const noexcept { return m_eType; }

        constexpr auto& VisPrefConfig() const noexcept { return m_Config; }

        constexpr auto& CanvasConfig() const noexcept { return VisPrefConfig().Canvas; }

        constexpr auto CanvasIsColor() const noexcept { return CanvasConfig().bColor; }
        constexpr auto CanvasWidth  () const noexcept { return CanvasConfig().nWidth;  }
        constexpr auto CanvasHeight () const noexcept { return CanvasConfig().nHeight; }

        constexpr auto& Config   () const noexcept { return *VisPrefConfig().pConfig; }
        constexpr auto& VisConfig() const noexcept { return VisConfigT<visualisation_type>(); }

        constexpr auto IsEnabled() const noexcept { return VisConfig().m_bEnabled; }

        constexpr auto GetStartWithThis() const noexcept {
            const auto& start{ Config().m_GeneralConfig.m_Visualisation.m_Start };
            if (start.m_bRememberLast) {
                return false;
            }
            if (start.m_LastMode != m_eMode) {
                return false;
            }
            if (start.m_LastIndex != static_cast<size_t>(m_eType)) {
                return false;
            }
            return IsEnabled();
        }

        constexpr auto StartWithThisAvailable() const noexcept {
            const auto& start{ Config().m_GeneralConfig.m_Visualisation.m_Start };
            if (start.m_bRememberLast) {
                return false;
            }
            if (start.m_LastMode  == m_eMode &&
                start.m_LastIndex == static_cast<size_t>(m_eType)) {
                return false;
            }
            return IsEnabled();
        }

        constexpr auto SetStartWithThis() noexcept {
            auto& start{ Config().m_GeneralConfig.m_Visualisation.m_Start };
            const auto oldMode = start.m_LastMode;
            const auto oldType = start.m_LastIndex;
            start.m_LastMode  = m_eMode;
            start.m_LastIndex = static_cast<size_t>(m_eType);
            return !(oldMode == start.m_LastMode &&
                     oldType == start.m_LastIndex);
        }

    private: // Data
        LPCTSTR            m_szName{ NULL };
        visualisation_mode m_eMode { VisualisationMode::ModeError };
        visualisation_type m_eType {};
        SVisPrefConfig     m_Config{};
    }; // template <...> class CVisualisationPrefsDlgT
} // namespace foobar::UI

#endif // GUID_C7D4FED1_69B1_408A_B450_8E28A37493D8
