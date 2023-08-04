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
#include "Windows/GDI/GDI_Font.h"
//--------------------------------------
//
//--------------------------------------
//
#include "Windows/Windows_StringUtil.h"
//--------------------------------------

//--------------------------------------
//
#include <cmath>
//--------------------------------------

namespace {
    //**************************************************************************
    // GetNonClientMetrics
    //**************************************************************************
    inline BOOL GetNonClientMetrics(_Out_ LPNONCLIENTMETRICS lpMetrics) noexcept {
        assert(lpMetrics); if (!lpMetrics) { return FALSE; }

        ZeroMemory(lpMetrics, sizeof(NONCLIENTMETRICS));
        lpMetrics->cbSize = sizeof(NONCLIENTMETRICS);

        const auto res = ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
                                                sizeof(NONCLIENTMETRICS),
                                                lpMetrics, 0);
        WinAPIAssert(res);
        return res;
    }
} // namespace <anonymous>

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // MeasureText
    //**************************************************************************
    BOOL MeasureText(_In_ HDC hDC,
                     _When_(cchText == -1, _In_z_)
                     _When_(cchText != -1, _In_reads_(cchText))
                     LPCTSTR lpchText,
                     _In_ INT cchText,
                     _In_ UINT uFormat,
                     _Inout_ LPLONG lpWidth,
                     _Inout_ LPLONG lpHeight) noexcept {
        assert(hDC);      if (!hDC)      { return FALSE; }
        assert(lpchText); if (!lpchText) { return FALSE; }
        assert(lpWidth);  if (!lpWidth)  { return FALSE; }
        assert(lpHeight); if (!lpHeight) { return FALSE; }

        Windows::GDI::ScopedSetMapMode _mapMode{ hDC, MM_TEXT };

        RECT rectSize = { 0, 0, (*lpWidth), (*lpHeight) };
        const auto iHeight = ::DrawText(hDC,
                                        lpchText, cchText,
                                        &rectSize,
                                        uFormat | DT_CALCRECT);
        WinAPIAssert(iHeight);
        if (iHeight != 0) {
            if (uFormat & DT_SINGLELINE) {
                (*lpHeight) = iHeight;
            } else {
                (*lpHeight) = rectSize.bottom - rectSize.top;
            }
            (*lpWidth) = rectSize.right - rectSize.left;
            return TRUE;
        }
        return FALSE;
    }

    /**************************************************************************
     *                                                                        *
     **************************************************************************/
    BOOL GetSystemFont(_In_ INT iType,
                       _At_(logFont.lpParam, _Out_)
                       Windows::GDI::detail::LogFontParam logFont) noexcept {
        assert(logFont); if (!logFont) { return FALSE; }
        NONCLIENTMETRICS ncm;
        if (!::GetNonClientMetrics(&ncm)) { return FALSE; }
        switch (iType) {
        case Windows::GDI::SYS_FONT_CAPTION:      *logFont = ncm.lfCaptionFont;   return TRUE;
        case Windows::GDI::SYS_FONT_CAPTIONSMALL: *logFont = ncm.lfSmCaptionFont; return TRUE;
        case Windows::GDI::SYS_FONT_MENU:         *logFont = ncm.lfMenuFont;      return TRUE;
        case Windows::GDI::SYS_FONT_STATUS:       *logFont = ncm.lfStatusFont;    return TRUE;
        case Windows::GDI::SYS_FONT_MESSAGE:      *logFont = ncm.lfMessageFont;   return TRUE;
        default: return FALSE;
        }
    }

    //------------------------------------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    BOOL GetSystemFont(_In_ INT iType,
                       _In_ INT iHeight,
                       _At_(logFont.lpParam, _Out_)
                       Windows::GDI::detail::LogFontParam logFont) noexcept {
        assert(logFont); if (!logFont) { return FALSE; }
        assert(iHeight); if (!iHeight) { return FALSE; }

        LOGFONT lf;
        ZeroMemory(&lf, sizeof(LOGFONT));

        if (!Windows::GDI::GetSystemFont(iType, lf)) { return FALSE; }

        lf.lfHeight = -iHeight;

        CFont font{};
        WinAPIVerify(font.CreateFontIndirect(&lf));
        if (font.IsNull()) { return FALSE; }
        const auto res = font.GetLogFont(lf);
        WinAPIAssert(res != 0);
        if (res == 0) { return FALSE; }

        if (lf.lfHeight == -iHeight) {
            *logFont = lf;
            return TRUE;
        } else {
            return FALSE;
        }
    }

    //------------------------------------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    BOOL GetFontExact(_In_z_ LPCTSTR szFamily,
                      _In_ INT iHeight,
                      _In_ INT iWeight,
                      _In_ INT iFlags,
                      _At_(logFont.lpParam, _Out_)
                      Windows::GDI::detail::LogFontParam logFont) noexcept {
        assert(logFont); if (!logFont) { return FALSE; }
        assert(iHeight); if (!iHeight) { return FALSE; }

        const auto    bItalic = (iFlags & Windows::GDI::FONT_FLAG_ITALIC)    ? TRUE : FALSE;
        const auto bUnderline = (iFlags & Windows::GDI::FONT_FLAG_UNDERLINE) ? TRUE : FALSE;
        const auto bStrikeout = (iFlags & Windows::GDI::FONT_FLAG_STRIKEOUT) ? TRUE : FALSE;

        auto quality = DEFAULT_QUALITY;
        if (iFlags & Windows::GDI::FONT_FLAG_ANTIALIAS) {
            quality = ANTIALIASED_QUALITY;
        }
        if (iFlags & Windows::GDI::FONT_FLAG_CLEARTYPE) {
            assert(quality == DEFAULT_QUALITY);
            quality = CLEARTYPE_QUALITY;
        }

        CFont font{};
        {
            LOGFONT lf;
            ZeroMemory(&lf, sizeof(LOGFONT));
            lf.lfHeight        = iHeight;
            lf.lfWeight        = iWeight;
            lf.lfCharSet       = DEFAULT_CHARSET;
            lf.lfOutPrecision  = OUT_CHARACTER_PRECIS; // OUT_TT_PRECIS;
            lf.lfClipPrecision = CLIP_CHARACTER_PRECIS; // CLIP_DEFAULT_PRECIS;
            if (iFlags & Windows::GDI::FONT_FLAG_ANTIALIAS) {
                lf.lfQuality = ANTIALIASED_QUALITY;
            }
            if (iFlags & Windows::GDI::FONT_FLAG_CLEARTYPE) {
                assert(lf.lfQuality == DEFAULT_QUALITY);
                lf.lfQuality = CLEARTYPE_QUALITY;
            }
            lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            if (bItalic) { lf.lfItalic = TRUE; }
            if (bUnderline) { lf.lfUnderline = TRUE; }
            if (bStrikeout) { lf.lfStrikeOut = TRUE; }
            Windows::StringCopy(lf.lfFaceName, szFamily, LF_FACESIZE);
            font.CreateFontIndirect(&lf);
        }

        WinAPIAssert(!font.IsNull()); if (font.IsNull()) { return FALSE; }

        LOGFONT lf{};
        ZeroMemory(&lf, sizeof(LOGFONT));
        const auto res = font.GetLogFont(lf);
        WinAPIAssert(res != 0);
        if (res == 0) { return FALSE; }

        *logFont = lf;

        if (lf.lfHeight != std::abs(iHeight)) { return FALSE; }
        if (lf.lfWeight != iWeight) { return FALSE; }
        if ((lf.lfItalic ? TRUE : FALSE) != bItalic) { return FALSE; }
        if ((lf.lfUnderline ? TRUE : FALSE) != bUnderline) { return FALSE; }
        if ((lf.lfStrikeOut ? TRUE : FALSE) != bStrikeout) { return FALSE; }
        if ((lf.lfQuality & quality) == 0) { return FALSE; }
        if (Windows::StringCompare(lf.lfFaceName,
                                   szFamily,
                                   LF_FACESIZE) != 0) {
            return FALSE;
        }

        return TRUE;
    }

    //------------------------------------------------------------------------------------------------------

    INT GetFontClose(_In_z_ LPCTSTR szFamily,
                     _In_ INT iHeight,
                     _In_ INT iWeight,
                     _In_ INT iFlags,
                     _At_(logFont.lpParam, _Out_) Windows::GDI::detail::LogFontParam logFont) noexcept {
        assert(logFont); if (!logFont) { return Windows::GDI::FONT_MATCH_ERROR; }
        assert(iHeight); if (!iHeight) { return Windows::GDI::FONT_MATCH_ERROR; }

        if (Windows::GDI::GetFontExact(szFamily,
                                       iHeight, iWeight,
                                       iFlags,
                                       logFont)) {
            return Windows::GDI::FONT_MATCH_EXACT;
        }

        if (logFont->lfHeight != iHeight) {
            return Windows::GDI::FONT_MATCH_NONE;
        }

        if (Windows::StringCompare(logFont->lfFaceName,
                                   szFamily,
                                   LF_FACESIZE) != 0) {
            return Windows::GDI::FONT_MATCH_CLOSE;
        } else {
            return Windows::GDI::FONT_MATCH_HEIGHT;
        }
    }
}
