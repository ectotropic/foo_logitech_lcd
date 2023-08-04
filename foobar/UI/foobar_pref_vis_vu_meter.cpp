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
#include "foobar/UI/foobar_pref_vis_vu_meter.h"
//--------------------------------------

namespace foobar::UI {
    /**************************************************************************
     * CVUMeterImageDlg *
     **************************************************************************/

    //-------------------------------------------------------------------------
    // Message Map Handlers
    //-------------------------------------------------------------------------
    LRESULT CVUMeterImageDlg::OnInit(_In_ UINT /*uMsg*/,
                                     _In_ WPARAM /*wParam*/,
                                     _In_ LPARAM /*lParam*/,
                                     _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_CFG_STATIC));
        WinAPIVerify(SetDlgItemText(IDC_CFG_STATIC, Name()));

        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    /*
     * Edit boxes will **not** receive a EN_KILLFOCUS message
     * when the parent is hidden, so need to process changes
     * made that have not otherwise been dealt with.
     */
    LRESULT CVUMeterImageDlg::OnShow(_In_ UINT /*uMsg*/,
                                     _In_ WPARAM wParam,
                                     _In_ LPARAM /*lParam*/,
                                     _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        // "If wParam is TRUE, the window is being shown.
        //  If wParam is FALSE, the window is being hidden."
        if (wParam) {
            UpdateControls();
            EnableControls();
        } else {
            BOOL bConfigChanged = FALSE;
            ATLASSERT(IsDlgItem(IDC_SCALE_W_EDIT));
            if (GetTextBoxAsFloat(IDC_SCALE_W_EDIT,
                                  VisConfig().m_Range[0].m_fMax,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_OFFSET_W_EDIT));
            if (GetTextBoxAsFloat(IDC_OFFSET_W_EDIT,
                                  VisConfig().m_Range[0].m_fMin,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCALE_H_EDIT));
            if (GetTextBoxAsFloat(IDC_SCALE_H_EDIT,
                                  VisConfig().m_Range[1].m_fMax,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_OFFSET_H_EDIT));
            if (GetTextBoxAsFloat(IDC_OFFSET_H_EDIT,
                                  VisConfig().m_Range[1].m_fMin,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            if (bConfigChanged) {
                SendMessageToParent(WM_CHANGEUISTATE);
            }
        }
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    LRESULT CVUMeterImageDlg::OnUpdateUIState(_In_ UINT /*uMsg*/,
                                              _In_ WPARAM /*wParam*/,
                                              _In_ LPARAM /*lParam*/,
                                              _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    LRESULT CVUMeterImageDlg::OnCommand(_In_ UINT /*uMsg*/,
                                        _In_ WPARAM wParam,
                                        _In_ LPARAM /*lParam*/,
                                        _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction = HIWORD(wParam);

        BOOL bConfigChanged = FALSE;

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_ENABLED_CHECK:
                bConfigChanged = GetCheckBoxState(nDialogItem, VisConfig().m_bEnabled);
                if (bConfigChanged) { EnableControls(); }
                bHandled = TRUE;
                break;
            case IDC_START_WITH_CHECK:
                if (IsDlgButtonChecked(nDialogItem) == BST_CHECKED) {
                    bConfigChanged = SetStartWithThis();
                    EnableDlgItem(nDialogItem, FALSE);
                }
                bHandled = TRUE;
                break;
            case IDC_SCALE_W_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[0].m_fMax,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_OFFSET_W_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[0].m_fMin,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_SCALE_H_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[1].m_fMax,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_OFFSET_H_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[1].m_fMin,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
        }

        if (bConfigChanged) { SendMessageToParent(WM_CHANGEUISTATE); }
        return 0;
    }

    //-----------------------------------------------------------------------------

    void CVUMeterImageDlg::UpdateControls() {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ENABLED_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_ENABLED_CHECK, IsEnabled() ? BST_CHECKED : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_START_WITH_CHECK, GetStartWithThis() ? BST_CHECKED : BST_UNCHECKED));

        WinAPIVerify(SetDlgItemFloat(IDC_SCALE_W_EDIT, VisConfig().m_Range[0].m_fMax));
        WinAPIVerify(SetDlgItemFloat(IDC_OFFSET_W_EDIT, VisConfig().m_Range[0].m_fMin));

        WinAPIVerify(SetDlgItemFloat(IDC_SCALE_H_EDIT, VisConfig().m_Range[1].m_fMax));
        WinAPIVerify(SetDlgItemFloat(IDC_OFFSET_H_EDIT, VisConfig().m_Range[1].m_fMin));
    }

    //-----------------------------------------------------------------------------

    void CVUMeterImageDlg::EnableControls() noexcept {
        const BOOL bEnable = IsEnabled() ? TRUE : FALSE;
        const BOOL bEnableStartWith = StartWithThisAvailable() ? TRUE : FALSE;

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        EnableDlgItem(IDC_START_WITH_CHECK, bEnableStartWith);

        ATLASSERT(IsDlgItem(IDC_SCALE_STATIC));
        ATLASSERT(IsDlgItem(IDC_OFFSET_STATIC));
        ATLASSERT(IsDlgItem(IDC_WIDTH_STATIC));
        ATLASSERT(IsDlgItem(IDC_HEIGHT_STATIC));
        EnableDlgItem(IDC_SCALE_STATIC, bEnable);
        EnableDlgItem(IDC_OFFSET_STATIC, bEnable);
        EnableDlgItem(IDC_WIDTH_STATIC, bEnable);
        EnableDlgItem(IDC_HEIGHT_STATIC, bEnable);

        ATLASSERT(IsDlgItem(IDC_SCALE_W_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_W_EDIT));
        EnableDlgItem(IDC_SCALE_W_EDIT, bEnable);
        EnableDlgItem(IDC_OFFSET_W_EDIT, bEnable);

        ATLASSERT(IsDlgItem(IDC_SCALE_H_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_H_EDIT));
        EnableDlgItem(IDC_SCALE_H_EDIT, bEnable);
        EnableDlgItem(IDC_OFFSET_H_EDIT, bEnable);
    }

    /**************************************************************************
     * CVUMeterDlg *
     **************************************************************************/

    //-------------------------------------------------------------------------
    // Message Map Handlers
    //-------------------------------------------------------------------------
    LRESULT CVUMeterDlg::OnInit(_In_ UINT /*uMsg*/,
                                _In_ WPARAM /*wParam*/,
                                _In_ LPARAM /*lParam*/,
                                _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_CFG_STATIC));
        WinAPIVerify(SetDlgItemText(IDC_CFG_STATIC, Name()));

        ATLASSERT(IsDlgItem(IDC_COLOUR_1_STATIC));
        ATLVERIFY(m_Color[PrimarySwatch].Initialise(GetDlgItem(IDC_COLOUR_1_STATIC),
                                                    VisConfig().m_Color.m_Palette.Primary));
        ATLASSERT(IsDlgItem(IDC_COLOUR_2_STATIC));
        ATLVERIFY(m_Color[SecondarySwatch].Initialise(GetDlgItem(IDC_COLOUR_2_STATIC),
                                                      VisConfig().m_Color.m_Palette.Secondary));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC));
        ATLVERIFY(m_Color[BackgroundSwatch].Initialise(GetDlgItem(IDC_BG_COLOUR_STATIC),
                                                       VisConfig().m_Color.m_Palette.Background));

        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    /*
     * Edit boxes will **not** receive a EN_KILLFOCUS message
     * when the parent is hidden, so need to process changes
     * made that have not otherwise been dealt with.
     */
    LRESULT CVUMeterDlg::OnShow(_In_ UINT /*uMsg*/,
                                _In_ WPARAM wParam,
                                _In_ LPARAM /*lParam*/,
                                _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        // "If wParam is TRUE, the window is being shown.
        //  If wParam is FALSE, the window is being hidden."
        if (wParam) {
            UpdateControls();
            EnableControls();
        } else {
            bool bConfigChanged = false;
            ATLASSERT(IsDlgItem(IDC_SCALE_L_EDIT));
            if (GetTextBoxAsFloat(IDC_SCALE_L_EDIT,
                                  VisConfig().m_Range[0].m_fMax,
                                  0.001f)) {
                bConfigChanged = true;
            }

            ATLASSERT(IsDlgItem(IDC_OFFSET_L_EDIT));
            if (GetTextBoxAsFloat(IDC_OFFSET_L_EDIT,
                                  VisConfig().m_Range[0].m_fMin,
                                  0.001f)) {
                bConfigChanged = true;
            }

            ATLASSERT(IsDlgItem(IDC_SCALE_R_EDIT));
            if (GetTextBoxAsFloat(IDC_SCALE_R_EDIT,
                                  VisConfig().m_Range[1].m_fMax,
                                  0.001f)) {
                bConfigChanged = true;
            }

            ATLASSERT(IsDlgItem(IDC_OFFSET_R_EDIT));
            if (GetTextBoxAsFloat(IDC_OFFSET_R_EDIT,
                                  VisConfig().m_Range[1].m_fMin,
                                  0.001f)) {
                bConfigChanged = true;
            }

            if (bConfigChanged) {
                SendMessageToParent(WM_CHANGEUISTATE);
            }
        }
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    LRESULT CVUMeterDlg::OnUpdateUIState(_In_ UINT /*uMsg*/,
                                         _In_ WPARAM /*wParam*/,
                                         _In_ LPARAM /*lParam*/,
                                         _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //-----------------------------------------------------------------------------

    LRESULT CVUMeterDlg::OnColorStatic(_In_ UINT uMsg,
                                       _In_ WPARAM wParam,
                                       _In_ LPARAM lParam,
                                       _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());
        ATLASSERT(m_Color[PrimarySwatch]);
        ATLASSERT(m_Color[SecondarySwatch]);
        ATLASSERT(m_Color[BackgroundSwatch]);
        if (m_Color[PrimarySwatch] == lParam) {
            return m_Color[PrimarySwatch].OnColorStatic(uMsg, wParam, lParam, bHandled);
        } else if (m_Color[SecondarySwatch] == lParam) {
            return m_Color[SecondarySwatch].OnColorStatic(uMsg, wParam, lParam, bHandled);
        } else if (m_Color[BackgroundSwatch] == lParam) {
            return m_Color[BackgroundSwatch].OnColorStatic(uMsg, wParam, lParam, bHandled);
        }
        return 0;
    }

    //-----------------------------------------------------------------------------

    LRESULT CVUMeterDlg::OnCommand(_In_ UINT /*uMsg*/,
                                   _In_ WPARAM wParam,
                                   _In_ LPARAM /*lParam*/,
                                   _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction = HIWORD(wParam);

        BOOL bConfigChanged = FALSE;

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_ENABLED_CHECK:
                bConfigChanged = GetCheckBoxState(nDialogItem, VisConfig().m_bEnabled);
                if (bConfigChanged) { EnableControls(); }
                bHandled = TRUE;
                break;
            case IDC_START_WITH_CHECK:
                if (IsDlgButtonChecked(nDialogItem) == BST_CHECKED) {
                    bConfigChanged = SetStartWithThis();
                    EnableDlgItem(nDialogItem, FALSE);
                }
                bHandled = TRUE;
                break;
            case IDC_PEAK_CHECK:
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  VisConfig().m_Peak.m_bEnable);
                bHandled = TRUE;
                break;
            case IDC_SCALE_L_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[0].m_fMax,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_OFFSET_L_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[0].m_fMin,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_SCALE_R_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[1].m_fMax,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_OFFSET_R_EDIT:
                if (nAction == EN_KILLFOCUS && SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_Range[1].m_fMin,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            case IDC_COLOUR_1_BUTTON:
            {
                const auto oldColor = m_Color[PrimarySwatch].GetColor();
                if (m_Color[PrimarySwatch].SelectColor() &&
                    oldColor != m_Color[PrimarySwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Primary = m_Color[PrimarySwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }
            case IDC_COLOUR_2_BUTTON:
            {
                const auto oldColor = m_Color[SecondarySwatch].GetColor();
                if (m_Color[SecondarySwatch].SelectColor() &&
                    oldColor != m_Color[SecondarySwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Secondary = m_Color[SecondarySwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }
            case IDC_BG_COLOUR_BUTTON:
            {
                const auto oldColor = m_Color[BackgroundSwatch].GetColor();
                if (m_Color[BackgroundSwatch].SelectColor() &&
                    oldColor != m_Color[BackgroundSwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Background = m_Color[BackgroundSwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }
        }

        if (bConfigChanged) {
            SendMessageToParent(WM_CHANGEUISTATE);
        }
        return 0;
    }

    //-----------------------------------------------------------------------------

    void CVUMeterDlg::UpdateControls() {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ENABLED_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_ENABLED_CHECK, IsEnabled() ? BST_CHECKED : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_START_WITH_CHECK, GetStartWithThis() ? BST_CHECKED : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_PEAK_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_PEAK_CHECK, VisConfig().m_Peak.m_bEnable ? BST_CHECKED : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_SCALE_L_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_L_EDIT));
        WinAPIVerify(SetDlgItemFloat(IDC_SCALE_L_EDIT, VisConfig().m_Range[0].m_fMax));
        WinAPIVerify(SetDlgItemFloat(IDC_OFFSET_L_EDIT, VisConfig().m_Range[0].m_fMin));

        ATLASSERT(IsDlgItem(IDC_SCALE_R_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_R_EDIT));
        WinAPIVerify(SetDlgItemFloat(IDC_SCALE_R_EDIT, VisConfig().m_Range[1].m_fMax));
        WinAPIVerify(SetDlgItemFloat(IDC_OFFSET_R_EDIT, VisConfig().m_Range[1].m_fMin));

        ATLVERIFY(m_Color[PrimarySwatch].SelectColor(VisConfig().m_Color.m_Palette.Primary));
        ATLVERIFY(m_Color[SecondarySwatch].SelectColor(VisConfig().m_Color.m_Palette.Secondary));
        ATLVERIFY(m_Color[BackgroundSwatch].SelectColor(VisConfig().m_Color.m_Palette.Background));
    }

    //-----------------------------------------------------------------------------

    void CVUMeterDlg::EnableControls() noexcept {
        const BOOL bEnable = IsEnabled() ? TRUE : FALSE;
        const BOOL bEnableStartWith = StartWithThisAvailable() ? TRUE : FALSE;
        const auto bEnableColor = (bEnable && CanvasIsColor()) ? TRUE : FALSE;

        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        EnableDlgItem(IDC_START_WITH_CHECK, bEnableStartWith);

        ATLASSERT(IsDlgItem(IDC_SCALE_STATIC));
        ATLASSERT(IsDlgItem(IDC_OFFSET_STATIC));
        ATLASSERT(IsDlgItem(IDC_LEFT_STATIC));
        ATLASSERT(IsDlgItem(IDC_RIGHT_STATIC));
        EnableDlgItem(IDC_SCALE_STATIC, bEnable);
        EnableDlgItem(IDC_OFFSET_STATIC, bEnable);
        EnableDlgItem(IDC_LEFT_STATIC, bEnable);
        EnableDlgItem(IDC_RIGHT_STATIC, bEnable);

        ATLASSERT(IsDlgItem(IDC_SCALE_L_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_L_EDIT));
        EnableDlgItem(IDC_SCALE_L_EDIT, bEnable);
        EnableDlgItem(IDC_OFFSET_L_EDIT, bEnable);

        ATLASSERT(IsDlgItem(IDC_SCALE_R_EDIT));
        ATLASSERT(IsDlgItem(IDC_OFFSET_R_EDIT));
        EnableDlgItem(IDC_SCALE_R_EDIT, bEnable);
        EnableDlgItem(IDC_OFFSET_R_EDIT, bEnable);

        ATLASSERT(IsDlgItem(IDC_PEAK_CHECK));
        EnableDlgItem(IDC_PEAK_CHECK, bEnable);

        ATLASSERT(IsDlgItem(IDC_COLOUR_1_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_COLOUR_1_STATIC));
        ATLASSERT(IsDlgItem(IDC_COLOUR_1_BUTTON));
        EnableDlgItem(IDC_COLOUR_1_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_COLOUR_1_STATIC, bEnableColor);
        EnableDlgItem(IDC_COLOUR_1_BUTTON, bEnableColor);

        ATLASSERT(IsDlgItem(IDC_COLOUR_2_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_COLOUR_2_STATIC));
        ATLASSERT(IsDlgItem(IDC_COLOUR_2_BUTTON));
        EnableDlgItem(IDC_COLOUR_2_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_COLOUR_2_STATIC, bEnableColor);
        EnableDlgItem(IDC_COLOUR_2_BUTTON, bEnableColor);

        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_BUTTON));
        EnableDlgItem(IDC_BG_COLOUR_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_STATIC, bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_BUTTON, bEnableColor);
    }
} // namespace foobar::UI


