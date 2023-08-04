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
#include "foobar/UI/foobar_pref_vis_track_details.h"
//--------------------------------------

namespace foobar::UI::detail {
    //**************************************************************************
    // CTextOptionsDialogCommon
    //**************************************************************************
    LRESULT CTextOptionsDialogCommon::OnInit(_In_ UINT /*uMsg*/,
                                             _In_ WPARAM /*wParam*/,
                                             _In_ LPARAM /*lParam*/,
                                             _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC));
        ATLVERIFY(m_BackgroundColor.Initialise(GetDlgItem(IDC_BG_COLOUR_STATIC),
                                               VisConfig().m_Text.m_BackgroundColor));

        ATLASSERT(IsDlgItem(IDC_LEFT_TIME_COMBO));
        m_TimeCombo_Left.Detach();
        m_TimeCombo_Left.Attach(GetDlgItem(IDC_LEFT_TIME_COMBO));
        ATLASSERT(m_TimeCombo_Left.IsWindow());
        ATLVERIFY(m_TimeCombo_Left.InsertAll());

        ATLASSERT(IsDlgItem(IDC_RIGHT_TIME_COMBO));
        m_TimeCombo_Right.Detach();
        m_TimeCombo_Right.Attach(GetDlgItem(IDC_RIGHT_TIME_COMBO));
        ATLASSERT(m_TimeCombo_Right.IsWindow());
        ATLVERIFY(m_TimeCombo_Right.InsertAll());

        if (IsTrackInfoExpertDialog()) {
            m_strTextExpert.SetString(VisConfig().m_Text.m_Format.c_str());
        } else {
            m_strText[::TrackDetailsType::Page1] = TEXT("");
            m_strText[::TrackDetailsType::Page2] = TEXT("");
            m_strText[::TrackDetailsType::Page3] = TEXT("");
            m_strText[::TrackDetailsType::Page4] = TEXT("");

            CString strText{ VisConfig().m_Text.m_Format.c_str() };
            auto nIndex = strText.Find(TEXT('\n'));
            if (nIndex >= 0) {
                m_strText[::TrackDetailsType::Page1] = strText.Left(nIndex);
                strText.Delete(0, nIndex + 1);
                nIndex = strText.Find(TEXT('\n'));
                if (nIndex >= 0) {
                    m_strText[::TrackDetailsType::Page2] = strText.Left(nIndex);
                    strText.Delete(0, nIndex + 1);
                    nIndex = strText.Find(TEXT('\n'));
                    if (nIndex >= 0) {
                        m_strText[::TrackDetailsType::Page3] = strText.Left(nIndex);
                        strText.Delete(0, nIndex + 1);
                        m_strText[::TrackDetailsType::Page4] = strText;
                    } else {
                        m_strText[::TrackDetailsType::Page3] = strText;
                    }
                } else {
                    m_strText[::TrackDetailsType::Page2] = strText;
                }
            } else {
                m_strText[::TrackDetailsType::Page1] = strText;
            }
        }

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
    LRESULT CTextOptionsDialogCommon::OnShow(_In_ UINT /*uMsg*/,
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
            if (IsTrackInfoExpertDialog()) {
                ATLASSERT(IsDlgItem(IDC_TRACK_INFO_EDIT));
                CString strText;
                const auto cch = GetDlgItemText(IDC_TRACK_INFO_EDIT, strText);
                ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                if ((cch != m_strTextExpert.GetLength()) ||
                    strText.Compare(m_strTextExpert) == 0) {
                    m_strTextExpert = strText;
                    bConfigChanged = TRUE;
                }
            } else {
                {
                    ATLASSERT(IsDlgItem(IDC_LINE1_EDIT));
                    CString strText;
                    const auto cch = GetDlgItemText(IDC_LINE1_EDIT, strText);
                    WinAPIAssert(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page1].GetLength()) ||
                        m_strText[::TrackDetailsType::Page1].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page1] = strText;
                        bConfigChanged = TRUE;
                    }
                }
                {
                    ATLASSERT(IsDlgItem(IDC_LINE2_EDIT));
                    CString strText;
                    const auto cch = GetDlgItemText(IDC_LINE2_EDIT, strText);
                    WinAPIAssert(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page2].GetLength()) ||
                        m_strText[::TrackDetailsType::Page2].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page2] = strText;
                        bConfigChanged = TRUE;
                    }
                }
                {
                    ATLASSERT(IsDlgItem(IDC_LINE3_EDIT));
                    CString strText;
                    const auto cch = GetDlgItemText(IDC_LINE3_EDIT, strText);
                    WinAPIAssert(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page3].GetLength()) ||
                        m_strText[::TrackDetailsType::Page3].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page3] = strText;
                        bConfigChanged = TRUE;
                    }
                }
                {
                    ATLASSERT(IsDlgItem(IDC_LINE4_EDIT));
                    CString strText;
                    const auto cch = GetDlgItemText(IDC_LINE4_EDIT, strText);
                    WinAPIAssert(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page4].GetLength()) ||
                        m_strText[::TrackDetailsType::Page4].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page4] = strText;
                        bConfigChanged = TRUE;
                    }
                }
            }

            if (bConfigChanged) {
                UpdateTrackInfoFmt();
            }

            ATLASSERT(IsDlgItem(IDC_PROGRESS_HEIGHT_EDIT));
            if (GetTextBoxAsFloat(IDC_PROGRESS_HEIGHT_EDIT,
                                  VisConfig().m_ProgressBar.m_fHeight,
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

    //------------------------------------------------------

    LRESULT CTextOptionsDialogCommon::OnUpdateUIState(_In_ UINT /*uMsg*/,
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

    LRESULT CTextOptionsDialogCommon::OnColorStatic(_In_ UINT uMsg,
                                                    _In_ WPARAM wParam,
                                                    _In_ LPARAM lParam,
                                                    _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());
        ATLASSERT(m_BackgroundColor);
        if (m_BackgroundColor == lParam) {
            return m_BackgroundColor.OnColorStatic(uMsg, wParam,
                                                   lParam, bHandled);
        }
        return 0;
    }

    //------------------------------------------------------

    LRESULT CTextOptionsDialogCommon::OnCommand(_In_ UINT /*uMsg*/,
                                                _In_ WPARAM wParam,
                                                _In_ LPARAM /*lParam*/,
                                                _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction = HIWORD(wParam);

        BOOL bConfigChanged = FALSE;

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_PROGRESS_HEIGHT_EDIT: {
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       VisConfig().m_ProgressBar.m_fHeight,
                                                       0.001f);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_TRACK_INFO_EDIT: {
                ATLASSERT(IsTrackInfoExpertDialog());
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    CString strText;
                    const auto cch = GetDlgItemText(nDialogItem, strText);
                    ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strTextExpert.GetLength()) ||
                        m_strTextExpert.Compare(strText) != 0) {
                        m_strTextExpert = strText;
                        UpdateTrackInfoFmt();
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LINE1_EDIT: {
                ATLASSERT(!IsTrackInfoExpertDialog());
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    CString strText;
                    const auto cch = GetDlgItemText(nDialogItem, strText);
                    ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page1].GetLength()) ||
                        m_strText[::TrackDetailsType::Page1].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page1] = strText;
                        UpdateTrackInfoFmt();
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LINE2_EDIT: {
                ATLASSERT(!IsTrackInfoExpertDialog());
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    CString strText;
                    GetDlgItemText(nDialogItem, strText);
                    const auto cch = GetDlgItemText(nDialogItem, strText);
                    ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page2].GetLength()) ||
                        m_strText[::TrackDetailsType::Page2].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page2] = strText;
                        UpdateTrackInfoFmt();
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LINE3_EDIT: {
                ATLASSERT(!IsTrackInfoExpertDialog());
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    CString strText;
                    const auto cch = GetDlgItemText(nDialogItem, strText);
                    ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page3].GetLength()) ||
                        m_strText[::TrackDetailsType::Page3].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page3] = strText;
                        UpdateTrackInfoFmt();
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LINE4_EDIT: {
                ATLASSERT(!IsTrackInfoExpertDialog());
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    CString strText;
                    const auto cch = GetDlgItemText(nDialogItem, strText);
                    ATLASSERT(cch > 0 || ::GetLastError() == ERROR_SUCCESS);
                    if ((cch != m_strText[::TrackDetailsType::Page4].GetLength()) ||
                        m_strText[::TrackDetailsType::Page4].Compare(strText) != 0) {
                        m_strText[::TrackDetailsType::Page4] = strText;
                        UpdateTrackInfoFmt();
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LEFT_TIME_COMBO: {
                auto trackTime = VisConfig().m_ProgressBar.m_Time.m_Left;
                if (m_TimeCombo_Left.GetCurSelVal(trackTime)) {
                    bConfigChanged = VisConfig().m_ProgressBar.m_Time.m_Left != trackTime;
                    VisConfig().m_ProgressBar.m_Time.m_Left = trackTime;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_RIGHT_TIME_COMBO: {
                auto trackTime = VisConfig().m_ProgressBar.m_Time.m_Right;
                if (m_TimeCombo_Right.GetCurSelVal(trackTime)) {
                    bConfigChanged = VisConfig().m_ProgressBar.m_Time.m_Right != trackTime;
                    VisConfig().m_ProgressBar.m_Time.m_Right = trackTime;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_LINE4_RADIO:
                [[fallthrough]];
            case IDC_PROGRESS_RADIO: {
                ATLASSERT(!IsTrackInfoExpertDialog());
                bConfigChanged = GetCheckBoxState(IDC_PROGRESS_RADIO,
                                                  VisConfig().m_ProgressBar.m_bEnabled);
                bHandled = TRUE;
                break;
            }

            case IDC_PROGRESS_CHECK: {
                ATLASSERT(IsTrackInfoExpertDialog());
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  VisConfig().m_ProgressBar.m_bEnabled);
                bHandled = TRUE;
                break;
            }

            case IDC_BG_COLOUR_BUTTON: {
                const auto oldColor = m_BackgroundColor.GetColor();
                if (m_BackgroundColor.SelectColor() &&
                    oldColor != m_BackgroundColor.GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Text.m_BackgroundColor = m_BackgroundColor;
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

    //------------------------------------------------------

    void CTextOptionsDialogCommon::UpdateTrackInfoFmt() {
        if (IsTrackInfoExpertDialog()) {
            VisConfig().m_Text.m_Format.assign(m_strTextExpert.GetString());
        } else {
            CString strText;
            strText.Append(m_strText[::TrackDetailsType::Page1]);
            strText.Append(TEXT("$char(10)\r\n"));
            strText.Append(m_strText[::TrackDetailsType::Page2]);
            strText.Append(TEXT("$char(10)\r\n"));
            strText.Append(m_strText[::TrackDetailsType::Page3]);
            strText.Append(TEXT("$char(10)\r\n"));
            strText.Append(m_strText[::TrackDetailsType::Page4]);
            VisConfig().m_Text.m_Format.assign(strText.GetString());
        }
    }

    //------------------------------------------------------

    void CTextOptionsDialogCommon::UpdateControls() {
        ATLASSERT(IsWindow());

        if (IsTrackInfoExpertDialog()) {
            ATLASSERT(IsDlgItem(IDC_TRACK_INFO_EDIT));
            WinAPIVerify(SetDlgItemText(IDC_TRACK_INFO_EDIT,
                                        m_strTextExpert));

            ATLASSERT(IsDlgItem(IDC_PROGRESS_CHECK));
            WinAPIVerify(CheckDlgButton(IDC_PROGRESS_CHECK,
                                        VisConfig().m_ProgressBar.m_bEnabled));

            ATLASSERT(!IsDlgItem(IDC_LINE1_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE2_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE3_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE4_EDIT));

            ATLASSERT(!IsDlgItem(IDC_LINE4_RADIO));
            ATLASSERT(!IsDlgItem(IDC_PROGRESS_RADIO));
        } else {
            ATLASSERT(IsDlgItem(IDC_LINE1_EDIT));
            ATLASSERT(IsDlgItem(IDC_LINE2_EDIT));
            ATLASSERT(IsDlgItem(IDC_LINE3_EDIT));
            ATLASSERT(IsDlgItem(IDC_LINE4_EDIT));
            WinAPIVerify(SetDlgItemText(IDC_LINE1_EDIT,
                                        m_strText[::TrackDetailsType::Page1]));
            WinAPIVerify(SetDlgItemText(IDC_LINE2_EDIT,
                                        m_strText[::TrackDetailsType::Page2]));
            WinAPIVerify(SetDlgItemText(IDC_LINE3_EDIT,
                                        m_strText[::TrackDetailsType::Page3]));
            WinAPIVerify(SetDlgItemText(IDC_LINE4_EDIT,
                                        m_strText[::TrackDetailsType::Page4]));

            ATLASSERT(IsDlgItem(IDC_LINE4_RADIO));
            ATLASSERT(IsDlgItem(IDC_PROGRESS_RADIO));
            WinAPIVerify(CheckDlgButton(IDC_LINE4_RADIO,
                                        VisConfig().m_ProgressBar.m_bEnabled
                                        ? BST_UNCHECKED
                                        : BST_CHECKED));
            WinAPIVerify(CheckDlgButton(IDC_PROGRESS_RADIO,
                                        VisConfig().m_ProgressBar.m_bEnabled
                                        ? BST_CHECKED
                                        : BST_UNCHECKED));

            ATLASSERT(!IsDlgItem(IDC_TRACK_INFO_EDIT));
            ATLASSERT(!IsDlgItem(IDC_PROGRESS_CHECK));
        }

        {
            ATLASSERT(m_TimeCombo_Left.IsWindow());
            const auto selected = VisConfig().m_ProgressBar.m_Time.m_Left;
            [[maybe_unused]]
            const auto index = m_TimeCombo_Left.SelectValue(selected);
            ATLASSERT(index != CB_ERR);
        }

        {
            ATLASSERT(m_TimeCombo_Right.IsWindow());
            const auto selected = VisConfig().m_ProgressBar.m_Time.m_Right;
            [[maybe_unused]]
            const auto index = m_TimeCombo_Right.SelectValue(selected);
            ATLASSERT(index != CB_ERR);
        }

        ATLASSERT(IsDlgItem(IDC_PROGRESS_HEIGHT_EDIT));
        WinAPIVerify(SetDlgItemFloat(IDC_PROGRESS_HEIGHT_EDIT,
                                     VisConfig().m_ProgressBar.m_fHeight));

        ATLVERIFY(m_BackgroundColor.SelectColor(VisConfig().m_Text.m_BackgroundColor));
    }

    //------------------------------------------------------

    void CTextOptionsDialogCommon::EnableControls() noexcept {
        const BOOL bEnable = IsEnabled() ? TRUE : FALSE;

        ATLASSERT(IsWindow());

        if (IsTrackInfoExpertDialog()) {
            ATLASSERT(IsDlgItem(IDC_TRACK_INFO_EDIT));
            EnableDlgItem(IDC_TRACK_INFO_EDIT, bEnable);

            ATLASSERT(IsDlgItem(IDC_PROGRESS_CHECK));
            EnableDlgItem(IDC_PROGRESS_CHECK, bEnable);

            ATLASSERT(!IsDlgItem(IDC_LINE1_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE2_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE3_EDIT));
            ATLASSERT(!IsDlgItem(IDC_LINE4_EDIT));

            ATLASSERT(!IsDlgItem(IDC_LINE4_RADIO));
            ATLASSERT(!IsDlgItem(IDC_PROGRESS_RADIO));
        } else {
            ATLASSERT(IsDlgItem(IDC_LINE1_STATIC));
            ATLASSERT(IsDlgItem(IDC_LINE1_EDIT));
            ATLASSERT(IsDlgItem(IDC_LINE2_STATIC));
            ATLASSERT(IsDlgItem(IDC_LINE2_EDIT));
            ATLASSERT(IsDlgItem(IDC_LINE3_STATIC));
            ATLASSERT(IsDlgItem(IDC_LINE3_EDIT));
            EnableDlgItem(IDC_LINE1_STATIC, bEnable);
            EnableDlgItem(IDC_LINE1_EDIT, bEnable);
            EnableDlgItem(IDC_LINE2_STATIC, bEnable);
            EnableDlgItem(IDC_LINE2_EDIT, bEnable);
            EnableDlgItem(IDC_LINE3_STATIC, bEnable);
            EnableDlgItem(IDC_LINE3_EDIT, bEnable);

            ATLASSERT(IsDlgItem(IDC_LINE4_RADIO));
            ATLASSERT(IsDlgItem(IDC_PROGRESS_RADIO));
            EnableDlgItem(IDC_LINE4_RADIO, bEnable);
            EnableDlgItem(IDC_PROGRESS_RADIO, bEnable);

            //ATLASSERT(IsDlgItem(IDC_LINE4_STATIC));
            const BOOL bEnableLine4 = (bEnable &&
                                       !VisConfig().m_ProgressBar.m_bEnabled)
                                       ? TRUE : FALSE;
            ATLASSERT(IsDlgItem(IDC_LINE4_EDIT));
            EnableDlgItem(IDC_LINE4_EDIT, bEnableLine4);

            ATLASSERT(!IsDlgItem(IDC_TRACK_INFO_EDIT));
            ATLASSERT(!IsDlgItem(IDC_PROGRESS_CHECK));
        }

        const BOOL bEnableProgressBar = (bEnable &&
                                         VisConfig().m_ProgressBar.m_bEnabled)
                                        ? TRUE : FALSE;
        ATLASSERT(IsDlgItem(IDC_PROGRESS_HEIGHT_STATIC));
        ATLASSERT(IsDlgItem(IDC_PROGRESS_HEIGHT_EDIT));
        EnableDlgItem(IDC_PROGRESS_HEIGHT_STATIC, bEnableProgressBar);
        EnableDlgItem(IDC_PROGRESS_HEIGHT_EDIT  , bEnableProgressBar);

        ATLASSERT(IsDlgItem(IDC_LEFT_TIME_COMBO));
        ATLASSERT(IsDlgItem(IDC_LEFT_TIME_STATIC_TEXT));
        EnableDlgItem(IDC_LEFT_TIME_COMBO      , bEnableProgressBar);
        EnableDlgItem(IDC_LEFT_TIME_STATIC_TEXT, bEnableProgressBar);

        ATLASSERT(IsDlgItem(IDC_RIGHT_TIME_COMBO));
        ATLASSERT(IsDlgItem(IDC_RIGHT_TIME_STATIC_TEXT));
        EnableDlgItem(IDC_RIGHT_TIME_COMBO      , bEnableProgressBar);
        EnableDlgItem(IDC_RIGHT_TIME_STATIC_TEXT, bEnableProgressBar);

        const auto bEnableColor = (bEnable && CanvasIsColor()) ? TRUE : FALSE;
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_STATIC));
        ATLASSERT(IsDlgItem(IDC_BG_COLOUR_BUTTON));
        EnableDlgItem(IDC_BG_COLOUR_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_STATIC     , bEnableColor);
        EnableDlgItem(IDC_BG_COLOUR_BUTTON     , bEnableColor);
    }
} // namespace foobar::UI::detail

//==============================================================================

namespace foobar::UI {
    //**************************************************************************
    // CFontOptionsDlg
    //**************************************************************************
    LRESULT CFontOptionsDlg::OnInit(_In_ UINT /*uMsg*/,
                                    _In_ WPARAM /*wParam*/,
                                    _In_ LPARAM /*lParam*/,
                                    _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_COLOUR_STATIC));
        ATLVERIFY(m_FontColor.Initialise(GetDlgItem(IDC_COLOUR_STATIC),
                                         VisConfig().m_Text.m_Color));

        ATLASSERT(IsDlgItem(IDC_SCROLL_MODE_V_COMBO));
        m_ScrollVCombo.Detach();
        m_ScrollVCombo.Attach(GetDlgItem(IDC_SCROLL_MODE_V_COMBO));
        ATLASSERT(m_ScrollVCombo.IsWindow());
        ATLVERIFY(m_ScrollVCombo.InsertAll());

        ATLASSERT(IsDlgItem(IDC_SCROLL_MODE_V_COMBO));
        m_ScrollHCombo.Detach();
        m_ScrollHCombo.Attach(GetDlgItem(IDC_SCROLL_MODE_H_COMBO));
        ATLASSERT(m_ScrollHCombo.IsWindow());
        ATLVERIFY(m_ScrollHCombo.InsertAll());


        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_V_COMBO));
        m_AlignVCombo.Detach();
        m_AlignVCombo.Attach(GetDlgItem(IDC_FONT_ALIGN_V_COMBO));
        ATLASSERT(m_AlignVCombo.IsWindow());
        ATLVERIFY(m_AlignVCombo.InsertAll());

        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_H_COMBO));
        m_AlignHCombo.Detach();
        m_AlignHCombo.Attach(GetDlgItem(IDC_FONT_ALIGN_H_COMBO));
        ATLASSERT(m_AlignHCombo.IsWindow());
        ATLVERIFY(m_AlignHCombo.InsertAll());

        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CFontOptionsDlg::OnUpdateUIState(_In_ UINT /*uMsg*/,
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

    /*
     * Edit boxes will **not** receive a EN_KILLFOCUS message
     * when the parent is hidden, so need to process changes
     * made that have not otherwise been dealt with.
     */
    LRESULT CFontOptionsDlg::OnShow(_In_ UINT /*uMsg*/,
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
            ATLASSERT(IsDlgItem(IDC_SCROLL_V_SPEED_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_V_SPEED_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fSpeed,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCROLL_V_DELAY_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_V_DELAY_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fDelay,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCROLL_V_GAP_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_V_GAP_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fGap,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCROLL_H_SPEED_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_H_SPEED_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fSpeed,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCROLL_H_DELAY_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_H_DELAY_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fDelay,
                                  0.001f)) {
                bConfigChanged = TRUE;
            }

            ATLASSERT(IsDlgItem(IDC_SCROLL_H_GAP_EDIT));
            if (GetTextBoxAsFloat(IDC_SCROLL_H_GAP_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fGap,
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

    //------------------------------------------------------

    LRESULT CFontOptionsDlg::OnColorStatic(_In_ UINT /*uMsg*/,
                                           _In_ WPARAM /*wParam*/,
                                           _In_ LPARAM lParam,
                                           _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());
        ATLASSERT(m_FontColor);
        const HWND hWndDialogCtrl = reinterpret_cast<HWND>(lParam);
        if (hWndDialogCtrl == m_FontColor) {
            bHandled = TRUE;
            return m_FontColor;
        }
        return 0;
    }

    //------------------------------------------------------

    LRESULT CFontOptionsDlg::OnCommand(_In_ UINT /*uMsg*/,
                                       _In_ WPARAM wParam,
                                       _In_ LPARAM /*lParam*/,
                                       _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        BOOL bConfigChanged = FALSE;

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction     = HIWORD(wParam);

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_SCROLL_MODE_V_COMBO: {
                auto scrollMode = VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode;
                if (m_ScrollVCombo.GetCurSelVal(scrollMode)) {
                    if (VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode != scrollMode) {
                        VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode = scrollMode;
                        const BOOL bEnable = VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode != Windows::GDI::ScrollMode::None ? TRUE : FALSE;
                        EnableDlgItem(IDC_SCROLL_V_SPEED_STATIC, bEnable);
                        EnableDlgItem(IDC_SCROLL_V_SPEED_EDIT  , bEnable);
                        EnableDlgItem(IDC_SCROLL_V_DELAY_STATIC, bEnable);
                        EnableDlgItem(IDC_SCROLL_V_DELAY_EDIT  , bEnable);
                        if (VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode == Windows::GDI::ScrollMode::Loop) {
                            EnableDlgItem(IDC_SCROLL_V_GAP_STATIC, TRUE);
                            EnableDlgItem(IDC_SCROLL_V_GAP_EDIT  , TRUE);
                        } else {
                            EnableDlgItem(IDC_SCROLL_V_GAP_STATIC, FALSE);
                            EnableDlgItem(IDC_SCROLL_V_GAP_EDIT  , FALSE);
                        }
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_SCROLL_V_SPEED_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Vertical.m_Scroll.m_fSpeed,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_SCROLL_V_DELAY_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Vertical.m_Scroll.m_fDelay,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_SCROLL_V_GAP_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Vertical.m_Scroll.m_fGap,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_SCROLL_MODE_H_COMBO: {
                auto scrollMode = VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode;
                if (m_ScrollHCombo.GetCurSelVal(scrollMode)) {
                    if (VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode != scrollMode) {
                        VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode = scrollMode;
                        const BOOL bEnable = VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode != Windows::GDI::ScrollMode::None ? TRUE : FALSE;
                        EnableDlgItem(IDC_SCROLL_H_SPEED_STATIC, bEnable);
                        EnableDlgItem(IDC_SCROLL_H_SPEED_EDIT  , bEnable);
                        EnableDlgItem(IDC_SCROLL_H_DELAY_STATIC, bEnable);
                        EnableDlgItem(IDC_SCROLL_H_DELAY_EDIT  , bEnable);
                        if (VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode == Windows::GDI::ScrollMode::Loop) {
                            EnableDlgItem(IDC_SCROLL_H_GAP_STATIC, TRUE);
                            EnableDlgItem(IDC_SCROLL_H_GAP_EDIT  , TRUE);
                        } else {
                            EnableDlgItem(IDC_SCROLL_H_GAP_STATIC, FALSE);
                            EnableDlgItem(IDC_SCROLL_H_GAP_EDIT  , FALSE);
                        }
                        bConfigChanged = TRUE;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_SCROLL_H_SPEED_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Horizontal.m_Scroll.m_fSpeed,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_SCROLL_H_DELAY_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Horizontal.m_Scroll.m_fDelay,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_SCROLL_H_GAP_EDIT: {
                if (nAction == EN_KILLFOCUS) {
                    if (SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                        bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                           VisConfig().m_Text.m_Horizontal.m_Scroll.m_fGap,
                                                           0.001f);
                    }
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_OVERLAP_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  VisConfig().m_Text.m_bAllowOverlap);
                bHandled = TRUE;
                break;
            }

            case IDC_FONT_BUTTON: {
                LOGFONT font{ VisConfig().m_Text.m_Font };
                COLORREF color{ VisConfig().m_Text.m_Color };
                if (Windows::UI::CommDialog::ChooseFont(*this, &font, &color, CF_EFFECTS) == CDERR_SUCCESS) {
                    //TODO: Check if font has actually changed
                    VisConfig().m_Text.m_Font = font;
                    m_FontColor.SelectColor(color);
                    bConfigChanged = true;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_FONT_ALIGN_V_COMBO: {
                auto align = VisConfig().m_Text.m_Vertical.m_Align;
                if (m_AlignVCombo.GetCurSelVal(align)) {
                    bConfigChanged = VisConfig().m_Text.m_Vertical.m_Align != align;
                    VisConfig().m_Text.m_Vertical.m_Align = align;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_FONT_ALIGN_H_COMBO: {
                auto align = VisConfig().m_Text.m_Horizontal.m_Align;
                if (m_AlignHCombo.GetCurSelVal(align)) {
                    bConfigChanged = VisConfig().m_Text.m_Horizontal.m_Align != align;
                    VisConfig().m_Text.m_Horizontal.m_Align = align;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_COLOUR_BUTTON: {
                const auto oldColor = m_FontColor.GetColor();
                if (m_FontColor.SelectColor() &&
                    oldColor != m_FontColor.GetColor()) {
                    bConfigChanged = TRUE;
                    VisConfig().m_Text.m_Color = m_FontColor;
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

    //------------------------------------------------------

    void CFontOptionsDlg::UpdateControls() {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_OVERLAP_CHECK));
        ATLVERIFY(CheckDlgButton(IDC_OVERLAP_CHECK,
                                 VisConfig().m_Text.m_bAllowOverlap
                                 ? BST_CHECKED
                                 : BST_UNCHECKED));

        {
            ATLASSERT(m_ScrollVCombo.IsWindow());
            const auto selected = VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode;
            [[maybe_unused]]
            const auto index = m_ScrollVCombo.SelectValue(selected);
            ATLASSERT(index != CB_ERR);
        }

        ATLASSERT(IsDlgItem(IDC_SCROLL_V_SPEED_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_DELAY_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_GAP_EDIT));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_V_SPEED_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fSpeed));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_V_DELAY_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fDelay));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_V_GAP_EDIT,
                                  VisConfig().m_Text.m_Vertical.m_Scroll.m_fGap));

        {
            ATLASSERT(m_ScrollHCombo.IsWindow());
            const auto selected = VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode;
            [[maybe_unused]]
            const auto index = m_ScrollHCombo.SelectValue(selected);
            ATLASSERT(index != CB_ERR);
        }

        ATLASSERT(IsDlgItem(IDC_SCROLL_H_SPEED_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_DELAY_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_GAP_EDIT));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_H_SPEED_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fSpeed));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_H_DELAY_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fDelay));
        ATLVERIFY(SetDlgItemFloat(IDC_SCROLL_H_GAP_EDIT,
                                  VisConfig().m_Text.m_Horizontal.m_Scroll.m_fGap));

        {
            ATLASSERT(m_AlignVCombo.IsWindow());
            [[maybe_unused]]
            const auto nIndex = m_AlignVCombo.SelectValue(VisConfig().m_Text.m_Vertical.m_Align);
            ATLASSERT(nIndex != CB_ERR);
        }

        {
            ATLASSERT(m_AlignHCombo.IsWindow());
            [[maybe_unused]]
            const auto nIndex = m_AlignHCombo.SelectValue(VisConfig().m_Text.m_Horizontal.m_Align);
            ATLASSERT(nIndex != CB_ERR);
        }

        ATLASSERT(m_FontColor);
        ATLVERIFY(m_FontColor.SelectColor(VisConfig().m_Text.m_Color));
    }

    //------------------------------------------------------

    void CFontOptionsDlg::EnableControls() noexcept {
        const BOOL bEnable = IsEnabled() ? TRUE : FALSE;
        const BOOL bEnableScrollV =
            (bEnable && (VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode != Windows::GDI::ScrollMode::None)) ? TRUE : FALSE;
        const BOOL bEnableScrollH =
            (bEnable && (VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode != Windows::GDI::ScrollMode::None)) ? TRUE : FALSE;
        const auto bEnableColor = (bEnable && CanvasIsColor()) ? TRUE : FALSE;

        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_V_COMBO));
        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_V_STATIC));
        EnableDlgItem(IDC_FONT_ALIGN_V_COMBO , bEnable);
        EnableDlgItem(IDC_FONT_ALIGN_V_STATIC, bEnable);

        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_H_COMBO));
        ATLASSERT(IsDlgItem(IDC_FONT_ALIGN_H_STATIC));
        EnableDlgItem(IDC_FONT_ALIGN_H_COMBO , bEnable);
        EnableDlgItem(IDC_FONT_ALIGN_H_STATIC, bEnable);

        ATLASSERT(IsDlgItem(IDC_SCROLL_MODE_V_COMBO));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_SPEED_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_SPEED_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_DELAY_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_DELAY_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_GAP_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_V_GAP_EDIT));
        EnableDlgItem(IDC_SCROLL_MODE_V_COMBO  , bEnable);
        EnableDlgItem(IDC_SCROLL_V_SPEED_STATIC, bEnableScrollV);
        EnableDlgItem(IDC_SCROLL_V_SPEED_EDIT  , bEnableScrollV);
        EnableDlgItem(IDC_SCROLL_V_DELAY_STATIC, bEnableScrollV);
        EnableDlgItem(IDC_SCROLL_V_DELAY_EDIT  , bEnableScrollV);
        if (VisConfig().m_Text.m_Vertical.m_Scroll.m_Mode == Windows::GDI::ScrollMode::Loop) {
            EnableDlgItem(IDC_SCROLL_V_GAP_STATIC, TRUE);
            EnableDlgItem(IDC_SCROLL_V_GAP_EDIT  , TRUE);
        } else {
            EnableDlgItem(IDC_SCROLL_V_GAP_STATIC, FALSE);
            EnableDlgItem(IDC_SCROLL_V_GAP_EDIT  , FALSE);
        }

        ATLASSERT(IsDlgItem(IDC_SCROLL_MODE_H_COMBO));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_SPEED_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_SPEED_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_DELAY_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_DELAY_EDIT));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_GAP_STATIC));
        ATLASSERT(IsDlgItem(IDC_SCROLL_H_GAP_EDIT));
        EnableDlgItem(IDC_SCROLL_MODE_H_COMBO  , bEnable);
        EnableDlgItem(IDC_SCROLL_H_SPEED_STATIC, bEnableScrollH);
        EnableDlgItem(IDC_SCROLL_H_SPEED_EDIT  , bEnableScrollH);
        EnableDlgItem(IDC_SCROLL_H_DELAY_STATIC, bEnableScrollH);
        EnableDlgItem(IDC_SCROLL_H_DELAY_EDIT  , bEnableScrollH);
        if (VisConfig().m_Text.m_Horizontal.m_Scroll.m_Mode == Windows::GDI::ScrollMode::Loop) {
            EnableDlgItem(IDC_SCROLL_H_GAP_STATIC, TRUE);
            EnableDlgItem(IDC_SCROLL_H_GAP_EDIT  , TRUE);
        } else {
            EnableDlgItem(IDC_SCROLL_H_GAP_STATIC, FALSE);
            EnableDlgItem(IDC_SCROLL_H_GAP_EDIT  , FALSE);
        }

        ATLASSERT(IsDlgItem(IDC_FONT_BUTTON));
        ATLASSERT(IsDlgItem(IDC_OVERLAP_CHECK));
        EnableDlgItem(IDC_FONT_BUTTON  , bEnable);
        EnableDlgItem(IDC_OVERLAP_CHECK, bEnable);

        ATLASSERT(IsDlgItem(IDC_COLOUR_STATIC_TEXT));
        ATLASSERT(IsDlgItem(IDC_COLOUR_STATIC));
        ATLASSERT(IsDlgItem(IDC_COLOUR_BUTTON));
        EnableDlgItem(IDC_COLOUR_STATIC_TEXT, bEnableColor);
        EnableDlgItem(IDC_COLOUR_STATIC     , bEnableColor);
        EnableDlgItem(IDC_COLOUR_BUTTON     , bEnableColor);
    }

    //**************************************************************************
    // CTrackDetailsDlg
    //**************************************************************************
    LRESULT CTrackDetailsDlg::OnInit(_In_ UINT /*uMsg*/,
                                     _In_ WPARAM /*wParam*/,
                                     _In_ LPARAM /*lParam*/,
                                     _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_CFG_STATIC));
        ATLVERIFY(SetDlgItemText(IDC_CFG_STATIC, Name()));

        ATLASSERT(IsDlgItem(IDD_TABCTRL));
        m_TabCtrl.Detach();
        m_TabCtrl.Attach(GetDlgItem(IDD_TABCTRL));
        ATLASSERT(m_TabCtrl.IsWindow());

        // NOTE: The temporary objects are required here as
        //       the invoked operator= take non-const
        //       references as arguments.
        {
            auto pTextOptionsExpert = CTextOptionsExpertDlg::MakeDialog(TEXT("Text (Expert)"),
                                                                        Mode(),
                                                                        Type(),
                                                                        VisPrefConfig());
            m_pTextOptionsExpert = pTextOptionsExpert;
        }
        {
            auto pTextOptions = CTextOptionsDlg::MakeDialog(TEXT("Text"),
                                                            Mode(),
                                                            Type(),
                                                            VisPrefConfig());
            m_pTextOptions = pTextOptions;
        }
        {
            auto pFontOptions = CFontOptionsDlg::MakeDialog(TEXT("Font"),
                                                            Mode(),
                                                            Type(),
                                                            VisPrefConfig());
            m_pFontOptions = pFontOptions;
        }

        {
            [[maybe_unused]]
            const auto nTab = m_TabCtrl.AddItem(TCIF_TEXT | TCIF_IMAGE,
                                                TEXT("Text"),
                                                -1, 0);
            ATLASSERT(nTab >= 0);
        }
        {
            [[maybe_unused]]
            const auto nTab = m_TabCtrl.AddItem(TCIF_TEXT | TCIF_IMAGE,
                                                TEXT("Font"),
                                                -1, 0);
            ATLASSERT(nTab >= 0);
        }
        m_TabCtrl.SetCurSel(0);
        ShowCurrentTab();

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
    LRESULT CTrackDetailsDlg::OnShow(_In_ UINT /*uMsg*/,
                                     _In_ WPARAM wParam,
                                     _In_ LPARAM /*lParam*/,
                                     _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        // "If wParam is TRUE, the window is being shown.
        //  If wParam is FALSE, the window is being hidden."
        if (wParam) {
            UpdateControls();
            EnableControls();
            ShowCurrentTab();
        }
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CTrackDetailsDlg::OnChangeUIState(_In_ UINT /*uMsg*/,
                                              _In_ WPARAM /*wParam*/,
                                              _In_ LPARAM /*lParam*/,
                                              _Inout_ BOOL& bHandled) noexcept {
        SendMessageToParent(WM_CHANGEUISTATE);
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CTrackDetailsDlg::OnUpdateUIState(_In_ UINT /*uMsg*/,
                                              _In_ WPARAM /*wParam*/,
                                              _In_ LPARAM /*lParam*/,
                                              _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());
        UpdateControls();
        EnableControls();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    void CTrackDetailsDlg::ShowCurrentTab() {
        ATLASSERT(IsWindow());
        ATLASSERT(m_TabCtrl.IsWindow());
        switch (m_TabCtrl.GetCurSel()) {
            case -1: {
                // No Tab Selected
                break;
            }

            case 0: {
                if (Config().m_GeneralConfig.m_bExpertMode) {
                    ShowCurrentTab(m_pTextOptionsExpert);
                } else {
                    ShowCurrentTab(m_pTextOptions);
                }
                break;
            }

            case 1: {
                ShowCurrentTab(m_pFontOptions);
                break;
            }

            DEFAULT_UNREACHABLE;
        } // switch (m_TabCtrl.GetCurSel())
    }

    //------------------------------------------------------

    LRESULT CTrackDetailsDlg::OnTabChange(_In_ WPARAM /*wParam*/,
                                          _In_ LPNMHDR /*lpnmHdr*/,
                                          _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        ShowCurrentTab();
        UpdateControls();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CTrackDetailsDlg::OnCommand(_In_ UINT /*uMsg*/,
                                        _In_ WPARAM wParam,
                                        _In_ LPARAM /*lParam*/,
                                        _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());

        BOOL bConfigChanged = FALSE;

        const auto nDialogItem = LOWORD(wParam);

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
        } // switch (nDialogItem)

        if (bConfigChanged) {
            SendMessageToParent(WM_CHANGEUISTATE);
        }
        return 0;
    }

    //------------------------------------------------------

    void CTrackDetailsDlg::UpdateControls() noexcept {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ENABLED_CHECK));
        ATLVERIFY(CheckDlgButton(IDC_ENABLED_CHECK,
                                 IsEnabled()
                                 ? BST_CHECKED
                                 : BST_UNCHECKED));

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        ATLVERIFY(CheckDlgButton(IDC_START_WITH_CHECK,
                                 GetStartWithThis()
                                 ? BST_CHECKED
                                 : BST_UNCHECKED));
    }

    //-----------------------------------------------------------------------------

    void CTrackDetailsDlg::EnableControls() noexcept {
        const auto bEnable = IsEnabled() ? TRUE : FALSE;
        const BOOL bEnableStartWith = StartWithThisAvailable() ? TRUE : FALSE;

        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_START_WITH_CHECK));
        EnableDlgItem(IDC_START_WITH_CHECK, bEnableStartWith);

        m_TabCtrl.EnableWindow(bEnable);

        SendMessageToDescendants(WM_UPDATEUISTATE);
    }
} // namespace foobar::UI
