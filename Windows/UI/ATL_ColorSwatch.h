#pragma once
#ifndef GUID_2FD4DBAF_639C_46DE_BEF3_D5CA9990135D
#define GUID_2FD4DBAF_639C_46DE_BEF3_D5CA9990135D
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
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/UI/Comm_Dialog.h"
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // CColorSwatchStatic
    //**************************************************************************
    class CColorSwatchStatic {
    public:
        CColorSwatchStatic() = default;

        CColorSwatchStatic(_In_ HWND hWndStatic,
                           _In_ COLORREF color) throw() {
            Initialise(hWndStatic, color);
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        BOOL Initialise(_In_ HWND hWndStatic,
                        _In_ COLORREF color) throw() {
            m_Color = color;

            ATLASSERT(::IsWindow(hWndStatic));
            m_Static.Detach();
            m_Static.Attach(hWndStatic);
            ATLASSERT(m_Static.IsWindow());

            if (!m_Brush.IsNull()) { m_Brush.DeleteObject(); }
            m_Brush.CreateSolidBrush(m_Color);
            WinAPIAssert(!m_Brush.IsNull());
            if (m_Brush.IsNull()) { return FALSE; }

            const auto res = m_Static.InvalidateRect(NULL, TRUE);
            WinAPIAssert(res);
            return res;
        }

        BOOL Invalidate(_In_ BOOL bErase = TRUE) throw() {
            ATLASSERT(m_Static.IsWindow());
            return m_Static.InvalidateRect(NULL, bErase);
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        BOOL SelectColor(_In_ COLORREF newColor) throw() {
            ATLASSERT(m_Static.IsWindow());
            if (newColor != m_Color) {
                m_Color = newColor;
                if (!m_Brush.IsNull()) {
                    m_Brush.DeleteObject();
                }
                return m_Brush.CreateSolidBrush(m_Color) &&
                       m_Static.InvalidateRect(NULL, TRUE);
            }
            return TRUE;
        }

        BOOL SelectColor(_In_ BYTE red,
                          _In_ BYTE green,
                          _In_ BYTE blue) throw() {
            return SelectColor(RGB(red, green, blue));
        }

        BOOL SelectColor() throw() {
            ATLASSERT(m_Static.IsWindow());
            COLORREF newColor{ m_Color };
            const auto status = CommDialog::ChooseColor(m_Static, &newColor);
            return (status == CDERR_SUCCESS) && SelectColor(newColor);
        }

        void Attach(_In_opt_ HWND hWnd) throw() { m_Static.Attach(hWnd); }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void Attach(_In_opt_ HBRUSH hBrush) throw() { m_Brush.Attach(hBrush); }

        void Attach(_In_opt_ HWND hWnd,
                    _In_opt_ HBRUSH hBrush) throw() {
            Attach(hWnd);
            Attach(hBrush);
        }

        void Detach() throw() {
            m_Static.Detach();
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            m_Brush.Detach();
        }

        LRESULT OnColorStatic(_In_ UINT /*uMsg*/,
                              _In_ WPARAM /*wParam*/,
                              _In_ LPARAM lParam,
                              _Inout_ BOOL& bHandled) noexcept {
            const HWND hWndDialogCtrl = reinterpret_cast<HWND>(lParam);
            if (hWndDialogCtrl != m_Static) { return 0; }
            bHandled = TRUE;
            return reinterpret_cast<LRESULT>(m_Brush.m_hBrush);
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        bool operator==(_In_opt_ HBRUSH hBrush) throw() { return hBrush == m_Brush; }
        bool operator==(_In_ LPARAM lParam) throw() { return reinterpret_cast<HWND>(lParam) == m_Static; }
        bool operator==(_In_opt_ HWND hWnd) throw() { return hWnd == m_Static; }

        operator bool    () throw() { return m_Static.m_hWnd && m_Brush.m_hBrush; }
        operator COLORREF() throw() { return m_Color; }
        operator HWND    () throw() { return m_Static.m_hWnd; }
        operator HBRUSH  () throw() { return m_Brush.m_hBrush; }
        operator LRESULT () throw() { return reinterpret_cast<LRESULT>(m_Brush.m_hBrush); }

        auto GetColor () const throw() { return m_Color; }
        auto GetStatic() const throw() { return m_Static; }
        auto GetBrush () const throw() { return m_Brush; }

    public: // Data
        CStatic  m_Static{};
        CBrush   m_Brush {};
        COLORREF m_Color { RGB(0, 0, 0) };
    }; // class CColorSwatchStatic
} // namespace Windows::UI

#endif // GUID_2FD4DBAF_639C_46DE_BEF3_D5CA9990135D