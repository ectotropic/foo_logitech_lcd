#pragma once
#ifndef GUID_52BAD302_26BC_406E_A99E_4AC389B9DD21
#define GUID_52BAD302_26BC_406E_A99E_4AC389B9DD21
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
#include "Windows/GDI/GDI_Util.h"
#include "Windows/GDI/GDI_Font.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/SequentialEnum.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // GDIChange
    //**************************************************************************
    enum class GDIChange {
        None = 0,
        Position,
        Size,
        Clip,
        Color,
        Brush,
        Pen,
        PenWidth
    };

    //**************************************************************************
    // Layer
    //**************************************************************************
    enum class Layer {
        Foreground = 0,
        Background,
        _count
    };

    //**************************************************************************
    // CDrawable
    //**************************************************************************
    class CDrawable {
    private:
        class CLayer {
        private:
            CLayer(const CLayer&)            = delete; // No Copy
            CLayer& operator=(const CLayer&) = delete; // No Copy

        public:
            CLayer() = default;

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            CLayer(_In_ CLayer&& other) throw() :
                m_Color   { std::exchange(other.m_Color, RGB(255,255,255)) },
                m_Brush   { other.m_Brush.Detach() },
                m_Pen     { other.m_Pen.Detach() },
                m_PenWidth{ std::exchange(other.m_PenWidth, 1) } {}

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            CLayer& operator=(_In_ CLayer&& other) throw() {
                m_Color    = std::exchange(other.m_Color,
                                           RGB(255, 255, 255));
                m_Brush    = other.m_Brush.Detach();
                m_Pen      = other.m_Pen.Detach();
                m_PenWidth = std::exchange(other.m_PenWidth, 1);
                return *this;
            }

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            bool Color(_In_ COLORREF color) throw() {
                if (m_Color == color) { return false; }
                m_Color = color;
                m_Brush.Attach(::CreateSolidBrush(color));
                m_Pen.Attach(::CreatePen(PS_SOLID, m_PenWidth, color));
                return true;
            }

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            bool Brush(_In_ HBRUSH hBrush) throw() {
                if (m_Brush == hBrush) { return false; }
                m_Brush.Attach(hBrush);
                return true;
            }

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            bool Pen(_In_ HPEN hPen) throw() {
                if (m_Pen == hPen) { return false; }
                m_Pen.Attach(hPen);
                return true;
            }

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            bool PenWidth(_In_ int nWidth) throw() {
                if (m_PenWidth == nWidth) { return false; }
                m_PenWidth = nWidth;
                m_Pen.Attach(::CreatePen(PS_SOLID, nWidth, m_Color));
                return true;
            }

            constexpr auto Color   () const throw() { return m_Color; }
            constexpr auto Brush   () const throw() { return m_Brush.m_hBrush; }
            constexpr auto Pen     () const throw() { return m_Pen.m_hPen; }
            constexpr auto PenWidth() const throw() { return m_PenWidth; }

        private:
            COLORREF m_Color   { RGB(255,255,255) };
            CBrush   m_Brush   { ::CreateSolidBrush(RGB(255,255,255)) };
            CPen     m_Pen     { ::CreatePen(PS_SOLID, m_PenWidth, RGB(255,255,255)) };
            int      m_PenWidth{ 1 };
        }; // class CLayer

    public:
        enum class StockBrush {
            White     = WHITE_BRUSH,
            LightGray = LTGRAY_BRUSH,
            Gray      = GRAY_BRUSH,
            DarkGray  = DKGRAY_BRUSH,
            Black     = BLACK_BRUSH,
            Null      = NULL_BRUSH,
            Hollow    = HOLLOW_BRUSH,
            _count    = Hollow - White,
        };

        static const auto GetStockBrush(_In_ StockBrush eType) throw() {
            static HBRUSH s_StockObject[static_cast<int>(StockBrush::_count)] = { 0 };
            const auto iType = static_cast<int>(eType);
            if (s_StockObject[iType] == NULL) {
                s_StockObject[iType] = static_cast<HBRUSH>(::GetStockObject(iType));
            }
            return s_StockObject[iType];
        }

        enum class StockPen {
            White  = WHITE_PEN,
            Black  = BLACK_PEN,
            Null   = NULL_PEN,
            _count = Null - White,
        };

        static const auto GetStockPen(_In_ StockPen eType) throw() {
            static HPEN s_StockObject[static_cast<int>(StockPen::_count)] = { 0 };
            const auto iType = static_cast<int>(eType);
            if (s_StockObject[iType] == NULL) {
                s_StockObject[iType] = static_cast<HPEN>(::GetStockObject(iType));
            }
            return s_StockObject[iType];
        }

        enum class StockFont {
            OEMFixedWidth     = OEM_FIXED_FONT,
            ANSIFixedWidth    = ANSI_FIXED_FONT,
            ANSIVariableWidth = ANSI_VAR_FONT,
            System            = SYSTEM_FONT,
            DeviceDefault     = DEVICE_DEFAULT_FONT,
            SystemFixedWidth  = SYSTEM_FIXED_FONT,
            _count            = SystemFixedWidth - OEMFixedWidth,
        };

        static const auto GetStockFont(_In_ StockFont eType) throw() {
            static HFONT s_StockObject[static_cast<int>(StockFont::_count)] = { 0 };
            const auto iType = static_cast<int>(eType);
            if (s_StockObject[iType] == NULL) {
                s_StockObject[iType] = static_cast<HFONT>(::GetStockObject(iType));
            }
            return s_StockObject[iType];
        }

    protected:
        CDrawable(_In_ HDC hDC) throw() :
            m_DC{ hDC } {}

        ~CDrawable() = default;

        virtual void OnDraw(_In_ float fInterp = 0.f) = 0;

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void OnChanged(_In_ GDIChange /*eWhat*/,
                               _In_ LPARAM /*lparam*/,
                               _In_ WPARAM /*wparam*/) {}

        virtual void OnDebugDraw() throw();

    public:
        static void Flush() throw() { WinAPIVerify(::GdiFlush()); }

        void Clear() throw();

        void Draw     (_In_ float fInterp = 0.f);
        void DebugDraw(_In_ float fInterp = 0.f);

        void PushViewportOrigin(_In_ int x,
                                _In_ int y);
        void PushViewportOriginOffset(_In_ int x,
                                      _In_ int y);
        void PopViewportOrigin();

        void PushViewportSize(_In_ int cx,
                              _In_ int cy);
        void PopViewportSize();


        void PushViewportOrigin(_In_ POINT origin) {
            PushViewportOrigin(origin.x, origin.y);
        };
        void PushViewportOriginOffset(_In_ POINT offset) {
            PushViewportOriginOffset(offset.x, offset.y);
        };
        void PushViewportOrigin() {
            PushViewportOrigin(m_Position);
        }

        void PushViewportSize(_In_ SIZE size) {
            PushViewportSize(size.cx, size.cy);
        };
        void PushViewportSize() {
            PushViewportSize(m_Size);
        }

        void PushViewport() {
            PushViewportSize();
            PushViewportOrigin();
        }
        void PopViewport() {
            PopViewportSize();
            PopViewportOrigin();
        }

        void PushClip();
        void  PopClip();

        void SetPosition(_In_ POINT pos);
        void SetPosition(_In_ INT nPosX,
                         _In_ INT nPosY) {
            const CPoint pt{ nPosX, nPosY };
            SetPosition(pt);
        }

        void SetSize(_In_ SIZE size);
        void SetSize(_In_ INT nWidth,
                     _In_ INT nHeight) {
            const CSize sz{ nWidth, nHeight };
            SetSize(sz);
        }

        void SetClip(_In_ const RECT& rect);
        void SetClip(_In_ INT nPosX1,
                     _In_ INT nPosY1,
                     _In_ INT nPosX2,
                     _In_ INT nPosY2) {
            const CRect rt{ nPosX1, nPosY1, nPosX2, nPosY2 };
            SetClip(rt);
        }

        constexpr void EnableClipping(BOOL bClip) throw() { m_bUseClipping = bClip; }

    public:
        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        BOOL DrawRect(_In_ INT iLeft, _In_ INT iTop,
                      _In_ INT iRight, _In_ INT iBottom) throw() {
            static HBRUSH hNullBrush = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
            ScopedSelectBrush brush{ m_DC, hNullBrush };
            return m_DC.Rectangle(iLeft, iTop, iRight, iBottom);
        }

        BOOL DrawRect(_In_ const RECT& rect) throw() {
            return DrawRect(rect.left, rect.top,
                            rect.right, rect.bottom);
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        int DrawTextLine(_In_ LPCTSTR lpstrText,
                         _In_ int cchText,
                         _At_(rect.lpParam, _Inout_) detail::RectParam rect,
                         _In_ UINT uFormat = 0) throw() {
            return m_DC.DrawText(lpstrText, cchText, rect, uFormat | DT_SINGLELINE);
        }

        int DrawTextLine(_In_ const CSimpleString& strText,
                         _At_(rect.lpParam, _Inout_) detail::RectParam rect,
                         _In_ UINT uFormat = 0) throw() {
            return DrawTextLine(strText.GetString(),
                                strText.GetLength(),
                                rect,
                                uFormat);
        }

    public:
        auto CreateDefaultRgn() const throw() {
            return ::CreateRectRgn(m_Position.x,
                                   m_Position.y,
                                   m_Position.x + m_Size.cx,
                                   m_Position.y + m_Size.cy);
        }

    public: // Accessors
        CDCHandle Context() throw() { return CDCHandle{ m_DC.m_hDC }; }

        constexpr HRGN  ClipRegion() const throw() { return m_Region.m_hRgn; }
        constexpr POINT Position  () const throw() { return m_Position; }
        constexpr SIZE  Size      () const throw() { return m_Size; }
        constexpr RECT  Clip      () const throw() { return m_Clip; }

        auto Color   (_In_ Layer eLayer) const throw() { return m_Layer[static_cast<int>(eLayer)].Color(); }
        auto Brush   (_In_ Layer eLayer) const throw() { return m_Layer[static_cast<int>(eLayer)].Brush(); }
        auto Pen     (_In_ Layer eLayer) const throw() { return m_Layer[static_cast<int>(eLayer)].Pen(); }
        auto PenWidth(_In_ Layer eLayer) const throw() { return m_Layer[static_cast<int>(eLayer)].PenWidth(); }

        void Color   (_In_ Layer eLayer,
                      _In_ COLORREF color);
        void Brush   (_In_ Layer eLayer,
                      _In_ HBRUSH hBrush);
        void Pen     (_In_ Layer eLayer,
                      _In_ HPEN hPen);
        void PenWidth(_In_ Layer eLayer,
                      _In_ int nWidth);

    private: //Data
        CDCHandle m_DC{};

        CRgn   m_Region{};
        CLayer m_Layer[static_cast<int>(Layer::_count)];

        POINT m_Position   { 0, 0 };
        SIZE  m_Size       { 0 };
        RECT  m_Clip       { 0, 0, 0, 0 };
        BOOL m_bUseClipping{ TRUE };

    private:
        template <class T>
        static auto SimpleArrayPop(CSimpleArray<T>& array) {
            const auto last = array.GetSize() - 1;
            auto value = array[last];
            array.RemoveAt(last);
            return value;
        }

        CSimpleArray<POINT> m_ViewportOriginStack;
        CSimpleArray<SIZE>  m_ViewportSizeStack;
        CSimpleArray<CRgn>  m_ClipStack;
    }; // class CDrawable
} // namespace Windows::GDI

#endif //GUID_52BAD302_26BC_406E_A99E_4AC389B9DD21