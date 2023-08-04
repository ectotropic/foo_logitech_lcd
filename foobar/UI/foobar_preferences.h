#pragma once
#ifndef GUID_F3E00143_6A0C_4241_A0E3_61AA8D5911A1
#define GUID_F3E00143_6A0C_4241_A0E3_61AA8D5911A1
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
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/foobar_pref_general.h"
#include "foobar/UI/foobar_pref_visualisation.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CFoobarPreferencesDlg
    //**************************************************************************
    class CFoobarPreferencesDlg :
        public preferences_page_instance,
        public Windows::UI::CDialogImpl<CFoobarPreferencesDlg,
                                        Windows::UI::CPreferencesDialog> {
    private: // Internal Types
        using thisClass  = CFoobarPreferencesDlg;
        using baseClass  = Windows::UI::CPreferencesDialog;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        template <class DialogT>
        using CDialogPtrT = Windows::UI::CAutoPtr<DialogT>;

        using CGeneralDlgPtr        = CGeneralDlg::Ptr;
        using CVisualisationsDlgPtr = CVisualisationsDlg::Ptr;

        using CTabCtrl   = Windows::UI::CTabCtrl;
        using CTabWindow = Windows::UI::CWindow;

        using ConfigData = Config::ConfigData;

    private:
        thisClass(thisClass&)            = delete; // No Copy
        thisClass& operator=(thisClass&) = delete; // No Copy

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;

    public: // preferences_page_instance
        CFoobarPreferencesDlg(preferences_page_callback::ptr callback) :
            m_pCallback(callback) {}

        virtual t_uint32 get_state() noexcept override;
        virtual void         apply() override;
        virtual void         reset() override;

    public: // Dialog
        enum {
            IDD           = IDD_CONFIG_MAIN,
            IDD_TABCTRL   = IDC_CONFIG_TABS,
            IDD_TAB_SIZER = IDC_STATIC_SIZER,
        };

    protected: //Message Map
        BEGIN_MSG_MAP(CFoobarPreferencesDlg)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW, OnShow)
            MESSAGE_HANDLER(WM_CHANGEUISTATE, OnChangeUIState)
            NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnTabChange)                     pragma_suppress(26454) // C26454: Arithmetic overflow: '...' operation produces a negative unsigned result at compile time (io.5).
        END_MSG_MAP()

        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnChangeUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled);

        LRESULT OnTabChange(_In_ WPARAM wParam,
                            _In_ LPNMHDR lpnmHdr,
                            _Inout_ BOOL& bHandled);

    protected: //Message Map Helpers
        void ShowCurrentTab();

        template <class DialogT>
        void ShowCurrentTab(_In_ DialogT& dialog) {
            ATLASSERT(IsWindow());
            if (m_CurrentTab.IsWindow()) {
                if (m_CurrentTab.m_hWnd == dialog.m_hWnd) {
                    m_CurrentTab.ShowWindow(SW_SHOW);
                    return;
                } else {
                    m_CurrentTab.ShowWindow(SW_HIDE);
                    m_CurrentTab.Detach();
                }
            }
            HWND hDlg = dialog.ShowDialog(*this,
                                          SW_SHOW,
                                          &dialog,
                                          GetDlgItem(IDD_TAB_SIZER));
            ATLASSERT(!m_CurrentTab);
            m_CurrentTab.Attach(hDlg);
        }

    private: //Data
        bool                  m_bPendingChanges      { false };
        ConfigData            m_CoreConfig           {};
        SVisPrefConfig        m_Config               {};

        CGeneralDlgPtr        m_pGeneralOptions      {};
        CVisualisationsDlgPtr m_pVisualisationOptions{};

        CTabCtrl              m_TabCtrl              {};
        CTabWindow            m_CurrentTab           {};

    private: //foobar callback
        const preferences_page_callback::ptr m_pCallback{ nullptr };
    }; // class CFoobarPreferencesDlg
} // namespace foobar::UI

#endif //GUID_F3E00143_6A0C_4241_A0E3_61AA8D5911A1