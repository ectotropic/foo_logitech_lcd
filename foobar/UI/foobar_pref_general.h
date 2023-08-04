#pragma once
#ifndef GUID_3708819F_E341_4D3F_9B93_DECFEA40EDB6
#define GUID_3708819F_E341_4D3F_9B93_DECFEA40EDB6
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
#include "Windows/UI/ATL_EnumComboBox.h"
#include "foobar/UI/foobar_pref_vis_dlg.h"
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CGeneralDlg
    //**************************************************************************
    class CGeneralDlg :
        public Windows::UI::CDialogImpl<CGeneralDlg,
                                        Windows::UI::CPreferencesDialog> {
    private: // Internal Types
        using thisClass  = CGeneralDlg;
        using baseClass  = Windows::UI::CPreferencesDialog;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        using CAlbumArtComboHelper =
            foobar::UI::CSequentialEnumHelperT<AlbumArtType>;
        using CAlbumArtCombo =
            Windows::UI::CEnumComboBoxT<AlbumArtType, CAlbumArtComboHelper>;

    private:
        thisClass(thisClass&)            = delete; // No Copy
        thisClass& operator=(thisClass&) = delete; // No Copy

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;

    public:
        CGeneralDlg(const SVisPrefConfig& config) :
            m_Config{ config } {}

    public:
        enum { IDD = IDD_LCD_CFG_TAB };

    public:
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

    protected:
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG   , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW   , OnShow)
            MESSAGE_HANDLER(WM_CHANGEUISTATE, OnChangeUIState)
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

        LRESULT OnChangeUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled);

    protected: //Message Map Helpers
        void UpdateControls();
        void EnableControls() noexcept;

    private:
        constexpr auto& Config       () const noexcept { return m_Config; }
        constexpr auto& CanvasConfig () const noexcept { return m_Config.Canvas; }
        constexpr auto& CoreConfig   () const noexcept { return *m_Config.pConfig; }
        constexpr auto& GeneralConfig() const noexcept { return CoreConfig().m_GeneralConfig; }

    private:
        SVisPrefConfig m_Config       {};
        CAlbumArtCombo m_AlbumArtCombo{};
    }; // class CGeneralDlg
} // namespace foobar::UI

#endif //GUID_3708819F_E341_4D3F_9B93_DECFEA40EDB6