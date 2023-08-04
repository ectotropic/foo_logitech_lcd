#pragma once
#ifndef GUID_1EB620FB_9E26_4B4D_B62C_1EBD607DB983
#define GUID_1EB620FB_9E26_4B4D_B62C_1EBD607DB983
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
//  Include GDI Core
#include "Windows/GDI/GDI.h"
//--------------------------------------

//--------------------------------------
//
#include "GDI_Drawable.h"
//--------------------------------------

namespace Windows::GDI::Text {
    //**************************************************************************
    // TrimMode
    //**************************************************************************
    enum class TrimMode {
        None = 0,
        Char,
        CharEllipsis,
        Word,
        WordEllipsis,
        Middle
    };

    //**************************************************************************
    // RenderMode
    //**************************************************************************
    enum class RenderMode {
        Default = 0,
        Precise,
        ClearType,
        AntiAliased
    };
}; // namespace Windows::GDI::Text

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // CTextFragment
    //**************************************************************************
    class CTextFragment {
    private:
        //----------------------------------------------------------------------
        // CClear
        //----------------------------------------------------------------------
        class CClear {
        public:
            CClear(_In_ COLORREF color) noexcept :
                m_Color{ color },
                m_Pen{ ::CreatePen(PS_SOLID, 1, color) },
                m_Brush{ ::CreateSolidBrush(color) } {}


            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            void Color(_In_ COLORREF color) noexcept {
                if (m_Color != color) {
                    m_Color = color;
                    m_Pen = ::CreatePen(PS_SOLID, 1, m_Color);
                    WinAPIAssert(m_Pen.m_hPen);
                    m_Brush = ::CreateSolidBrush(m_Color);
                    WinAPIAssert(m_Brush.m_hBrush);
                }
            }

            constexpr auto Color() const noexcept { return m_Color; }
            constexpr auto Pen  () const noexcept { return m_Pen.m_hPen; }
            constexpr auto Brush() const noexcept { return m_Brush.m_hBrush; }

            constexpr void Rect(_In_ RECT rect) noexcept { m_Rect = rect; }
            constexpr void Rect(_In_ LONG left,
                                _In_ LONG top,
                                _In_ LONG right,
                                _In_ LONG bottom) noexcept {
                Rect({ left, top, right, bottom });
            }
            constexpr void Curve(_In_ POINT curve) noexcept { m_Curve = curve; }
            constexpr void Curve(_In_ LONG x,
                                 _In_ LONG y) noexcept {
                Curve({ x, y });
            }

            constexpr auto    Rect() const noexcept { return m_Rect; }
            constexpr auto RectPtr() const noexcept { return &m_Rect; }
            constexpr auto   Curve() const noexcept { return m_Curve; }

        private:
            COLORREF m_Color{ RGB(0, 0, 0) };
            CPen     m_Pen{ NULL };
            CBrush   m_Brush{ NULL };

            RECT  m_Rect{ 0, 0, 0, 0 };
            POINT m_Curve{ 0, 0 };
        }; // class CClear

        //----------------------------------------------------------------------

    private:
        CTextFragment() = delete;
        CTextFragment(CTextFragment&) = delete;
        CTextFragment& operator=(CTextFragment&) = delete;

    public:
        using TrimMode   = Text::TrimMode;
        using RenderMode = Text::RenderMode;

        struct UseOwn final {
            enum {
                Font = 1 << 0,
                Color = 1 << 1,
                ClearColor = 1 << 2,
            };
        }; // struct UseOwn final

    public:
        CTextFragment(CTextFragment&&) = default;
        CTextFragment& operator=(CTextFragment&&) = default;

        CTextFragment(_In_ HDC hDC,
                      _In_opt_ HFONT hFont = NULL) noexcept :
            m_DC(hDC),
            m_Font(hFont),
            m_Color{ ::GetTextColor(hDC) },
            m_Clear{ ::GetBkColor(hDC) } {
            assert(hDC);
        }

        ~CTextFragment() noexcept {
            DetachFont();
        }

        void Draw(_In_ int iWidth,
                  _In_ int iHeight) noexcept;
        void Clear(_In_ int iWidth,
                   _In_ int iHeight) noexcept;

        void OnChanged(_In_ GDIChange eWhat,
                       _In_ LPARAM lparam,
                       _In_ WPARAM wparam) noexcept;

        void SetText(_In_ const CSimpleString& str,
                     _In_ HFONT hFont,
                     _In_ COLORREF color,
                     _In_ COLORREF clearColor,
                     _In_ DWORD dwFlags) {
            m_strFragment = str;
            AttachFont(hFont);
            m_Color = color;
            m_Clear.Color(clearColor);
            m_dwFlags = dwFlags;
            UpdateMetrics();
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void DefaultFont(_In_ const HFONT hFont) noexcept {
            if (m_Font != hFont) {
                AttachFont(hFont);
                UpdateMetrics();
            }
        }

        void DefaultFont(_At_(logFont.lpParam, _In_) detail::LogFontParam logFont) noexcept {
            const auto hFont = ::CreateFontIndirect(logFont);
            WinAPIAssert(hFont);
            if (hFont) { DefaultFont(hFont); }
        }

        void SetRenderMode(_In_ RenderMode eRenderMode) noexcept;
        constexpr auto GetRenderMode() const noexcept { return m_eRenderMode; }

        auto CharCount() const noexcept { return m_strFragment.GetLength(); }

        constexpr SIZE Size() const noexcept { return m_Size; }

        constexpr auto Ascent   () const noexcept { return m_iAscent; }
        constexpr auto CharWidth() const noexcept { return m_iCharWidth; }

        constexpr void SetTrimMode(_In_ TrimMode eTrimMode) noexcept { m_eTrimMode = eTrimMode; }
        constexpr auto GetTrimMode() const noexcept { return m_eTrimMode; }

        constexpr bool OwnFont      () const noexcept { return m_dwFlags & UseOwn::Font; }
        constexpr bool OwnColor     () const noexcept { return m_dwFlags & UseOwn::Color; }
        constexpr bool OwnClearColor() const noexcept { return m_dwFlags & UseOwn::ClearColor; }

        auto Context() const noexcept { return m_DC; }
        auto Context()       noexcept { return m_DC; }

        auto Font() const noexcept { return m_Font; }
        auto Font()       noexcept { return m_Font; }

    private:
        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void AttachFont(_In_ HFONT hFont) noexcept {
            DetachFont();
            m_Font = hFont;
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void DetachFont() noexcept {
            if (!m_Font) { return; }
            if (OwnFont()) {
                WinAPIVerify(m_Font.DeleteObject());
            } else {
                m_Font.Detach();
            }
        }

        void UpdateMetrics() noexcept;

    private:
        CDCHandle m_DC{};

        DWORD       m_dwFlags{ 0 };
        CFontHandle m_Font{};
        COLORREF    m_Color{ RGB(255, 255, 255) };
        CClear      m_Clear;

        SIZE m_Size{ 0, 0 };
        int  m_iAscent{ 0 };
        int  m_iCharWidth{ 0 };

        Text::TrimMode   m_eTrimMode  { TrimMode::None };
        Text::RenderMode m_eRenderMode{ RenderMode::Default };

        CString m_strFragment{};
    }; // class CTextFragment
} // namespace Windows::GDI

#endif // GUID_1EB620FB_9E26_4B4D_B62C_1EBD607DB983