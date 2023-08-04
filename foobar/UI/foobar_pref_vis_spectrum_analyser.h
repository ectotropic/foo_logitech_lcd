#pragma once
#ifndef GUID_281B38A7_23AB_4061_B71D_7A74E1B63195
#define GUID_281B38A7_23AB_4061_B71D_7A74E1B63195
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
#include "foobar/UI/foobar_pref_vis_dlg.h"
#include "Windows/UI/ATL_EnumComboBox.h"
#include "Config/Config_SpectrumAnalyser.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI::detail {
    //**************************************************************************
    // CSpectrumAnalyserDialogCommon
    //**************************************************************************
    class CSpectrumAnalyserDialogCommon :
        public foobar::UI::CVisualisationPrefsDlgT<::SpectrumAnalyserType> {
    private: // Internal Types
        using thisClass = CSpectrumAnalyserDialogCommon;
        using baseClass =
            foobar::UI::CVisualisationPrefsDlgT<::SpectrumAnalyserType>;

        using CColorSwatchStatic =
            Windows::UI::CColorSwatchStatic;
        using CModeComboHelper =
            foobar::UI::CSequentialEnumHelperT<SpectrumAnalyserMode>;
        using CModeCombo =
            Windows::UI::CEnumComboBoxT<SpectrumAnalyserMode, CModeComboHelper>;

    protected: // Construction
        CSpectrumAnalyserDialogCommon() = default;

    protected: // Message Map Handlers
        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnUpdateUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled);

        LRESULT OnColorStatic(_In_ UINT uMsg,
                              _In_ WPARAM wParam,
                              _In_ LPARAM lParam,
                              _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled);

    private: //Message Map Helpers
        void UpdateControls();
        void EnableControls() noexcept;

    protected:
        INT m_nIDD{ 0 };

    private: // Data
        auto IsBlockSpecDialog() const noexcept { return m_nIDD == IDD_BLOCK_SPEC_CFG; }

    private: // Data
        enum {
            PrimarySwatch    = 0,
            SecondarySwatch  = 1,
            BackgroundSwatch = 2,
            SwatchCount
        };
        CColorSwatchStatic m_Color[SwatchCount]{};
        CModeCombo         m_ModeCombo{};
    }; //class CSpectrumAnalyserDialogCommon
} // namespace foobar::UI::detail

//==============================================================================

namespace foobar::UI {
    //**************************************************************************
    // CSpectrumAnalyserDlg
    //**************************************************************************
    class CSpectrumAnalyserDlg :
        public Windows::UI::CDialogImpl<CSpectrumAnalyserDlg,
                                        foobar::UI::detail::CSpectrumAnalyserDialogCommon> {
    private: // Internal Types
        using thisClass  = CSpectrumAnalyserDlg;
        using baseClass  = foobar::UI::detail::CSpectrumAnalyserDialogCommon;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

    public: // Construction
        CSpectrumAnalyserDlg() noexcept { m_nIDD = IDD; };

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum { IDD = IDD_SPEC_CFG, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
        END_MSG_MAP();

        using baseClass::OnInit;
        using baseClass::OnShow;
        using baseClass::OnUpdateUIState;
        using baseClass::OnColorStatic;
        using baseClass::OnCommand;
    }; // class CSpectrumAnalyserDlg

    //**************************************************************************
    // CBlockSpecDialog
    //**************************************************************************
    class CBlockSpecDialog :
        public Windows::UI::CDialogImpl<CBlockSpecDialog,
                                        foobar::UI::detail::CSpectrumAnalyserDialogCommon> {
    private: // Internal Types
        using thisClass  = CBlockSpecDialog;
        using baseClass  = foobar::UI::detail::CSpectrumAnalyserDialogCommon;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

    public: // Construction
        CBlockSpecDialog() noexcept { m_nIDD = IDD; };

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum { IDD = IDD_BLOCK_SPEC_CFG, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass);
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit);
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow);
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic);
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand);
        END_MSG_MAP();

        using baseClass::OnInit;
        using baseClass::OnShow;
        using baseClass::OnUpdateUIState;
        using baseClass::OnColorStatic;
        using baseClass::OnCommand;
    }; // class CBlockSpecDialog
} // namespace foobar::UI

#endif // GUID_281B38A7_23AB_4061_B71D_7A74E1B63195