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
#include "GDI_ProgressBar.h"
#include "GDI_ProgressBar_Util.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI_Util.h"
#include "Windows/GDI/GDI_Font.h"
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // CProgressBar
    //**************************************************************************
    CProgressBar::CProgressBar(_In_ HDC hDC)
        : baseClass{ hDC } {
        CLogFont logFont;
        GetSystemFont(SYS_FONT_MESSAGE, logFont);
        SetFont(logFont);
    }

    //--------------------------------------------------------------------------

    void CProgressBar::Update(_In_ float fProgress) {
        if (m_Progress.Current(fProgress)) {
            UpdateProgress();
        }
    }

    //--------------------------------------------------------------------------

    void CProgressBar::Range(_In_ float fMin,
                             _In_ float fMax) {
        if (m_Progress.Range(fMin, fMax)) {
            UpdateBounds();
        }
    }

    //--------------------------------------------------------------------------

    void CProgressBar::SetTextMode(_In_ ProgressText eLeft,
                                   _In_ ProgressText eRight) {
        if ((m_Text[TextLeft].m_eMode != eLeft) ||
            (m_Text[TextRight].m_eMode != eRight)) {
            m_Text[TextLeft].m_eMode = eLeft;
            m_Text[TextRight].m_eMode = eRight;
            UpdateBounds();
        }
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    bool CProgressBar::SetFont(_In_ const LOGFONT& logFont) {
        CFont newFont = ::CreateFontIndirect(&logFont);
        ATLASSERT(newFont);
        if (!newFont) { return false; }

        {
            ScopedSelectFont _font{ Context(), newFont };
            ZeroMemory(&m_Font.Metrics, sizeof(m_Font.Metrics));
            if (Context().GetTextMetrics(&m_Font.Metrics) == FALSE) { return false; }

            m_Font.Metrics.tmDateStringWidth = MeasureDateStringWidth(Context());
            m_Font.Metrics.tmTimeStringWidth = MeasureTimeStringWidth(Context());
            m_Font.Metrics.tmPercentStringWidth = MeasurePercentStringWidth(Context());

            UpdateBounds();
        }

        m_Font.font = newFont.Detach();
        ATLASSERT(m_Font.font);

        return !m_Font.font.IsNull();
    }

    //--------------------------------------------------------------------------

    bool CProgressBar::SetFont(_In_z_ LPCTSTR szFamily,
                               _In_ int iHeight) {
        CLogFont logFont{};
        if (GetFontClose(szFamily, -iHeight, &logFont) == FONT_MATCH_NONE) {
            return false;
        }
        return SetFont(logFont);
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void CProgressBar::OnDraw(float /*fInterp*/) noexcept {
        if (m_bClear) {
            ScopedSelectPen _pen{ Context(), Pen(Layer::Background) };
            ScopedSelectBrush _brush{ Context(), Brush(Layer::Background) };
            ScopedSetDCPenColor _penColor{ Context(), Color(Layer::Background) };
            const auto size = Size();
            WinAPIVerify(Context().Rectangle(0, 0, size.cx, size.cy));
        }

        ScopedSelectPen _pen{ Context(), Pen(Layer::Foreground) };
        ScopedSetDCPenColor _penColor{ Context(), Color(Layer::Foreground) };

        // Draw Progress
        {
            // Outline
            WinAPIVerify(Context().FrameRect(m_OutlineRect, Brush(Layer::Foreground)));
            // Fill
            WinAPIVerify(Context().FillRect(m_FillRect, Brush(Layer::Foreground)));
        }

        // Draw Text
        {
            ScopedSetMapMode _mapMode{ Context(), MM_TEXT };
            ScopedSelectFont _font{ Context(), m_Font.font };

            if (m_Text[TextLeft].m_eMode != ProgressText::None) {
                m_Text[TextLeft].Draw(Context(), DT_LEFT);
            }

            if (m_Text[TextRight].m_eMode != ProgressText::None) {
                m_Text[TextRight].Draw(Context(), DT_RIGHT);
            }
        }
    }

    //--------------------------------------------------------------------------

    void CProgressBar::OnChanged(_In_ GDIChange eWhat,
                                 _In_ LPARAM lparam,
                                 _In_ WPARAM /*wparam*/) {
        switch (eWhat) {
        case GDIChange::Size: { //Size has been changed so change font to match
            CLogFont logFont;
            const auto cy = (LONG)lparam;
            if (m_Font.font) {
                WinAPIVerify(m_Font.font.GetLogFont(logFont));
                logFont.lfHeight = -cy;
            } else {
                GetSystemFont(SYS_FONT_MESSAGE, -cy, &logFont);
            }
            SetFont(logFont);
            break;
        }
        }
    }

    //--------------------------------------------------------------------------

    int CProgressBar::CalcTextSize(ProgressText eMode) {
        switch (eMode) {
        case ProgressText::None:
            return 0;
        case ProgressText::ElapsedPercent:
            [[fallthrough]];
        case ProgressText::RemainingPercent:
            return m_Font.Metrics.tmPercentStringWidth;
        case ProgressText::SystemDate:
            return m_Font.Metrics.tmDateStringWidth;
        case ProgressText::SystemTime:
            return m_Font.Metrics.tmTimeStringWidth;
        default: {
            LONG cchTimeMax = 0;
            LONG dxWidth = 0;
            CalcTimeStringMetrics(Context(),
                                  m_Font.font,
                                  m_Progress.Total(),
                                  cchTimeMax,
                                  dxWidth);
            if (cchTimeMax > 6) {
                m_TimeFormat.Format(TEXT("%%0%dd:%%02d:%%02d"),
                                    cchTimeMax - 6);
            } else if (m_Progress.Total() > 3600.f) {
                m_TimeFormat = TEXT("%d:%02d:%02d");
            } else if (cchTimeMax > 4) {
                m_TimeFormat = TEXT("%02d:%02d");
            } else {
                m_TimeFormat = TEXT("%d:%02d");
            }
            return dxWidth;
        }
        }
    }

    //--------------------------------------------------------------------------

    void CProgressBar::UpdateBounds() {
        const auto size = Size();
        const auto fHeight = static_cast<float>(size.cy);
        const auto iMarginX = m_Font.Metrics.tmAveCharWidth / 2;
        const auto fMarginY = fHeight * 0.25f;

        m_OutlineRect.left = iMarginX;
        m_OutlineRect.top = static_cast<LONG>(std::ceil(fMarginY));
        m_OutlineRect.right = size.cx - iMarginX;
        m_OutlineRect.bottom = static_cast<LONG>(std::floor(fHeight - fMarginY));

        {
            auto& text = m_Text[TextLeft];
            text.m_Bounds.top = 0;
            text.m_Bounds.bottom = size.cy;
            text.m_Bounds.left = iMarginX;
            const auto dxTextSize = CalcTextSize(text.m_eMode);
            text.m_Bounds.right = text.m_Bounds.left + dxTextSize;
        }

        {
            auto& text = m_Text[TextRight];
            text.m_Bounds.top = 0;
            text.m_Bounds.bottom = size.cy;
            text.m_Bounds.right = size.cx - iMarginX;
            const auto dxTextSize = CalcTextSize(text.m_eMode);
            text.m_Bounds.left = text.m_Bounds.right - dxTextSize;
        }

        const auto outlineLeft = m_Text[TextLeft].m_Bounds.right + iMarginX;
        const auto outlineRight = m_Text[TextRight].m_Bounds.left - iMarginX;

        if (outlineLeft < outlineRight) {
            m_OutlineRect.left = outlineLeft;
            m_OutlineRect.right = outlineRight;
        }

        m_FillRect = m_OutlineRect;
        UpdateProgress();
    }

    //--------------------------------------------------------------------------

    void CProgressBar::UpdateProgress() {
        if (m_Progress.Start() == m_Progress.Finish()) { return; }

        const auto x0 = m_OutlineRect.left;
        const auto x1 = m_OutlineRect.right;
        const auto dx = std::round(static_cast<float>(x1 - x0) * m_Progress.ElapsedFraction());
        m_FillRect.right = x0 + static_cast<LONG>(dx);
        assert(m_FillRect.right >= m_OutlineRect.left);
        assert(m_FillRect.right <= m_OutlineRect.right);

        for (auto i = 0; i < TextCount; ++i) {
            switch (m_Text[i].m_eMode) {
                case ProgressText::ElapsedPercent: {
                    const auto fPercent = m_Progress.ElapsedPercent();
                    const auto nPercent = static_cast<int>(std::round(fPercent));
                    assert(nPercent >= 0 && nPercent <= 100);
                    m_Text[i].m_strText.Format(TEXT("%3d"), nPercent);
                    break;
                }
                case ProgressText::RemainingPercent: {
                    const auto fPercent = m_Progress.RemainPercent();
                    const auto nPercent = static_cast<int>(std::round(fPercent));
                    assert(nPercent >= 0 && nPercent <= 100);
                    m_Text[i].m_strText.Format(TEXT("%3d"), nPercent);
                    break;
                }
                case ProgressText::ElapsedTime:
                    FormatStringFromSeconds(m_Text[i].m_strText,
                                            m_TimeFormat,
                                            m_Progress.Elapsed(),
                                            m_Progress.Total());
                    break;
                case ProgressText::RemainingTime:
                    FormatStringFromSeconds(m_Text[i].m_strText,
                                            m_TimeFormat,
                                            m_Progress.Remain(),
                                            m_Progress.Total());
                    break;
                case ProgressText::TotalTime:
                    FormatStringFromSeconds(m_Text[i].m_strText,
                                            m_TimeFormat,
                                            m_Progress.Total(),
                                            m_Progress.Total());
                    break;
                case ProgressText::SystemTime: {
                    static constexpr const auto s_cchTimeBuf = 32;
                    StringFromTimeNow(m_Text[i].m_strText.GetBuffer(s_cchTimeBuf),
                                      s_cchTimeBuf - 1,
                                      TEXT("%H:%M"));
                    break;
                }
                case ProgressText::SystemDate: {
                    static constexpr const auto s_cchDateBuf = 32;
                    StringFromDateNow(m_Text[i].m_strText.GetBuffer(s_cchDateBuf),
                                      s_cchDateBuf - 1);
                    break;
                }
            }
        }
    }
} // namespace Windows::GDI
