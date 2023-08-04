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
#include "GDI_TextFragment.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI_Font.h"
//--------------------------------------

//==============================================================================

namespace  {
    const CString NonEmptyString{ TEXT(" ") };
} // namespace <anonymous>

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // CTextFragment
    //**************************************************************************
    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CTextFragment::Draw(_In_ int iWidth,
                             _In_ int iHeight) noexcept {
        if (m_strFragment.IsEmpty()) { return; }

        HFONT oldFont = NULL;
        if (OwnFont()) {
            oldFont = Context().SelectFont(m_Font);
        }
        COLORREF oldColor = RGB(255, 255, 255);
        if (OwnColor()) {
            oldColor = Context().SetTextColor(m_Color);
        }
        COLORREF oldClearColor = RGB(0, 0, 0);
        if (OwnClearColor()) {
            oldClearColor = Context().SetBkColor(m_Clear.Color());
        }

        if (m_eTrimMode == TrimMode::None) {
            // TextOut is faster, but lacks the clipping
            // and positional functionality of DrawText
            WinAPIVerify(Context().TextOut(0, 0,
                                           m_strFragment.GetString(),
                                           m_strFragment.GetLength()));
        } else {
            UINT uFormatFlags = DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE;
            switch (m_eTrimMode) {
                case Text::TrimMode::None:		   uFormatFlags |= DT_NOCLIP;		 break;
                case Text::TrimMode::Char:		   uFormatFlags |= 0;				 break;
                case Text::TrimMode::CharEllipsis: uFormatFlags |= DT_END_ELLIPSIS;  break;
                case Text::TrimMode::Word:         uFormatFlags |= DT_WORDBREAK;     break;
                case Text::TrimMode::WordEllipsis: uFormatFlags |= DT_WORD_ELLIPSIS; break;
                case Text::TrimMode::Middle:       uFormatFlags |= DT_PATH_ELLIPSIS; break;
                DEFAULT_UNREACHABLE;
            }

            RECT bounds = { 0, 0, iWidth, iHeight };
            WinAPIVerify(Context().DrawText(m_strFragment.GetString(),
                                            m_strFragment.GetLength(),
                                            &bounds,
                                            uFormatFlags));
        }

        if (OwnClearColor()) { Context().SetTextColor(oldClearColor); }
        if (OwnColor()) { Context().SetTextColor(oldColor); }
        if (OwnFont()) { Context().SelectFont(oldFont); }
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CTextFragment::Clear(_In_ int /*iWidth*/,
                              _In_ int /*iHeight*/) noexcept {
        if (m_strFragment.IsEmpty()) { return; }

        ScopedSelectPen _pen{ m_DC, m_Clear.Pen()};
        ScopedSelectBrush _brush{ m_DC, m_Clear.Brush() };
        ScopedSetDCPenColor _penColor{ m_DC, m_Clear.Color() };
        ScopedSetDCBrushColor _brushColor{ m_DC, m_Clear.Color() };

        WinAPIVerify(m_DC.RoundRect(m_Clear.RectPtr(),
                                    m_Clear.Curve()));
    }

    //--------------------------------------------------------------------------

    void CTextFragment::OnChanged(_In_ GDIChange eWhat,
                                  _In_ LPARAM lparam,
                                  _In_ WPARAM wparam) noexcept {
        switch (eWhat) {
            case GDIChange::Color: {
                const auto color = (COLORREF)wparam;
                switch ((Layer)lparam) {
                    case Layer::Background: {
                        if (! OwnClearColor()) { m_Clear.Color(color); }
                        break;
                    }
                    case Layer::Foreground: {
                        if (!OwnColor()) { m_Color = color; }
                        break;
                    }
                    DEFAULT_UNREACHABLE;
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CTextFragment::SetRenderMode(_In_ RenderMode eRenderMode) noexcept {
        if (!OwnFont()) { return; }
        if (m_eRenderMode == eRenderMode) { return; }

        CLogFont currentLogFont;
        WinAPIVerify(m_Font.GetLogFont(&currentLogFont));

        CLogFont newLogFont{ currentLogFont };
        switch (eRenderMode) {
            case Text::RenderMode::Default:     newLogFont.lfQuality = DEFAULT_QUALITY;     break;
            case Text::RenderMode::Precise:     newLogFont.lfQuality = DRAFT_QUALITY;       break;
            case Text::RenderMode::ClearType:   newLogFont.lfQuality = CLEARTYPE_QUALITY;   break;
            case Text::RenderMode::AntiAliased: newLogFont.lfQuality = ANTIALIASED_QUALITY; break;
            DEFAULT_UNREACHABLE;
        }

        CFontHandle font{ ::CreateFontIndirect(&newLogFont) };
        if (font) {
            AttachFont(font);
            UpdateMetrics();
        }
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CTextFragment::UpdateMetrics() noexcept {
        assert(m_DC); assert(m_Font);

        ScopedSelectFont _font{ m_DC, m_Font };

        TEXTMETRIC metric;
        ZeroMemory(&metric, sizeof(TEXTMETRIC));
        WinAPIVerify(m_DC.GetTextMetrics(&metric));

        m_iAscent = metric.tmAscent;
        m_iCharWidth = metric.tmAveCharWidth;
        const auto halfAveCharWidth = metric.tmAveCharWidth / 2;

        // NOTE: DT_VCENTER causes measurements to be
        //       very different and not useful
        MeasureText(Context(),
                    m_strFragment.IsEmpty() ? NonEmptyString : m_strFragment,
                    DT_CENTER | DT_NOPREFIX | DT_NOCLIP | DT_SINGLELINE,
                    m_Size);
        if (m_strFragment.IsEmpty()) { m_Size.cx = 0; }
        m_Clear.Rect(-halfAveCharWidth,            0,
                     m_Size.cx + halfAveCharWidth, m_Size.cy);
        m_Clear.Curve(m_Size.cy / 2, m_Size.cy / 2);
    }
} // namespace Windows::GDI
