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
#include "foobar/UI/foobar_pref_vis_oscilloscope.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // COscilloscopeDlg
    //**************************************************************************

    //--------------------------------------------------------------------------
    // Message Map Handlers
    //--------------------------------------------------------------------------
    LRESULT COscilloscopeDlg::OnInit(_In_ UINT /*uMsg*/,
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

    //------------------------------------------------------

    /*
     * Edit boxes will **not** receive a EN_KILLFOCUS message
     * when the parent is hidden, so need to process changes
     * made that have not otherwise been dealt with.
     */
    LRESULT COscilloscopeDlg::OnShow(_In_ UINT /*uMsg*/,
                                     _In_ WPARAM wParam,
                                     _In_ LPARAM /*lParam*/,
                                     _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        // "If wParam is TRUE, the window is being shown.
        //  If wParam is FALSE, the window is being hidden."
        if (wParam) {
            UpdateControls();
            EnableControls();
            bHandled = TRUE;
        } else {
            ATLASSERT(IsDlgItem(IDC_SCALE_EDIT));
            if (GetTextBoxAsFloat(IDC_SCALE_EDIT,
                                  VisConfig().m_fScale,
                                  0.001f)) {
                SendMessageToParent(WM_CHANGEUISTATE);
            }
            bHandled = TRUE;
        }
        return 0;
    }

    //------------------------------------------------------

    LRESULT COscilloscopeDlg::OnUpdateUIState(_In_ UINT /*uMsg*/,
                                              _In_ WPARAM /*wParam*/,
                                              _In_ LPARAM /*lParam*/,
                                              _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT COscilloscopeDlg::OnColorStatic(_In_ UINT uMsg,
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

    //------------------------------------------------------

    LRESULT COscilloscopeDlg::OnCommand(_In_ UINT /*uMsg*/,
                                        _In_ WPARAM wParam,
                                        _In_ LPARAM /*lParam*/,
                                        _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        BOOL bConfigChanged = FALSE;

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction     = HIWORD(wParam);

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_ENABLED_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem, VisConfig().m_bEnabled);
                if (bConfigChanged) { EnableControls(); }
                bHandled = TRUE;
                break;
            }

            case IDC_START_WITH_CHECK: {
                if (IsDlgButtonChecked(nDialogItem) == BST_CHECKED) {
                    bConfigChanged = SetStartWithThis();
                    EnableDlgItem(nDialogItem, FALSE);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_SCALE_EDIT: {
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_fScale,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_COLOUR_1_BUTTON: {
                const auto oldColor = m_Color[PrimarySwatch].GetColor();
                if (m_Color[PrimarySwatch].SelectColor() &&
                    oldColor != m_Color[PrimarySwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Primary = m_Color[PrimarySwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }

            case IDC_COLOUR_2_BUTTON: {
                const auto oldColor = m_Color[SecondarySwatch].GetColor();
                if (m_Color[SecondarySwatch].SelectColor() &&
                    oldColor != m_Color[SecondarySwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Secondary = m_Color[SecondarySwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }

            case IDC_BG_COLOUR_BUTTON: {
                const auto oldColor = m_Color[BackgroundSwatch].GetColor();
                if (m_Color[BackgroundSwatch].SelectColor() &&
                    oldColor != m_Color[BackgroundSwatch].GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Color.m_Palette.Background = m_Color[BackgroundSwatch].GetColor();
                }
                bHandled = TRUE;
                break;
            }
        } // switch (nDialogItem)

        if (bConfigChanged) {
            SendMessageToParent(WM_CHANGEUISTATE);
        }

        return 0;
    }

    //--------------------------------------------------------------------------
    // Message Map Helpers
    //--------------------------------------------------------------------------

    void COscilloscopeDlg::UpdateControls() {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ENABLED_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_ENABLED_CHECK,
                                    IsEnabled()
                                    ? BST_CHECKED
                                    : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_START_WITH_CHECK,
                                    GetStartWithThis()
                                    ? BST_CHECKED
                                    : BST_UNCHECKED));

        ATLVERIFY(m_Color[PrimarySwatch].SelectColor(VisConfig().m_Color.m_Palette.Primary));
        ATLVERIFY(m_Color[SecondarySwatch].SelectColor(VisConfig().m_Color.m_Palette.Secondary));
        ATLVERIFY(m_Color[BackgroundSwatch].SelectColor(VisConfig().m_Color.m_Palette.Background));

        ATLASSERT(IsDlgItem(IDC_SCALE_EDIT));
        WinAPIVerify(SetDlgItemFloat(IDC_SCALE_EDIT, VisConfig().m_fScale));
    }

    //-----------------------------------------------------------------------------

    void COscilloscopeDlg::EnableControls() noexcept {
        const auto bEnable = IsEnabled() ? TRUE : FALSE;
        const BOOL bEnableStartWith = StartWithThisAvailable() ? TRUE : FALSE;
        const auto bEnableColor = (bEnable && CanvasIsColor()) ? TRUE : FALSE;

        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        EnableDlgItem(IDC_START_WITH_CHECK, bEnableStartWith);

        ATLASSERT(IsDlgItem(IDC_SCALE_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCALE_EDIT));
        EnableDlgItem(IDC_SCALE_STATIC, bEnable);
        EnableDlgItem(IDC_SCALE_EDIT  , bEnable);

        ATLASSERT(IsDlgItem(IDC_COLOUR_1_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_COLOUR_1_STATIC));
        ATLASSERT(IsDlgItem(IDC_COLOUR_1_BUTTON));
        EnableDlgItem(IDC_COLOUR_1_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_COLOUR_1_STATIC     , bEnableColor);
        EnableDlgItem(IDC_COLOUR_1_BUTTON     , bEnableColor);

        ATLASSERT(IsDlgItem(IDC_COLOUR_2_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_COLOUR_2_STATIC));
        ATLASSERT(IsDlgItem(IDC_COLOUR_2_BUTTON));
        EnableDlgItem(IDC_COLOUR_2_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_COLOUR_2_STATIC     , bEnableColor);
        EnableDlgItem(IDC_COLOUR_2_BUTTON     , bEnableColor);

        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_BUTTON));
        EnableDlgItem(IDC_BG_COLOUR_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_STATIC     , bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_BUTTON     , bEnableColor);
    }
} // namespace foobar::UI
