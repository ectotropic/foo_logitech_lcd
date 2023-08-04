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
#include "CommonHeaders.h"
#include "GDI_Drawable.h"
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // CDrawable
    //**************************************************************************

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CDrawable::Clear() throw() {
        if (m_Region) {
            m_DC.FillRgn(m_Region, Brush(Layer::Background));
        } else {
            CRgn region{ CreateDefaultRgn() };
            m_DC.FillRgn(region, Brush(Layer::Background));
        }
    }

    //------------------------------------------------------

    void CDrawable::Color(_In_ Layer eLayer,
                          _In_ COLORREF color) {
        if (m_Layer[static_cast<int>(eLayer)].Color(color)) {
            OnChanged(GDIChange::Color,
                      (LPARAM)(eLayer),
                      (WPARAM)(color));
        }
    }

    //------------------------------------------------------

    void CDrawable::Brush(_In_ Layer eLayer,
                          _In_ HBRUSH hBrush) {
        if (m_Layer[static_cast<int>(eLayer)].Brush(hBrush)) {
            OnChanged(GDIChange::Brush,
                      (LPARAM)(eLayer),
                      (WPARAM)(hBrush));
        }
    }

    //------------------------------------------------------

    void CDrawable::Pen(_In_ Layer eLayer,
                        _In_ HPEN hPen) {
        if (m_Layer[static_cast<int>(eLayer)].Pen(hPen)) {
            OnChanged(GDIChange::Pen,
                      (LPARAM)(eLayer),
                      (WPARAM)(hPen));
        }
    }

    //------------------------------------------------------

    void CDrawable::PenWidth(_In_ Layer eLayer,
                             _In_ int nWidth) {
        if (m_Layer[static_cast<int>(eLayer)].PenWidth(nWidth)) {
            OnChanged(GDIChange::PenWidth,
                      (LPARAM)(eLayer),
                      (WPARAM)(nWidth));
        }
    }

    //------------------------------------------------------

    void CDrawable::SetPosition(_In_ POINT pos) {
        if (m_Position.x != pos.x || m_Position.y != pos.y) {
            m_Position = pos;
            OnChanged(GDIChange::Position,
                      (LPARAM)pos.x, (WPARAM)pos.y);
        }
    }

    //------------------------------------------------------

    void CDrawable::SetSize(_In_ SIZE size) {
        if (m_Size.cx != size.cx || m_Size.cy != size.cy) {
            m_Size = size;
            OnChanged(GDIChange::Size,
                      (LPARAM)size.cx, (WPARAM)size.cy);
        }
    }

    //------------------------------------------------------

    void CDrawable::SetClip(_In_ const RECT& rect) {
        if (m_Clip.left  != rect.left  || m_Clip.top    != rect.top ||
            m_Clip.right != rect.right || m_Clip.bottom != rect.bottom) {
            m_Clip = rect;
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            m_Region = ::CreateRectRgn(m_Clip.left, m_Clip.top,
                                       m_Clip.right, m_Clip.bottom);
            OnChanged(GDIChange::Clip,
                      (LPARAM)&m_Clip,
                      (WPARAM)m_Region.m_hRgn);
        }
    }

    //------------------------------------------------------

    void CDrawable::Draw(_In_ float fInterp) {
        ScopedSetBkMode       _bkMode{ m_DC, TRANSPARENT };
        ScopedSetBkColor      _bkColor{ m_DC, Color(Layer::Background) };
        ScopedSetTextColor    _textColor{ m_DC, Color(Layer::Foreground) };
        ScopedSetDCBrushColor _brushColor{ m_DC, Color(Layer::Foreground) };
        ScopedSetDCPenColor   _penColor{ m_DC, Color(Layer::Foreground) };
        ScopedSelectPen       _pen{ m_DC, Pen(Layer::Foreground) };

        PushViewport();
        PushClip();
        OnDraw(fInterp);
        PopClip();
        PopViewport();
    }

    //------------------------------------------------------

    void CDrawable::DebugDraw(_In_ float /*fInterp*/) {
        static CPen dbgPen{};
        if (!dbgPen) { WinAPIVerify(dbgPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255))); }

        ScopedSetBkMode       _bkMode{ m_DC, TRANSPARENT };
        ScopedSetBkColor      _bkColor{ m_DC, RGB(0, 0, 0) };
        ScopedSetTextColor    _textColor{ m_DC, RGB(255, 255, 255) };
        ScopedSetDCBrushColor _brushColor{ m_DC, RGB(255, 255, 255) };
        ScopedSetDCPenColor   _penColor{ m_DC, RGB(255, 255, 255) };
        ScopedSelectPen       _pen{ m_DC, dbgPen };

        PushViewport();
        OnDebugDraw();
        PopViewport();
    }

    //------------------------------------------------------

    void CDrawable::PushViewportOrigin(_In_ int x,
                                       _In_ int y) {
        CPoint oldOrigin{ 0,0 };
        WinAPIVerify(m_DC.SetViewportOrg(x, y, &oldOrigin));
        m_ViewportOriginStack.Add(oldOrigin);
    }

    //------------------------------------------------------

    void CDrawable::PopViewportOrigin() {
        if (m_ViewportOriginStack.GetSize() > 0) {
            const auto pos = SimpleArrayPop(m_ViewportOriginStack);
            WinAPIVerify(m_DC.SetViewportOrg(pos));
        }
    }

    //------------------------------------------------------

    void CDrawable::PushViewportOriginOffset(_In_ int x,
                                             _In_ int y) {
        CPoint oldOffset{ 0,0 };
        WinAPIVerify(m_DC.OffsetViewportOrg(x, y, &oldOffset));
        m_ViewportOriginStack.Add(oldOffset);
    }

    //------------------------------------------------------

    void CDrawable::PushViewportSize(_In_ int cx,
                                     _In_ int cy) {
        CSize oldSize = { 0,0 };
        WinAPIVerify(m_DC.SetViewportExt(cx, cy, &oldSize));
        m_ViewportSizeStack.Add(oldSize);
    }

    //------------------------------------------------------

    void CDrawable::PopViewportSize() {
        if (m_ViewportSizeStack.GetSize() > 0) {
            const auto size = SimpleArrayPop(m_ViewportSizeStack);
            WinAPIVerify(m_DC.SetViewportExt(size));
        }
    }

    //------------------------------------------------------

    void CDrawable::PushClip() {
        CRgn region{};
        m_DC.GetClipRgn(region);
        m_DC.SelectClipRgn(m_bUseClipping ? region : NULL);
        m_ClipStack.Add(region);
    }

    //------------------------------------------------------

    void CDrawable::PopClip() {
        if (m_ClipStack.GetSize() > 0) {
            const auto region = SimpleArrayPop(m_ClipStack);
            m_DC.SelectClipRgn(region);
        } else {
            m_DC.SelectClipRgn(NULL);
        }
    }

    //------------------------------------------------------

    void CDrawable::OnDebugDraw() throw() {
        DrawRect(m_Position.x,
                 m_Position.y,
                 m_Position.x + m_Size.cx - 1,
                 m_Position.y + m_Size.cy - 1);
        DrawRect(m_Clip);
    }
} // namespace Windows::GDI