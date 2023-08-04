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
#include "foobar/UI/foobar_pref_general.h"
//--------------------------------------

//--------------------------------------
//
#include "Image_ImageLoader.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CGeneralDlg
    //**************************************************************************
    LRESULT CGeneralDlg::OnInit(_In_ UINT /*uMsg*/,
                                _In_ WPARAM /*wParam*/,
                                _In_ LPARAM /*lParam*/,
                                _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ALBUM_ART_TYPE_COMBO));
        m_AlbumArtCombo.Detach();
        m_AlbumArtCombo.Attach(GetDlgItem(IDC_ALBUM_ART_TYPE_COMBO));
        ATLASSERT(m_AlbumArtCombo.IsWindow());
        ATLVERIFY(m_AlbumArtCombo.InsertAll());

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
    LRESULT CGeneralDlg::OnShow(_In_ UINT /*uMsg*/,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        // "If wParam is TRUE, the window is being shown.
        //  If wParam is FALSE, the window is being hidden."
        if (wParam) {
            UpdateControls();
            EnableControls();
            bHandled = TRUE;
        } else {
            ATLASSERT(IsDlgItem(IDC_RAND_TIME_EDIT));
            if (GetTextBoxAsFloat(IDC_RAND_TIME_EDIT,
                                  GeneralConfig().m_Visualisation.m_AutoChange.m_fChangeSeconds,
                                  0.001f)) {
                SendMessageToParent(WM_CHANGEUISTATE);
            }

            ATLASSERT(IsDlgItem(IDC_TRACK_INFO_TIME_EDIT));
            if (GetTextBoxAsFloat(IDC_TRACK_INFO_TIME_EDIT,
                                  GeneralConfig().m_Visualisation.m_OnTrackChange.m_fShowSeconds,
                                  0.001f)) {
                SendMessageToParent(WM_CHANGEUISTATE);
            }
            bHandled = TRUE;
        }
        SendMessageToDescendants(WM_SHOWWINDOW, wParam, lParam);
        return 0;
    }

    //------------------------------------------------------

    LRESULT CGeneralDlg::OnChangeUIState(_In_ UINT /*uMsg*/,
                                         _In_ WPARAM /*wParam*/,
                                         _In_ LPARAM /*lParam*/,
                                         _Inout_ BOOL& bHandled) noexcept {
        SendMessageToParent(WM_CHANGEUISTATE);
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CGeneralDlg::OnCommand(_In_ UINT /*uMsg*/,
                                   _In_ WPARAM wParam,
                                   _In_ LPARAM /*lParam*/,
                                   _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        const auto nDialogItem = LOWORD(wParam);
        const auto nAction = HIWORD(wParam);

        bool bConfigChanged = false;

        gsl_suppress(26818) // C26818: Switch statement does not cover all cases. Consider adding a 'default' label (es.79).
        switch (nDialogItem) {
            case IDC_ALLOW_HW_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Canvas.m_bPreferHardwareCanvas);
                bHandled = TRUE;
                break;
            }

            case IDC_VSYNC_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Display.m_bVSync);
                bHandled = TRUE;
                break;
            }

            case IDC_FORCE_FG_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Display.m_bForceToForeground);
                bHandled = TRUE;
                break;
            }

            case IDC_BG_PAUSE_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Display.m_bBackgroundOnPause);
                bHandled = TRUE;
                break;
            }

            case IDC_RANDOM_VIS_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Visualisation.m_AutoChange.m_bEnable);
                if (bConfigChanged) {
                    const BOOL bEnable = GeneralConfig().m_Visualisation.m_AutoChange.m_bEnable ? TRUE : FALSE;
                    EnableDlgItem(IDC_RAND_TIME_EDIT,   bEnable);
                    EnableDlgItem(IDC_RAND_TIME_STATIC, bEnable);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_PRIORITY_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Display.m_bPopUpOnTrackChange);
                bHandled = TRUE;
                break;
            }

            case IDC_TRACK_INFO_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Visualisation.m_OnTrackChange.m_bShowTrackDetails);
                if (bConfigChanged) {
                    const BOOL bEnable = GeneralConfig().m_Visualisation.m_OnTrackChange.m_bShowTrackDetails ? TRUE : FALSE;
                    EnableDlgItem(IDC_TRACK_INFO_TIME_EDIT  , bEnable);
                    EnableDlgItem(IDC_TRACK_INFO_TIME_STATIC, bEnable);
                    EnableDlgItem(IDC_BG_MODE_CHECK         , bEnable);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_REMEBER_RADIO:
                [[fallthrough]];
            case IDC_START_RADIO: {
                bConfigChanged = GetCheckBoxState(IDC_REMEBER_RADIO,
                                                  GeneralConfig().m_Visualisation.m_Start.m_bRememberLast);
                bHandled = TRUE;
                break;
            }

            case IDC_EXPERT_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_bExpertMode);
                if (bConfigChanged) {
                    const BOOL bEnable = GeneralConfig().m_bExpertMode ? TRUE : FALSE;
                    EnableDlgItem(IDC_FORCE_FG_CHECK, bEnable);
                    EnableDlgItem(IDC_VSYNC_CHECK, bEnable);
                    EnableDlgItem(IDC_PRIORITY_CHECK, bEnable);
                }
                bHandled = TRUE;
                break;
            }

            /*
            case IDC_NORMAL_MODE_RADIO: {
                bool bNormalMode = GeneralConfig().m_Visualisation.m_DisplayMode == VisualisationDisplayMode::Normal;
                if (GetCheckBoxState(nDialogItem, bNormalMode)) {
                    bConfigChanged = true;
                    GeneralConfig().m_Visualisation.m_DisplayMode = VisualisationDisplayMode::Normal;
                }
                bHandled = TRUE;
                break;
            }
            */

            case IDC_PERM_PROGRESS_CHECK: { // case IDC_PERM_PROGRESS_RADIO: {
                bool bPermanentProgressBar = GeneralConfig().m_Visualisation.m_DisplayMode == VisualisationDisplayMode::PermanentProgressBar;
                if (GetCheckBoxState(nDialogItem, bPermanentProgressBar)) {
                    bConfigChanged = true;
                    GeneralConfig().m_Visualisation.m_DisplayMode = VisualisationDisplayMode::PermanentProgressBar;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_PERM_SONG_INFO_CHECK: { // case IDC_PERM_SONG_INFO_RADIO: {
                bool bPermanentTrackDetails = GeneralConfig().m_Visualisation.m_DisplayMode == VisualisationDisplayMode::PermanentTrackDetails;
                if (GetCheckBoxState(nDialogItem, bPermanentTrackDetails)) {
                    bConfigChanged = true;
                    GeneralConfig().m_Visualisation.m_DisplayMode = VisualisationDisplayMode::PermanentTrackDetails;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_SLIT_SCREEN_CHECK: { // case IDC_SLIT_SCREEN_RADIO: {
                bool bSplitScreenMode = GeneralConfig().m_Visualisation.m_DisplayMode == VisualisationDisplayMode::SplitScreen;
                if (GetCheckBoxState(nDialogItem, bSplitScreenMode)) {
                    bConfigChanged = true;
                    GeneralConfig().m_Visualisation.m_DisplayMode = VisualisationDisplayMode::SplitScreen;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_TRAIL_EFFECT_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Canvas.m_bUseTrailEffect);
                bHandled = TRUE;
                break;
            }

            case IDC_BG_NONE_RADIO:
                [[fallthrough]];
            case IDC_BG_ART_RADIO:
                [[fallthrough]];
            case IDC_BG_FILE_RADIO: {
                const auto eOldMode = GeneralConfig().m_Canvas.m_Wallpaper.m_Mode;
                if (IsDlgButtonChecked(IDC_BG_ART_RADIO)) {
                    GeneralConfig().m_Canvas.m_Wallpaper.m_Mode = WallpaperMode::AlbumArt;
                } else if (IsDlgButtonChecked(IDC_BG_FILE_RADIO)) {
                    GeneralConfig().m_Canvas.m_Wallpaper.m_Mode = WallpaperMode::File;
                } else if (IsDlgButtonChecked(IDC_BG_NONE_RADIO)) {
                    GeneralConfig().m_Canvas.m_Wallpaper.m_Mode = WallpaperMode::None;
                }
                if (eOldMode != GeneralConfig().m_Canvas.m_Wallpaper.m_Mode) {
                    bConfigChanged = true;
                    switch (GeneralConfig().m_Canvas.m_Wallpaper.m_Mode) {
                        case WallpaperMode::AlbumArt:
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , TRUE);
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , TRUE);
                            EnableDlgItem(IDC_BG_FILE_BUTTON         , FALSE);
                            EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , TRUE);
                            EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, TRUE);
                            break;
                        case WallpaperMode::File:
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , FALSE);
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , FALSE);
                            EnableDlgItem(IDC_BG_FILE_BUTTON         , TRUE);
                            EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , TRUE);
                            EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, TRUE);
                            break;
                        case WallpaperMode::None:
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , FALSE);
                            EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , FALSE);
                            EnableDlgItem(IDC_BG_FILE_BUTTON         , FALSE);
                            EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , FALSE);
                            EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, FALSE);
                            break;
                    }
                }
                bHandled = TRUE;
                break;
            }

            case IDC_RAND_TIME_EDIT: {
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                        GeneralConfig().m_Visualisation.m_AutoChange.m_fChangeSeconds,
                                                        0.001f);
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_BG_MODE_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Display.m_bBackgroundMode);
                bHandled = TRUE;
                break;
            }

            case IDC_TRACK_INFO_TIME_EDIT: {
                if (nAction == EN_KILLFOCUS &&
                    SendDlgItemMessage(nDialogItem, EM_GETMODIFY)) {
                    bConfigChanged = GetTextBoxAsFloat(nDialogItem,
                                                       GeneralConfig().m_Visualisation.m_OnTrackChange.m_fShowSeconds,
                                                       0.001f);
                    bHandled = TRUE;
                }
                break;
            }

            case IDC_BG_PIC_STRETCH_CHECK: {
                bConfigChanged = GetCheckBoxState(nDialogItem,
                                                  GeneralConfig().m_Canvas.m_Wallpaper.m_bStretchToFit);
                bHandled = TRUE;
                break;
            }

            case IDC_BG_PIC_CLEAR_TEXT_CHECK: {
                for (const auto page : TrackDetailsType{}) {
                    bConfigChanged = bConfigChanged || GetCheckBoxState(nDialogItem, CoreConfig().m_TrackDetails[page].m_Text.m_bClearBackground);
                }
                bHandled = TRUE;
                break;
            }

            case IDC_ALBUM_ART_TYPE_COMBO: {
                auto albumArtType = GeneralConfig().m_Canvas.m_Wallpaper.m_AlbumArtType;
                if (m_AlbumArtCombo.GetCurSelVal(albumArtType)) {
                    bConfigChanged = GeneralConfig().m_Canvas.m_Wallpaper.m_AlbumArtType != albumArtType;
                    GeneralConfig().m_Canvas.m_Wallpaper.m_AlbumArtType = albumArtType;
                }
                bHandled = TRUE;
                break;
            }

            case IDC_BG_FILE_BUTTON: {
                CString strFile;
                // Supported image types:
                //  https://learn.microsoft.com/en-gb/windows/win32/gdiplus/-gdiplus-using-image-encoders-and-decoders-use
                constexpr const auto szFilter{ TEXT("*.bmp;*.emf;*.ico;*.jpg;*.jpeg;*.gif;*.png;*.tiff;*.tif;*.wmf\0\0") };
                if (Windows::UI::CommDialog::GetOpenFileName(*this, strFile, szFilter) == CDERR_SUCCESS) {
                    if (auto image = ::Image::ImageLoader::Load(strFile)) {
                        GeneralConfig().m_Canvas.m_Wallpaper.m_File.assign(strFile.GetString());
                        SetDlgItemText(IDC_BG_FILE_EDIT, GeneralConfig().m_Canvas.m_Wallpaper.m_File.c_str());
                        bConfigChanged = true;
                    } else {
                        ::MessageBoxExW(*this, L"Selected file could not be loaded as an image.",
                                        NULL, MB_OK | MB_ICONERROR | MB_APPLMODAL,
                                        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
                    }
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

    void CGeneralDlg::UpdateControls() {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_ALLOW_HW_CHECK));
        ATLASSERT(IsDlgItem(IDC_VSYNC_CHECK));
        ATLASSERT(IsDlgItem(IDC_BG_PAUSE_CHECK));
        ATLASSERT(IsDlgItem(IDC_FORCE_FG_CHECK));
        WinAPIVerify(CheckDlgButton(IDC_ALLOW_HW_CHECK,
                                    GeneralConfig().m_Canvas.m_bPreferHardwareCanvas));
        WinAPIVerify(CheckDlgButton(IDC_VSYNC_CHECK,
                                    GeneralConfig().m_Display.m_bVSync));
        WinAPIVerify(CheckDlgButton(IDC_BG_PAUSE_CHECK,
                                    GeneralConfig().m_Display.m_bBackgroundOnPause));
        WinAPIVerify(CheckDlgButton(IDC_FORCE_FG_CHECK,
                                    GeneralConfig().m_Display.m_bForceToForeground));

        ATLASSERT(IsDlgItem(IDC_RANDOM_VIS_CHECK));
        ATLASSERT(IsDlgItem(IDC_RAND_TIME_EDIT));
        WinAPIVerify(CheckDlgButton(IDC_RANDOM_VIS_CHECK,
                                    GeneralConfig().m_Visualisation.m_AutoChange.m_bEnable));
        WinAPIVerify(SetDlgItemFloat(IDC_RAND_TIME_EDIT,
                                     GeneralConfig().m_Visualisation.m_AutoChange.m_fChangeSeconds));

        WinAPIVerify(CheckDlgButton(IDC_PRIORITY_CHECK,
                                    GeneralConfig().m_Display.m_bPopUpOnTrackChange));

        WinAPIVerify(CheckDlgButton(IDC_TRACK_INFO_CHECK,
                                    GeneralConfig().m_Visualisation.m_OnTrackChange.m_bShowTrackDetails));

        WinAPIVerify(SetDlgItemFloat(IDC_TRACK_INFO_TIME_EDIT,
                                     GeneralConfig().m_Visualisation.m_OnTrackChange.m_fShowSeconds));
        WinAPIVerify(CheckDlgButton(IDC_BG_MODE_CHECK,
                                    GeneralConfig().m_Display.m_bBackgroundMode));

        WinAPIVerify(CheckDlgButton(IDC_REMEBER_RADIO,
                                    GeneralConfig().m_Visualisation.m_Start.m_bRememberLast));
        WinAPIVerify(CheckDlgButton(IDC_START_RADIO ,
                                    !GeneralConfig().m_Visualisation.m_Start.m_bRememberLast));
        WinAPIVerify(CheckDlgButton(IDC_EXPERT_CHECK,
                                    GeneralConfig().m_bExpertMode));

        //WinAPIVerify(CheckDlgButton(IDC_PERM_PROGRESS_CHECK,
        //                            GeneralConfig().m_Visualisation.m_bPermanentProgressBar));
        //WinAPIVerify(CheckDlgButton(IDC_PERM_SONG_INFO_CHECK,
        //                            GeneralConfig().m_Visualisation.m_bPermanentTrackDetails));
        //WinAPIVerify(CheckDlgButton(IDC_SLIT_SCREEN_CHECK,
        //                            GeneralConfig().m_Canvas.m_bSplitScreenMode));

        if (GeneralConfig().m_Canvas.m_bPreferHardwareCanvas) {
            WinAPIVerify(CheckDlgButton(IDC_TRAIL_EFFECT_CHECK, GeneralConfig().m_Canvas.m_bUseTrailEffect));
        } else {
            WinAPIVerify(CheckDlgButton(IDC_TRAIL_EFFECT_CHECK, FALSE));
        }

        switch (GeneralConfig().m_Canvas.m_Wallpaper.m_Mode) {
            case WallpaperMode::File:
                WinAPIVerify(CheckDlgButton(IDC_BG_NONE_RADIO, FALSE));
                WinAPIVerify(CheckDlgButton(IDC_BG_ART_RADIO,  FALSE));
                WinAPIVerify(CheckDlgButton(IDC_BG_FILE_RADIO, TRUE));
                break;
            case WallpaperMode::AlbumArt:
                WinAPIVerify(CheckDlgButton(IDC_BG_NONE_RADIO, FALSE));
                WinAPIVerify(CheckDlgButton(IDC_BG_ART_RADIO,  TRUE));
                WinAPIVerify(CheckDlgButton(IDC_BG_FILE_RADIO, FALSE));
                break;
            case WallpaperMode::None:
                [[fallthrough]];
            default:
                WinAPIVerify(CheckDlgButton(IDC_BG_NONE_RADIO, TRUE));
                WinAPIVerify(CheckDlgButton(IDC_BG_ART_RADIO,  FALSE));
                WinAPIVerify(CheckDlgButton(IDC_BG_FILE_RADIO, FALSE));
                break;
        }

        ATLASSERT(m_AlbumArtCombo.IsWindow());
        m_AlbumArtCombo.SelectValue(GeneralConfig().m_Canvas.m_Wallpaper.m_AlbumArtType);

        SetDlgItemText(IDC_BG_FILE_EDIT, GeneralConfig().m_Canvas.m_Wallpaper.m_File.c_str());
        WinAPIVerify(CheckDlgButton(IDC_BG_PIC_STRETCH_CHECK, GeneralConfig().m_Canvas.m_Wallpaper.m_bStretchToFit));
        WinAPIVerify(CheckDlgButton(IDC_BG_PIC_CLEAR_TEXT_CHECK, CoreConfig().m_TrackDetails[TrackDetailsType::Page1].m_Text.m_bClearBackground)); //!!FIXME!! - move this to track info page.
    }

    //------------------------------------------------------

    void CGeneralDlg::EnableControls() noexcept {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_RAND_TIME_EDIT));
        ATLASSERT(IsDlgItem(IDC_RAND_TIME_STATIC));
        if (GeneralConfig().m_Visualisation.m_AutoChange.m_bEnable) {
            EnableDlgItem(IDC_RAND_TIME_EDIT  , TRUE);
            EnableDlgItem(IDC_RAND_TIME_STATIC, TRUE);
        } else {
            EnableDlgItem(IDC_RAND_TIME_EDIT  , FALSE);
            EnableDlgItem(IDC_RAND_TIME_STATIC, FALSE);
        }

        ATLASSERT(IsDlgItem(IDC_TRACK_INFO_TIME_EDIT));
        ATLASSERT(IsDlgItem(IDC_TRACK_INFO_TIME_STATIC));
        ATLASSERT(IsDlgItem(IDC_BG_MODE_CHECK));
        if (GeneralConfig().m_Visualisation.m_OnTrackChange.m_bShowTrackDetails) {
            EnableDlgItem(IDC_TRACK_INFO_TIME_EDIT  , TRUE);
            EnableDlgItem(IDC_TRACK_INFO_TIME_STATIC, TRUE);
            EnableDlgItem(IDC_BG_MODE_CHECK         , TRUE);
        } else {
            EnableDlgItem(IDC_TRACK_INFO_TIME_EDIT  , FALSE);
            EnableDlgItem(IDC_TRACK_INFO_TIME_STATIC, FALSE);
            EnableDlgItem(IDC_BG_MODE_CHECK         , FALSE);
        }

        ATLASSERT(IsDlgItem(IDC_FORCE_FG_CHECK));
        ATLASSERT(IsDlgItem(IDC_VSYNC_CHECK));
        ATLASSERT(IsDlgItem(IDC_PRIORITY_CHECK));
        if (GeneralConfig().m_bExpertMode) {
            EnableDlgItem(IDC_FORCE_FG_CHECK, TRUE);
            EnableDlgItem(IDC_VSYNC_CHECK   , TRUE);
            EnableDlgItem(IDC_PRIORITY_CHECK, TRUE);
        } else {
            EnableDlgItem(IDC_FORCE_FG_CHECK, FALSE);
            EnableDlgItem(IDC_VSYNC_CHECK   , FALSE);
            EnableDlgItem(IDC_PRIORITY_CHECK, FALSE);
        }

        if (CanvasConfig().bColor) {
            ATLASSERT(IsDlgItem(IDC_COLOUR_LCD_STATIC));
            ATLASSERT(IsDlgItem(IDC_SLIT_SCREEN_CHECK));
            ATLASSERT(IsDlgItem(IDC_TRAIL_EFFECT_CHECK));
            EnableDlgItem(IDC_COLOUR_LCD_STATIC,  TRUE);
            EnableDlgItem(IDC_SLIT_SCREEN_CHECK,  TRUE);
            EnableDlgItem(IDC_TRAIL_EFFECT_CHECK, TRUE);

            ATLASSERT(IsDlgItem(IDC_BACKGROUND_IMG_STATIC));
            ATLASSERT(IsDlgItem(IDC_BG_NONE_RADIO));
            ATLASSERT(IsDlgItem(IDC_BG_ART_RADIO));
            ATLASSERT(IsDlgItem(IDC_BG_FILE_RADIO));
            EnableDlgItem(IDC_BACKGROUND_IMG_STATIC, TRUE);
            EnableDlgItem(IDC_BG_NONE_RADIO        , TRUE);
            EnableDlgItem(IDC_BG_ART_RADIO         , TRUE);
            EnableDlgItem(IDC_BG_FILE_RADIO        , TRUE);

            ATLASSERT(IsDlgItem(IDC_BG_FILE_EDIT));
            ATLASSERT(IsDlgItem(IDC_BG_FILE_BUTTON));
            ATLASSERT(IsDlgItem(IDC_ALBUM_ART_TYPE_COMBO));
            ATLASSERT(IsDlgItem(IDC_ALBUM_ART_TYPE_STATIC));
            ATLASSERT(IsDlgItem(IDC_BG_PIC_STRETCH_CHECK));
            ATLASSERT(IsDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK));
            EnableDlgItem(IDC_BG_FILE_EDIT, FALSE); //Never editable on purpose
            switch (GeneralConfig().m_Canvas.m_Wallpaper.m_Mode) {
                case WallpaperMode::AlbumArt:
                    EnableDlgItem(IDC_BG_FILE_BUTTON         , FALSE);

                    EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , TRUE);
                    EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , TRUE);

                    EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , TRUE);
                    EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, TRUE);
                    break;
                case WallpaperMode::File:
                    EnableDlgItem(IDC_BG_FILE_BUTTON         , TRUE);

                    EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , FALSE);
                    EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , FALSE);

                    EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , TRUE);
                    EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, TRUE);
                    break;
                case WallpaperMode::None:
                    EnableDlgItem(IDC_BG_FILE_BUTTON         , FALSE);

                    EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , FALSE);
                    EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , FALSE);

                    EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , FALSE);
                    EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, FALSE);
                    break;
            }
        } else {
            ATLASSERT(IsDlgItem(IDC_COLOUR_LCD_STATIC));
            ATLASSERT(IsDlgItem(IDC_SLIT_SCREEN_CHECK));
            ATLASSERT(IsDlgItem(IDC_TRAIL_EFFECT_CHECK));
            EnableDlgItem(IDC_COLOUR_LCD_STATIC,  FALSE);
            EnableDlgItem(IDC_SLIT_SCREEN_CHECK,  FALSE);
            EnableDlgItem(IDC_TRAIL_EFFECT_CHECK, FALSE);

            ATLASSERT(IsDlgItem(IDC_BACKGROUND_IMG_STATIC));
            ATLASSERT(IsDlgItem(IDC_BG_NONE_RADIO));
            ATLASSERT(IsDlgItem(IDC_BG_ART_RADIO));
            ATLASSERT(IsDlgItem(IDC_BG_FILE_RADIO));
            EnableDlgItem(IDC_BACKGROUND_IMG_STATIC, FALSE);
            EnableDlgItem(IDC_BG_NONE_RADIO        , FALSE);
            EnableDlgItem(IDC_BG_ART_RADIO         , FALSE);
            EnableDlgItem(IDC_BG_FILE_RADIO        , FALSE);

            ATLASSERT(IsDlgItem(IDC_BG_FILE_EDIT));
            ATLASSERT(IsDlgItem(IDC_BG_FILE_BUTTON));
            ATLASSERT(IsDlgItem(IDC_ALBUM_ART_TYPE_COMBO));
            ATLASSERT(IsDlgItem(IDC_ALBUM_ART_TYPE_STATIC));
            ATLASSERT(IsDlgItem(IDC_BG_PIC_STRETCH_CHECK));
            ATLASSERT(IsDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK));
            EnableDlgItem(IDC_BG_FILE_EDIT           , FALSE);
            EnableDlgItem(IDC_BG_FILE_BUTTON         , FALSE);
            EnableDlgItem(IDC_ALBUM_ART_TYPE_COMBO   , FALSE);
            EnableDlgItem(IDC_ALBUM_ART_TYPE_STATIC  , FALSE);
            EnableDlgItem(IDC_BG_PIC_STRETCH_CHECK   , FALSE);
            EnableDlgItem(IDC_BG_PIC_CLEAR_TEXT_CHECK, FALSE);
        }
    }
} // namespace foobar::UI

