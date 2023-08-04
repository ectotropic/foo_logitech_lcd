#pragma once
#ifndef GUID_4AB5E457_8034_4027_A1F6_EB333126143E
#define GUID_4AB5E457_8034_4027_A1F6_EB333126143E
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
#include "Config/Config_VUMeter.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CVUMeterImageDlg
    //**************************************************************************
    class CVUMeterImageDlg :
        public Windows::UI::CDialogImpl<CVUMeterImageDlg,
                                        CVisualisationPrefsDlgT<VUMeterType>> {
    private: // Internal Types
        using thisClass  = CVUMeterImageDlg;
        using baseClass  = CVisualisationPrefsDlgT<VUMeterType>;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

    public: // Construction
        CVUMeterImageDlg() noexcept = default;

    public: // Public Types
        using Ptr = ATL::CAutoPtr<thisClass>;
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
        enum { IDD = IDD_VU_PIC_CFG, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG   , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW   , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUIState)
            MESSAGE_HANDLER(WM_COMMAND      , OnCommand)
        END_MSG_MAP();

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

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled);

    private: //Message Map Helpers
        void UpdateControls();
        void EnableControls() noexcept;
    }; // class CVUMeterImageDlg

    //**************************************************************************
    // CVUMeterDlg
    //**************************************************************************
    class CVUMeterDlg :
        public Windows::UI::CDialogImpl<CVUMeterDlg,
                                        CVisualisationPrefsDlgT<VUMeterType>> {
    private: // Internal Types
        using thisClass  = CVUMeterDlg;
        using baseClass  = CVisualisationPrefsDlgT<VUMeterType>;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        using CColorSwatchStatic = Windows::UI::CColorSwatchStatic;

    public: // Construction
        CVUMeterDlg() = default;

    public: // Public Types
        using Ptr = ATL::CAutoPtr<thisClass>;
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
        enum { IDD = IDD_VU_CFG, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
        END_MSG_MAP();

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

    private: // Data
        enum { PrimarySwatch = 0, SecondarySwatch = 1, BackgroundSwatch = 2, SwatchCount };
        CColorSwatchStatic m_Color[SwatchCount]{};
    }; // class CVUMeterDlg
} // namespace foobar::UI

#endif // GUID_4AB5E457_8034_4027_A1F6_EB333126143E