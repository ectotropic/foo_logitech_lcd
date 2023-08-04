#pragma once
#ifndef GUID_3E047461_1E0C_4CCF_A172_6EA19018294A
#define GUID_3E047461_1E0C_4CCF_A172_6EA19018294A
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
#include "Windows/UI/UI.h"
#include "Windows/UI/ATL_Window.h"
//--------------------------------------

//--------------------------------------
//
#include <cerrno>
#include <cstring>
#include <cwchar>
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // CDialogWindow
    //**************************************************************************
    class ATL_NO_VTABLE CDialogWindow : public CATLWindow {
    private:
        using thisClass = CDialogWindow;
        using baseClass = CATLWindow;

    private:
        static inline float StringToFloat(_In_z_ LPCSTR szFloat,
                                          _Out_ LPSTR* pEnd) noexcept {
            return std::strtof(szFloat, pEnd);
        }

        //------------------------------

        static inline float StringToFloat(_In_z_ LPCWSTR szFloat,
                                          _Out_ LPWSTR* pEnd) noexcept {
            return std::wcstof(szFloat, pEnd);
        }

        //------------------------------

        template <typename CharT, typename FloatT>
        static inline BOOL StringToFloat(_In_z_ FAR const CharT* szFloat,
                                         _Out_ FloatT& fValue) noexcept {
            ::_set_errno(0);
            FAR CharT* pEnd{ nullptr };
            fValue = StringToFloat(szFloat, &pEnd);
            errno_t err;
            ::_get_errno(&err);
            if (err == ERANGE || !pEnd || *pEnd != '\0') {
                ::SetLastError(ERROR_INVALID_DATA);
                return FALSE;
            }
            return TRUE;
        }

    public:
        using windowClass     = baseClass::windowClass;
        using stringClass     = baseClass::stringClass;
        using stringBaseClass = baseClass::stringBaseClass;

    public:
        auto IsDlgItem(_In_ INT nIDDlgItem) const throw() {
            ATLASSERT(IsWindow());
            const auto item = GetDlgItem(nIDDlgItem);
            return item && item.IsWindow();
        }

        //------------------------------

        LRESULT SendDlgItemMessage(_In_ INT nIDDlgItem,
                                   _In_ UINT uMsg,
                                   _In_ WPARAM wParam = 0,
                                   _In_ LPARAM lParam = 0) throw() {
            ATLASSERT(IsWindow());
            auto item = GetDlgItem(nIDDlgItem);
            ATLASSERT(item.IsWindow());
            if (!item.IsWindow()) { return 0; }
            return item.SendMessageW(uMsg, wParam, lParam);
        }

        //------------------------------

        auto EnableDlgItem(_In_ INT nIDDlgItem,
                           _In_ BOOL bEnable) const throw() {
            ATLASSERT(IsWindow());
            auto item = GetDlgItem(nIDDlgItem);
            ATLASSERT(item.IsWindow());
            if (!item.IsWindow()) { return FALSE; }
            return item.EnableWindow(bEnable);
        }

        //------------------------------

        auto ShowDlgItem(_In_ INT nIDDlgItem,
                         _In_ INT nCmdShow) const throw() {
            ATLASSERT(IsWindow());
            auto item = GetDlgItem(nIDDlgItem);
            ATLASSERT(item.IsWindow());
            if (!item.IsWindow()) { return FALSE; }
            return item.ShowWindow(nCmdShow);
        }

        //------------------------------

        auto GetDlgItemTextLength(_In_ INT nIDDlgItem) const throw() {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            const auto item = GetDlgItem(nIDDlgItem);
            ATLASSERT(item.IsWindow());
            return item.IsWindow() ? item.GetWindowTextLength() : 0;
        }

        //------------------------------

        auto GetDlgItemTextLength(_In_ INT nIDDlgItem,
                                  _Out_ LPUINT lpLength) const throw() {
            ATLASSERT(lpLength);
            if (!lpLength) { return FALSE; }
            (*lpLength) = 0;
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            if (const auto uLength = GetDlgItemTextLength(nIDDlgItem)) {
                (*lpLength) = uLength;
                return TRUE;
            } else {
                WinAPIAssertErrorSuccess();
                return (::GetLastError() == ERROR_SUCCESS) ? TRUE : FALSE;
            }
        }

        //------------------------------

        auto GetDlgItemText(_In_ INT nIDDlgItem,
                            _Inout_ stringBaseClass& strText) const {
            if (!strText.IsEmpty()) { strText.Empty(); }
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            const auto cch = GetDlgItem(nIDDlgItem).GetWindowText(strText);
            WinAPIAssertErrorSuccess();
            return cch;
        }

        //------------------------------

        auto GetDlgItemSignedInt(_In_ INT nIDDlgItem,
                                 _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            return (INT)GetDlgItemInt(nIDDlgItem, lpTranslated, TRUE);
        }

        //------------------------------

        auto GetDlgItemSignedInt(_In_ INT nIDDlgItem,
                                 _In_ INT nMin,
                                 _In_ INT nMax,
                                 _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            if (lpTranslated) { (*lpTranslated) = FALSE; }
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            INT nValue = GetDlgItemSignedInt(nIDDlgItem,
                                             &bTranslated);
            if (!bTranslated) { return 0; }
            if (nValue < nMin) { bTranslated = DLG_TRANS_CLAMPED; nValue = nMin; }
            if (nValue > nMax) { bTranslated = DLG_TRANS_CLAMPED; nValue = nMax; }
            if (lpTranslated) { (*lpTranslated) = bTranslated; }
            return nValue;
        }

        //------------------------------

        auto SetDlgItemSignedInt(_In_ INT nIDDlgItem,
                                 _In_ INT nValue) throw() {
            const auto res = SetDlgItemInt(nIDDlgItem, static_cast<UINT>(nValue), TRUE);
            WinAPIAssert(res);
            return res;
        }

        //------------------------------

        auto GetDlgItemUnsignedInt(_In_ INT nIDDlgItem,
                                   _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            return (UINT)GetDlgItemInt(nIDDlgItem, lpTranslated, FALSE);
        }

        //------------------------------

        auto GetDlgItemUnsignedInt(_In_ INT nIDDlgItem,
                                   _In_ UINT nMin,
                                   _In_ UINT nMax,
                                   _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            if (lpTranslated) { (*lpTranslated) = FALSE; }
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            BOOL bTranslated = FALSE;
            UINT nValue = GetDlgItemUnsignedInt(nIDDlgItem,
                                                &bTranslated);
            if (!bTranslated) { return 0U; }
            if (nValue < nMin) { bTranslated = DLG_TRANS_CLAMPED; nValue = nMin; }
            if (nValue > nMax) { bTranslated = DLG_TRANS_CLAMPED; nValue = nMax; }
            if (lpTranslated) { (*lpTranslated) = bTranslated; }
            return nValue;
        }

        //------------------------------

        auto SetDlgItemUnsignedInt(_In_ INT nIDDlgItem,
                                   _In_ UINT nValue) throw() {
            const auto res = SetDlgItemInt(nIDDlgItem, nValue, FALSE);
            WinAPIAssert(res);
            return res;
        }

        //------------------------------

        auto GetDlgItemFloat(_In_ INT nIDDlgItem,
                             _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            if (lpTranslated) { (*lpTranslated) = FALSE; }
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            CString strText;
            FLOAT fValue = 0.f;
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            const auto res = GetDlgItemText(nIDDlgItem, strText);
            WinAPIAssert(res);
            if (res) {
                const auto bTranslated = StringToFloat(strText.GetString(), fValue);
                if (lpTranslated) { (*lpTranslated) = bTranslated; }
            }
            return fValue;
        }

        //------------------------------

        auto GetDlgItemFloat(_In_ INT nIDDlgItem,
                             _In_ FLOAT fMin,
                             _In_ FLOAT fMax,
                             _Out_opt_ LPBOOL lpTranslated = NULL) throw() {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            if (lpTranslated) { (*lpTranslated) = FALSE; }
            BOOL bTranslated = FALSE;
            FLOAT fValue = GetDlgItemFloat(nIDDlgItem,
                                           &bTranslated);
            if (!bTranslated) { return 0.f; }
            if (fValue < fMin) { bTranslated = DLG_TRANS_CLAMPED; fValue = fMin; }
            if (fValue > fMax) { bTranslated = DLG_TRANS_CLAMPED; fValue = fMax; }
            if (lpTranslated) { (*lpTranslated) = bTranslated; }
            return fValue;
        }

        //------------------------------

        auto SetDlgItemFloat(_In_ INT nIDDlgItem,
                             _In_ FLOAT fValue) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            stringClass strFloat{};
            strFloat.Format(TEXT("%.2f"), fValue);
            const auto res = SetDlgItemTextW(nIDDlgItem, strFloat);
            WinAPIAssert(res);
            return res;
        }

        //------------------------------

        auto SetDlgItemFloat(_In_ INT nIDDlgItem,
                             _In_ FLOAT fValue,
                             _In_ INT nPrecision) {
            ATLASSERT(IsWindow());
            ATLASSERT(IsDlgItem(nIDDlgItem));
            stringClass strFmt{};
            strFmt.Format(TEXT("%%.%df"), nPrecision);
            stringClass strFloat{};
            strFloat.Format(strFmt.GetString(), fValue);
            const auto res = SetDlgItemTextW(nIDDlgItem, strFloat);
            WinAPIAssert(res);
            return res;
        }
    }; // class CDialogWindow
} // namespace Windows::UI

#endif // GUID_3E047461_1E0C_4CCF_A172_6EA19018294A