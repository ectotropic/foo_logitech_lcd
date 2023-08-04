#pragma once
#ifndef GUID_4B24EC2A_01C9_4728_ABF6_88D38A3405D9
#define GUID_4B24EC2A_01C9_4728_ABF6_88D38A3405D9
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
#include "Windows/GDI/GDI.h"
//--------------------------------------

//--------------------------------------
//
#include <cwchar>
#include <ctime>
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // GetLocaleCompactDateFormat...
    //
    //  %d      Day of month as a decimal number (01 - 31)
    //  %m      Month as a decimal number (01 - 12)
    //  %y      Year without century, as decimal number (00 - 99)
    //**************************************************************************
    inline auto GetLocaleCompactDateFormatLength() noexcept { return 8 + 1; }

    //--------------------------------------------------------------------------

    inline auto GetLocaleCompactDateFormatA() noexcept {
        static constexpr const auto s_DateFormatCount = 3;
        static constexpr const char* const s_szDateFormat[s_DateFormatCount + 1] = {
            "%m/%d/%y",
            "%d/%m/%y",
            "%y/%m/%d",
            "<INVAL>",
        };

        DWORD dwDateFormatIndex = static_cast<DWORD>(-1);
        [[maybe_unused]]
        const auto len = ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                                           LOCALE_IDATE | LOCALE_RETURN_NUMBER,
                                           (LPWSTR)&dwDateFormatIndex,
                                           sizeof(DWORD));
        WinAPIAssert(len == 2);
        assert(dwDateFormatIndex >= 0 && dwDateFormatIndex < s_DateFormatCount);
        if (dwDateFormatIndex >= 0 && dwDateFormatIndex < s_DateFormatCount) {
            dwDateFormatIndex = 3;
        }
        return s_szDateFormat[dwDateFormatIndex];
    }

    //--------------------------------------------------------------------------

    inline auto GetLocaleCompactDateFormatW() noexcept {
        static constexpr const auto s_DateFormatCount = 3;
        static constexpr const wchar_t* const s_szDateFormat[s_DateFormatCount + 1] = {
            L"%m/%d/%y",
            L"%d/%m/%y",
            L"%y/%m/%d",
            L"<INVAL>",
        };

        DWORD dwDateFormatIndex = static_cast<DWORD>(-1);
        [[maybe_unused]]
        const auto len = ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                                           LOCALE_IDATE | LOCALE_RETURN_NUMBER,
                                           (LPWSTR)&dwDateFormatIndex,
                                           sizeof(DWORD));
        WinAPIAssert(len == 2);
        assert(dwDateFormatIndex >= 0 && dwDateFormatIndex < s_DateFormatCount);
        if (dwDateFormatIndex >= s_DateFormatCount) {
            dwDateFormatIndex = 3;
        }
        return s_szDateFormat[dwDateFormatIndex];
    }

    //--------------------------------------------------------------------------

#ifdef _UNICODE
    inline auto GetLocaleCompactDateFormat() noexcept {
        return GetLocaleCompactDateFormatW();
    }
#else
    inline auto GetLocaleCompactDateFormat() noexcept {
        return GetLocaleCompactDateFormatA();
    }
