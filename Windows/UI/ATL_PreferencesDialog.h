#pragma once
#ifndef GUID_45E15AF4_AB6A_4D6B_9058_59523F24A778
#define GUID_45E15AF4_AB6A_4D6B_9058_59523F24A778
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
#include "Windows/UI/ATL_DialogWindow.h"
#include "Windows/UI/ATL_ColorSwatch.h"
//--------------------------------------

//--------------------------------------
//
#include <cstring> // memcmp
#include <cmath> // fabs
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // CPreferencesDialogT
    //**************************************************************************
    template <class BaseT>
    class ATL_NO_VTABLE CPreferencesDialogT : public BaseT {
    private:
        using thisClass = CPreferencesDialogT<BaseT>;
        using baseClass = BaseT;

    protected:
        typedef HWND(*pfnShowDialogCallBack_t)(HWND, INT, LPVOID, HWND);

        //------------------------------

        template <class DerivedT> gsl_suppress(26491) // C26491: Don't use static_cast downcasts (type.2).
        HWND ShowDialog(_In_ HWND hParentWnd,
                        _In_ INT nShowCmd,
                        _In_opt_ HWND hSizerWnd = NULL) {
            if ((nShowCmd == SW_HIDE) || (nShowCmd == SW_MINIMIZE)) {
                if (static_cast<DerivedT*>(this)->IsWindow()) {
                    static_cast<DerivedT*>(this)->ShowWindow(nShowCmd);
                }
            } else {
                if (!static_cast<DerivedT*>(this)->IsWindow()) {
                    static_cast<DerivedT*>(this)->Create(hParentWnd);
                    if (hSizerWnd) {
                        ATLASSERT(::IsWindow(hSizerWnd));
                        RECT SizerRect;
                        ZeroMemory(&SizerRect, sizeof(RECT));
                        ATLVERIFY(::GetWindowRect(hSizerWnd, &SizerRect));
                        const auto nWidth = SizerRect.right - SizerRect.left;
                        ATLASSERT(nWidth > 0);
                        const auto nHeight = SizerRect.bottom - SizerRect.top;
                        ATLASSERT(nHeight > 0);
                        POINT TopLeft{ SizerRect.left, SizerRect.top };
                        if (hParentWnd) {
                            ATLASSERT(::IsWindow(hParentWnd));
                            ATLVERIFY(::ScreenToClient(hParentWnd, &TopLeft));
                        } else {
                            ATLVERIFY(static_cast<DerivedT*>(this)->ScreenToClient(&TopLeft));
                        }
                        ATLVERIFY(static_cast<DerivedT*>(this)->MoveWindow(TopLeft.x, TopLeft.y, nWidth, nHeight, FALSE));
                    }
                }
                ATLASSERT(static_cast<DerivedT*>(this)->IsWindow());
                static_cast<DerivedT*>(this)->ShowWindow(nShowCmd);
            }
            return static_cast<DerivedT*>(this)->m_hWnd;
        }

    public:
        bool GetCheckBoxState(INT nIDDlgItem,
                              bool& rbStorage) noexcept {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            const auto bOld = rbStorage;
            rbStorage = IsDlgButtonChecked(nIDDlgItem) == BST_CHECKED;
            return rbStorage != bOld;
        }

        //------------------------------


        BOOL GetCheckBoxState(INT nIDDlgItem,
                              BOOL& rbStorage) noexcept {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            const auto bOld = rbStorage;
            rbStorage = IsDlgButtonChecked(nIDDlgItem) == BST_CHECKED ? TRUE : FALSE;
            return rbStorage != bOld ? TRUE : FALSE;
        }

        //------------------------------

        BOOL GetTextBoxAsFloat(INT nIDDlgItem,
                               FLOAT& rfStorage,
                               FLOAT fEpsilon) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto fNew = GetDlgItemFloat(nIDDlgItem,
                                              &bTranslated);
            if (bTranslated) {
                const auto fOld = rfStorage;
                rfStorage = fNew;
                return (std::fabs(fNew - fOld) > fEpsilon) ? TRUE : FALSE;
            } else {
                ATLVERIFY(SetDlgItemFloat(nIDDlgItem, rfStorage));
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetTextBoxAsFloat(INT nIDDlgItem,
                               FLOAT& rfStorage,
                               FLOAT fMin,
                               FLOAT fMax,
                               FLOAT fEpsilon) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto fNew = GetDlgItemFloat(nIDDlgItem,
                                              fMin, fMax,
                                              &bTranslated);
            if (bTranslated) {
                if (bTranslated == TRUE_CLAMPED) {
                    ATLVERIFY(SetDlgItemFloat(nIDDlgItem, fNew));
                }
                const auto fOld = rfStorage;
                rfStorage = fNew;
                return (std::fabs(fNew - fOld) > fEpsilon) ? TRUE : FALSE;
            } else {
                ATLVERIFY(SetDlgItemFloat(nIDDlgItem, rfStorage));
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetTextBoxAsSignedInt(INT nIDDlgItem,
                                   INT& rnStorage) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto nValue = GetDlgItemSignedInt(nIDDlgItem,
                                                    &bTranslated);
            if (bTranslated) {
                const auto nOld = rnStorage;
                rnStorage = nValue;
                return (rnStorage != nOld) ? TRUE : FALSE;
            } else {
                ATLVERIFY(SetDlgItemSignedInt(nIDDlgItem, rnStorage));
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetTextBoxAsSignedInt(INT nIDDlgItem,
                                   INT& rnStorage,
                                   INT nMin,
                                   INT nMax) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto nValue = GetDlgItemSignedInt(nIDDlgItem,
                                                    nMin, nMax,
                                                    &bTranslated);
            if (bTranslated) {
                if (bTranslated == TRUE_CLAMPED) {
                    ATLVERIFY(SetDlgItemSignedInt(nIDDlgItem, nValue));
                }
                const auto nOld = rnStorage;
                rnStorage = nValue;
                return (rnStorage != nOld) ? TRUE : FALSE;
            } else {
                ATLVERIFY(SetDlgItemSignedInt(nIDDlgItem, rnStorage));
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetTextBoxAsUnsignedInt(INT nIDDlgItem,
                                     UINT& ruStorage) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto uValue = GetDlgItemUnsignedInt(nIDDlgItem,
                                                      &bTranslated);
            if (bTranslated) {
                const auto uOld = ruStorage;
                ruStorage = uValue;
                return (ruStorage != uOld) ? TRUE : FALSE;
            } else {
                ATLVERIFY(SetDlgItemUnsignedInt(nIDDlgItem, rnStorage));
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetTextBoxAsUnsignedInt(INT nIDDlgItem,
                                     UINT& ruStorage,
                                     UINT nMin,
                                     UINT nMax) noexcept {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            const auto nValue = GetDlgItemUnsignedInt(nIDDlgItem,
                                                      nMin, nMax,
                                                      &bTranslated);
            if (bTranslated) {
                if (bTranslated == DLG_TRANS_CLAMPED) {
                    ATLVERIFY(SetDlgItemSignedInt(nIDDlgItem, nValue));
                }
                const auto uOld = ruStorage;
                ruStorage = nValue;
                return (ruStorage != uOld) ? TRUE : FALSE;
            } else {
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetSelectedColor(_Inout_ COLORREF& color,
                               _In_ DWORD flags = CC_FULLOPEN) {
            ATLASSERT(IsWindow());
            COLORREF newColor{ color };
            const auto err = CommDialog::ChooseColor((*this), &newColor, flags);
            if (err == CDERR_SUCCESS) {
                const auto oldColor = color;
                color = newColor;
                return newColor != oldColor;
            } else {
                return FALSE;
            }
        }

        //------------------------------

        BOOL GetSelectedFont(_Inout_ LOGFONT& font,
                             _Inout_ COLORREF& color,
                             _In_ DWORD flags = 0) {
            ATLASSERT(IsWindow());
            LOGFONT newFont{ font };
            COLORREF newColor{ color };
            const auto err = CommDialog::ChooseFont((*this), &newFont,
                                                    &newColor, flags);
            if (err == CDERR_SUCCESS) {
                const auto oldFont = font;
                const auto oldColor = color;
                font = newFont;
                color = newColor;
                return newColor != oldColor || (std::memcmp(&newFont, &oldFont, sizeof(LOGFONT)) != 0);
            } else {
                return FALSE;
            }
        }
    }; // template <...> class CPreferencesDialogT

    //**************************************************************************
    // CPreferencesDialog
    //**************************************************************************
    using CPreferencesDialog =
        Windows::UI::CPreferencesDialogT<Windows::UI::CDialogWindow>;
} // namespace Windows::UI

#endif //GUID_45E15AF4_AB6A_4D6B_9058_59523F24A778