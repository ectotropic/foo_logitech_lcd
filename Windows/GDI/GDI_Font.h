#pragma once
#ifndef GUID_23B767ED_1090_4738_BA0D_DDD6E05E6BD8
#define GUID_23B767ED_1090_4738_BA0D_DDD6E05E6BD8
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
                     _Inout_ LPLONG lpHeight) noexcept;

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _When_(cchText == -1, _In_z_)
                            _When_(cchText != -1, _In_reads_(cchText))
                            LPCTSTR lpchText,
                            _In_ INT cchText,
                            _In_ UINT uFormat,
                            _At_(size.lpParam, _Inout_)
                            Windows::GDI::detail::SizeParam size) noexcept {
        return Windows::GDI::MeasureText(hDC,
                                         lpchText, cchText,
                                         uFormat,
                                         &(size->cx), &(size->cy));
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ HFONT hFont,
                            _When_(cchText == -1, _In_z_)
                            _When_(cchText != -1, _In_reads_(cchText))
                            LPCTSTR lpchText,
                            _In_ INT cchText,
                            _In_ UINT uFormat,
                            _Inout_ LPLONG lpWidth,
                            _Inout_ LPLONG lpHeight) noexcept {
        assert(hDC);   if (!hDC)   { return FALSE; }
        assert(hFont); if (!hFont) { return FALSE; }
        Windows::GDI::ScopedSelectFont _font{ hDC, hFont };
        return Windows::GDI::MeasureText(hDC,
                                         lpchText, cchText,
                                         uFormat,
                                         lpWidth, lpHeight);
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ HFONT hFont,
                            _When_(cchText == -1, _In_z_)
                            _When_(cchText != -1, _In_reads_(cchText))
                            LPCTSTR lpchText,
                            _In_ INT cchText,
                            _In_ UINT uFormat,
                            _At_(size.lpParam, _Inout_)
                            Windows::GDI::detail::SizeParam size) noexcept {
        return Windows::GDI::MeasureText(hDC,
                                         hFont,
                                         lpchText, cchText,
                                         uFormat,
                                         &(size->cx), &(size->cy));
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ const CSimpleString& strString,
                            _In_ UINT uFormat,
                            _Inout_ LPLONG lpWidth,
                            _Inout_ LPLONG lpHeight) noexcept {
        return Windows::GDI::MeasureText(hDC,
                                         strString.GetString(),
                                         strString.GetLength(),
                                         uFormat,
                                         lpWidth, lpHeight);
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ const CSimpleString& strString,
                            _In_ UINT uFormat,
                            _At_(size.lpParam, _Inout_)
                            Windows::GDI::detail::SizeParam size) noexcept {
        return Windows::GDI::MeasureText(hDC,
                                         strString,
                                         uFormat,
                                         &(size->cx), &(size->cy));
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ HFONT hFont,
                            _In_ const CSimpleString& strString,
                            _In_ UINT uFormat,
                            _Inout_ LPLONG lpWidth,
                            _Inout_ LPLONG lpHeight) noexcept {
        return Windows::GDI::MeasureText(hDC, hFont,
                                         strString.GetString(),
                                         strString.GetLength(),
                                         uFormat,
                                         lpWidth, lpHeight);
    }

    //------------------------------------------------------

    inline auto MeasureText(_In_ HDC hDC,
                            _In_ HFONT hFont,
                            _In_ const CSimpleString& strString,
                            _In_ UINT uFormat,
                            _At_(size.lpParam, _Inout_)
                            Windows::GDI::detail::SizeParam size) noexcept {
        return Windows::GDI::MeasureText(hDC, hFont,
                                         strString,
                                         uFormat,
                                         &(size->cx), &(size->cy));
    }

    //**************************************************************************
    // SYS_FONT
    //**************************************************************************
    enum SYS_FONT {
        SYS_FONT_CAPTION = 0,
        SYS_FONT_CAPTIONSMALL,
        SYS_FONT_MENU,
        SYS_FONT_STATUS,
        SYS_FONT_MESSAGE,
    };

    //------------------------------------------------------

    BOOL GetSystemFont(_In_ INT iType,
                       _At_(logFont.lpParam, _Out_)
                       Windows::GDI::detail::LogFontParam logFont) noexcept;

    //------------------------------------------------------

    BOOL GetSystemFont(_In_ INT iType,
                       _In_ INT iHeight,
                       _At_(logFont.lpParam, _Out_)
                       Windows::GDI::detail::LogFontParam logFont) noexcept;

    //**************************************************************************
    // FONT_FLAG
    //**************************************************************************
    enum FONT_FLAG {
        FONT_FLAG_ITALIC    = 1 << 0,
        FONT_FLAG_UNDERLINE = 1 << 1,
        FONT_FLAG_STRIKEOUT = 1 << 2,
        // -----
        FONT_FLAG_ANTIALIAS = 1 << 3,
        FONT_FLAG_CLEARTYPE = 1 << 4,
    };
    DEFINE_ENUM_FLAG_OPERATORS(FONT_FLAG);

    //------------------------------------------------------

    BOOL GetFontExact(_In_z_ LPCTSTR szFamily,
                      _In_ INT iHeight,
                      _In_ INT iWeight,
                      _In_ INT iFlags,
                      _At_(logFont.lpParam, _Out_)
                      Windows::GDI::detail::LogFontParam logFont) noexcept;

    //------------------------------------------------------

    inline auto GetFontExact(_In_z_ LPCTSTR szFamily,
                             _In_ INT iHeight,
                             _In_ INT iWeight,
                             _At_(logFont.lpParam, _Out_)
                             Windows::GDI::detail::LogFontParam logFont) noexcept {
        return Windows::GDI::GetFontExact(szFamily,
                                          iHeight, iWeight,
                                          0,
                                          logFont);
    }

    //------------------------------------------------------

    inline auto GetFontExact(_In_z_ LPCTSTR szFamily,
                             _In_ INT iHeight,
                             _At_(logFont.lpParam, _Out_)
                             Windows::GDI::detail::LogFontParam logFont) noexcept {
        return Windows::GDI::GetFontExact(szFamily,
                                          iHeight, FW_DONTCARE,
                                          logFont);
    }

    //**************************************************************************
    // FONT_MATCH
    //**************************************************************************
    enum FONT_MATCH {
        FONT_MATCH_NONE = 0,
        FONT_MATCH_EXACT,
        FONT_MATCH_CLOSE,
        FONT_MATCH_HEIGHT,

        FONT_MATCH_ERROR = 0xFFFF,
    };

    //------------------------------------------------------

    INT GetFontClose(_In_z_ LPCTSTR szFamily,
                     _In_ INT iHeight,
                     _In_ INT iWeight,
                     _In_ INT iFlags,
                     _At_(logFont.lpParam, _Out_)
                     Windows::GDI::detail::LogFontParam logFont) noexcept;

    //------------------------------------------------------

    inline auto GetFontClose(_In_z_ LPCTSTR szFamily,
                             _In_ INT iHeight,
                             _In_ INT iWeight,
                             _At_(logFont.lpParam, _Out_)
                             Windows::GDI::detail::LogFontParam logFont) noexcept {
        return Windows::GDI::GetFontClose(szFamily,
                                          iHeight, iWeight,
                                          0,
                                          logFont);
    }

    //------------------------------------------------------

    inline auto GetFontClose(_In_z_ LPCTSTR szFamily,
                             _In_ INT iHeight,
                             _At_(logFont.lpParam, _Out_)
                             Windows::GDI::detail::LogFontParam logFont) noexcept {
        return Windows::GDI::GetFontClose(szFamily,
                                          iHeight, FW_DONTCARE,
                                          0,
                                          logFont);
    }
} // namespace Windows::GDI

#endif // GUID_23B767ED_1090_4738_BA0D_DDD6E05E6BD8