#endif

    //**************************************************************************
    // StringFrom...
    //**************************************************************************

    //--------------------------------------------------------------------------
    // StringFromTime
    //--------------------------------------------------------------------------
    inline auto StringFromTime(_Out_writes_z_(cchBuffer) LPSTR szBuffer,
                               _In_ INT cchBuffer,
                               _In_z_ _Printf_format_string_ LPCSTR szFormat,
                               _In_ const std::tm* when) noexcept {
        const auto cchWritten = std::strftime(szBuffer, cchBuffer,
                                              szFormat,
                                              when);
        WinAPIAssert(cchWritten);
        return cchWritten;
    }

    //--------------------------------------------------------------------------

    inline auto StringFromTime(_Out_writes_z_(cchBuffer) LPWSTR szBuffer,
                               _In_ INT cchBuffer,
                               _In_z_ _Printf_format_string_ LPCWSTR szFormat,
                               _In_ const std::tm* when) noexcept {
        const auto cchWritten = std::wcsftime(szBuffer, cchBuffer,
                                              szFormat,
                                              when);
        WinAPIAssert(cchWritten);
        return cchWritten;
    }

    //--------------------------------------------------------------------------

    template <typename CharT>
    inline decltype(auto) StringFromTime(_Out_writes_z_(cchBuffer) FAR CharT* szBuffer,
                                         _In_ INT cchBuffer,
                                         _In_z_ _Printf_format_string_ FAR const CharT* szFormat,
                                         _In_ const std::time_t* when) noexcept {
        return StringFromTime(szBuffer, cchBuffer,
                              szFormat,
                              std::localtime(when));
    }

    //--------------------------------------------------------------------------
    // StringFromTimeNow
    //--------------------------------------------------------------------------
    template <typename CharT>
    inline decltype(auto) StringFromTimeNow(_Out_writes_z_(cchBuffer) FAR CharT* szBuffer,
                                            _In_ INT cchBuffer,
                                            _In_z_ _Printf_format_string_ FAR const CharT* szFormat) noexcept {
        std::time_t now; std::time(&now);
        return StringFromTime(szBuffer, cchBuffer,
                              szFormat, &now);
    }

    //--------------------------------------------------------------------------
    // StringFromDate
    //--------------------------------------------------------------------------
    inline decltype(auto) StringFromDate(_Out_writes_z_(cchBuffer) LPSTR szBuffer,
                                         _In_ INT cchBuffer,
                                         _In_ const std::tm* when) noexcept {
        static const auto s_LocaleDateFormat = GetLocaleCompactDateFormatA();
        return StringFromTime(szBuffer, cchBuffer,
                              s_LocaleDateFormat, when);
    }

    //--------------------------------------------------------------------------

    inline decltype(auto) StringFromDate(_Out_writes_z_(cchBuffer) LPWSTR szBuffer,
                                         _In_ INT cchBuffer,
                                         _In_ const std::tm* when) noexcept {
        static const auto s_LocaleDateFormat = GetLocaleCompactDateFormatW();
        return StringFromTime(szBuffer, cchBuffer,
                              s_LocaleDateFormat,
                              when);
    }

    //--------------------------------------------------------------------------

    template <typename CharT>
    inline decltype(auto) StringFromDate(_Out_writes_z_(cchBuffer) FAR CharT* szBuffer,
                                         _In_ INT cchBuffer,
                                         _In_ const std::time_t* when) noexcept {
        return StringFromDate(szBuffer, cchBuffer,
                              std::localtime(when));
    }

    //--------------------------------------------------------------------------
    // StringFromDateNow
    //--------------------------------------------------------------------------
    template <typename CharT>
    inline decltype(auto) StringFromDateNow(_Out_writes_z_(cchBuffer) FAR CharT* szBuffer,
                                            _In_ INT cchBuffer) noexcept {
        std::time_t now; std::time(&now);
        return StringFromDate(szBuffer, cchBuffer, &now);
    }

    //**************************************************************************
    // FormatStringFromSeconds
    //**************************************************************************
    inline void FormatStringFromSeconds(_Out_ CString& str,
                                        _In_ const CString& fmt,
                                        _In_ FLOAT fSeconds,
                                        _In_ FLOAT fMaxSeconds) {
        if (fMaxSeconds > 3600.f) {
            const auto fHours = std::floor(fSeconds / 3600.f);
            fSeconds -= fHours * 3600.f;
            const auto fMinutes = std::floor(fSeconds / 60.f);
            fSeconds -= fMinutes * 60.f;
            const auto nHours = static_cast<int>(fHours);
            const auto nMinutes = static_cast<int>(fMinutes);
            const auto nSeconds = static_cast<int>(fSeconds);
            str.Format(fmt, nHours, nMinutes, nSeconds);
        } else {
            const auto fMinutes = std::floor(fSeconds / 60.f);
            fSeconds -= fMinutes * 60.f;
            const auto nMinutes = static_cast<int>(fMinutes);
            const auto nSeconds = static_cast<int>(fSeconds);
            str.Format(fmt, nMinutes, nSeconds);
        }
    }

    //**************************************************************************
    // CalcTimeStringMetrics
    //**************************************************************************
    inline void CalcTimeStringMetrics(_In_ HDC hDC,
                                      _In_ HFONT hFont,
                                      _In_ float fSeconds,
                                      _Out_ LONG& cch,
                                      _Out_ LONG& dxWidth) {
        assert(hDC);
        CString strFormat;
        if (fSeconds > 3600.f) {
            const int hours = static_cast<int>(std::floor(fSeconds / 3600.f));
            strFormat.Format(TEXT("%d:00:00"), hours);
        } else {
            const int minutes = static_cast<int>(std::floor(fSeconds / 60.f));
            strFormat.Format(TEXT("%d:00"), minutes);
        }
        LONG dxHeight = 0;
        Windows::GDI::MeasureText(hDC, hFont,
                                  strFormat.GetString(), /*cchText*/ -1,
                                  DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE,
                                  &dxWidth, &dxHeight);
        cch = strFormat.GetLength();
    }

    //**************************************************************************
    // MeasureDateStringWidth
    //**************************************************************************
    inline auto MeasureDateStringWidth(_In_ HDC hDC) noexcept {
        assert(hDC);

        // Set all possible digits in all locations
        // and measure each, take the widest as the
        // maximum length.
        TCHAR szDateStr[] = TEXT("00/00/00");
        int dxDateStrWidth = 0;
        for (char i = 0; i <= 9; ++i) {
            SIZE size{ 0,0 };
            Windows::GDI::MeasureText(hDC,
                                      szDateStr, /*cchText*/ -1,
                                      DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE,
                                      size);
            if (size.cx > dxDateStrWidth) { dxDateStrWidth = size.cx; }
            ++szDateStr[0]; ++szDateStr[1];
            // "/"
            ++szDateStr[3]; ++szDateStr[4];
            // "/"
            ++szDateStr[6]; ++szDateStr[7];
        }
        return dxDateStrWidth;
    }

    //**************************************************************************
    // MeasureTimeStringWidth
    //**************************************************************************
    inline auto MeasureTimeStringWidth(_In_ HDC hDC) noexcept {
        assert(hDC);

        // Set all possible digits in all locations
        // and measure each, take the widest as the
        // maximum length.
        TCHAR szTimeStr[] = TEXT("00:00");
        int dxTimeStrWidth = 0;
        for (char i = 0; i <= 9; ++i) {
            SIZE size{ 0,0 };
            Windows::GDI::MeasureText(hDC,
                                      szTimeStr, /*cchText*/ -1,
                                      DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE,
                                      size);
            if (size.cx > dxTimeStrWidth) { dxTimeStrWidth = size.cx; }
            ++szTimeStr[0]; ++szTimeStr[1];
            // ":"
            ++szTimeStr[3]; ++szTimeStr[4];
        }
        return dxTimeStrWidth;
    }

    //**************************************************************************
    // MeasurePercentStringWidth
    //**************************************************************************
    inline auto MeasurePercentStringWidth(_In_ HDC hDC) noexcept {
        assert(hDC);

        // Set all possible digits in all locations
        // and measure each, take the widest as the
        // maximum length.
        TCHAR szPercentStr[] = TEXT("000");
        int dxPercentStrWidth = 0;
        for (char i = 0; i <= 9; ++i) {
            SIZE size{ 0,0 };
            Windows::GDI::MeasureText(hDC,
                                      (LPCTSTR)szPercentStr, /*cchText*/ -1,
                                      DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE,
                                      size);
            if (size.cx > dxPercentStrWidth) { dxPercentStrWidth = size.cx; }
            ++szPercentStr[0]; ++szPercentStr[1];  ++szPercentStr[2];
        }
        return dxPercentStrWidth;
    }
} // namespace Windows::GDI

#endif // GUID_4B24EC2A_01C9_4728_ABF6_88D38A3405D9