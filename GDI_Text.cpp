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
#include "GDI_Text.h"
//--------------------------------------

//--------------------------------------
//
#include <cwctype>
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // CText
    //**************************************************************************
    CText::CText(_In_ HDC hDC) : baseClass(hDC) {
        CLogFont logFont;
        WinAPIVerify(GetSystemFont(SYS_FONT_MESSAGE, &logFont));
        WinAPIVerify(m_Font.CreateFontIndirect(&logFont));
    }

    //--------------------------------------------------------------------------

    void CText::SetFont(_In_ const LOGFONT& logFont, _In_ bool bAllowOverlap) {
        if (m_Font) { m_Font.DeleteObject(); }
        WinAPIVerify(m_Font.CreateFontIndirect(&logFont));
        m_bAllowOverlap = bAllowOverlap;

        int iWidth = 0, iHeight = 0;
        for (auto& line: m_Lines) {
            line.SetFont(m_Font);
            if (line.Size().cx > iWidth) { iWidth = line.Size().cx; }
            iHeight += GetLineHeight(line);
        }
        Update(iWidth, iHeight);
    }

    //--------------------------------------------------------------------------

    void CText::SetText(_In_reads_z_(nLength) LPCTSTR szText, _In_ int nLength,
                        _In_ int nMaxLines) {
        CString              text{ szText, nLength };
        std::vector<CString> TextLines;

        int index{ 0 };
        while (text.GetLength() > 0) {
            index = text.FindOneOf(TEXT("\r\n"));
            if (index != -1) {
                TextLines.push_back(std::move(text.Left(index)));
                if (text[index] == TEXT('\r')) {
                    const auto new_index{ index + 1 };
                    if ((text.GetLength() > new_index) &&
                        text[new_index] == TEXT('\n')) {
                        index = new_index;
                    }
                }
                text = text.Mid(index + 1);
            } else {
                TextLines.push_back(std::move(text));
                break;
            }
        }

        auto lineCount = TextLines.size();
        if (nMaxLines > 0) {
            lineCount =
              std::min(static_cast<std::size_t>(nMaxLines), lineCount);
        }
        SetLineCount(lineCount);

        lineCount = std::min(TextLines.size(), m_Lines.size());

        int iWidth  = 0;
        int iHeight = 0;
        for (decltype(lineCount) i = 0; i < lineCount; ++i) {
            auto& strLine = TextLines[i];
            auto& line    = m_Lines[i];
            line.SetText(strLine.Trim());
            if (line.Size().cx > iWidth) { iWidth = line.Size().cx; }
            iHeight += GetLineHeight(line);
        }

        Update(iWidth, iHeight);
    }

    //--------------------------------------------------------------------------

    void CText::SetHAlign(_In_ Text::Align::Horizontal eAlign) noexcept {
        if (m_eHAlign == eAlign) { return; }
        m_eHAlign = eAlign;
        for (auto& line: m_Lines) { line.SetHAlign(m_eHAlign); }
    }

    //--------------------------------------------------------------------------

    void CText::SetVScroll(_In_ const ScrollParams& params) noexcept {
        m_Scroller.SetMode(params.eMode);
        m_Scroller.SetSpeed(params.fSpeed);
        m_Scroller.SetDelay(params.fDelay);
        if ((params.eMode == ScrollMode::Loop) && (params.fGapFactor > 0.f)) {
            m_fLoopGapFactor      = params.fGapFactor;
            const auto fHeight    = static_cast<float>(Size().cy);
            const auto fLineCount = static_cast<float>(m_Lines.size());
            m_fLoopGap = std::round(m_fLoopGapFactor * (fHeight / fLineCount));
        } else {
            m_fLoopGapFactor = m_fLoopGap = 0.f;
        }
    }

    //--------------------------------------------------------------------------

    void CText::SetHScroll(_In_ const ScrollParams& params) noexcept {
        m_ScrollH = params;

        for (auto& line: m_Lines) {
            line.SetHScroll(m_ScrollH.eMode, m_ScrollH.fSpeed, m_ScrollH.fDelay,
                            m_ScrollH.fGapFactor);
        }
    }

    //--------------------------------------------------------------------------

    void CText::SetBGClear(bool bClear) noexcept {
        if (m_bBGClear == bClear) { return; }
        m_bBGClear = bClear;
        for (auto& line: m_Lines) { line.SetClear(m_bBGClear); }
    }

    //--------------------------------------------------------------------------

    void CText::Reset() noexcept {
        m_Lines.clear();
        m_Scroller.Reset();

        m_fTarget           = 0.f;
        m_iScrollLineID     = 0;
        m_eScrollLastStatus = ScrollStatus::None;
    }

    //--------------------------------------------------------------------------

    void CText::Update() {
        const auto iClipHeight = Clip().bottom - Clip().top;
        m_Scroller.Update(static_cast<float>(Size().cy) + m_fLoopGap,
                          static_cast<float>(iClipHeight));
        const float fClipTop    = static_cast<float>(Clip().top);
        const float fClipBottom = static_cast<float>(Clip().bottom);

        int iScrollingLineCount = 0;
        for (auto& line: m_Lines) {
            const auto iLineHeight = GetLineHeight(line);

            CPoint lineLimits[2] = {
                { 0, 0 },
                { line.Size().cx, iLineHeight },
            };
            WinAPIVerify(Context().LPtoDP(lineLimits, 2));

            const auto fLineHeight = static_cast<float>(iLineHeight);
            const auto fLineStart  = static_cast<float>(lineLimits[0].y);
            const auto fLineEnd    = static_cast<float>(lineLimits[1].y);

            // BOOL RectVisible( [in] HDC hdc, [in] const RECT* lprect );
            bool bVisible      = true;
            bool bShouldScroll = false;
            if (fLineStart > 0) {
                bVisible = fLineStart < fClipBottom;
                bShouldScroll =
                  (!m_bScrollStaggered) ||
                  ((m_iScrollLineID >= 0) &&
                   ((fClipBottom - fLineStart) > (0.75f * fLineHeight)));
            } else {
                bVisible = fLineEnd > fClipTop;
                bShouldScroll =
                  (!m_bScrollStaggered) ||
                  ((m_iScrollLineID >= 0) &&
                   ((fLineEnd - fClipTop) > (0.75f * fLineHeight)));
            }

            const auto eScrollStatus = line.Update(bVisible && bShouldScroll);
            if (bVisible) {
                if (eScrollStatus == ScrollStatus::Finished) {
                    if (m_Scroller.IsScrolling() && m_bScrollStaggered) {
                        line.PauseScroll();
                    }
                } else if ((eScrollStatus != ScrollStatus::None) &&
                           (eScrollStatus != ScrollStatus::Paused)) {
                    ++iScrollingLineCount;
                }
            }
        }

        const auto iHeight = Size().cy;
        const bool bScrolling =
          m_Scroller.IsScrolling() && (iHeight > iClipHeight);
        if (bScrolling && m_bScrollStaggered) {
            if (iScrollingLineCount == 0) {
                m_Scroller.Resume(true);
            } else {
                m_Scroller.Pause();
            }
        }
    }

    //--------------------------------------------------------------------------

    void CText::OnDraw(float fInterp) {
        if (m_Lines.empty()) { return; }

        ScopedSetBkColor   _bkColor{ Context(), Color(Layer::Background) };
        ScopedSelectFont   _font{ Context(), m_Font };
        ScopedSetMapMode   _mapMode{ Context(), MM_TEXT };
        ScopedSetTextColor _textColor{ Context(), Color(Layer::Foreground) };

        const auto iClipHeight = Clip().bottom - Clip().top;
        const auto iHeight     = Size().cy;
        const auto fHeight     = static_cast<float>(iHeight);
        float      fOffsetV    = 0.f;

        const bool bScrolling =
          m_Scroller.IsScrolling() && (iHeight > iClipHeight);
        if (bScrolling) {
            fOffsetV              = m_Scroller.GetOffset(fInterp);
            const auto fMaxOffset = fHeight + m_fLoopGap;
            if (std::fabs(fOffsetV) > fMaxOffset) {
                fOffsetV = std::copysign(fMaxOffset, fOffsetV);
            }
        } else {
            const auto fClipHeight = static_cast<float>(iClipHeight);
            switch (m_eVAlign) {
            case Text::Align::Horizontal::Left: fOffsetV = 0.0f; break;
            case Text::Align::Horizontal::Centre:
                fOffsetV = std::max(0.0f, (fClipHeight - fHeight) * 0.5f);
                break;
            case Text::Align::Horizontal::Right:
                fOffsetV = std::max(0.0f, fClipHeight - fHeight);
                break;
                DEFAULT_UNREACHABLE;
            }
        }

        const int iOffsetV = static_cast<int>(std::floor(fOffsetV));

        DrawLines(0, iOffsetV);
        if (bScrolling && m_Scroller.IsLooping()) {
            DrawLines(0, static_cast<int>(
                           std::round(static_cast<float>(iOffsetV + iHeight)) +
                           m_fLoopGap));
        }
    }

    //--------------------------------------------------------------------------

    void CText::OnChanged(_In_ GDIChange eWhat, _In_ LPARAM lparam,
                          _In_ WPARAM wparam) noexcept {
        for (auto& line: m_Lines) { line.OnChanged(eWhat, lparam, wparam); }
    }

    //--------------------------------------------------------------------------

    void CText::ResumeLineScroll(bool bDelay) noexcept {
        for (auto& line: m_Lines) { line.ResumeScroll(bDelay); }
    }

    //--------------------------------------------------------------------------

    void CText::DrawLines(int iOffsetX, int iOffsetY) {
        const float fClipTop    = static_cast<float>(Clip().top);
        const float fClipBottom = static_cast<float>(Clip().bottom);

        POINT ViewportOffset = { iOffsetX, iOffsetY };
        Context().SetTextAlign(TA_LEFT | TA_TOP | TA_NOUPDATECP);
        for (auto& line: m_Lines) {
            PushViewportOriginOffset(ViewportOffset);
            {
                const auto iLineHeight = GetLineHeight(line);

                CPoint lineLimits[2] = {
                    { 0, 0 },
                    { line.Size().cx, iLineHeight },
                };
                WinAPIVerify(Context().LPtoDP(lineLimits, 2));

                // const auto fLineHeight = static_cast<float>(iLineHeight);
                const auto fLineStart = static_cast<float>(lineLimits[0].y);
                const auto fLineEnd   = static_cast<float>(lineLimits[1].y);

                bool bVisible = true;
                if (fLineStart > 0) {
                    bVisible = fLineStart < fClipBottom;
                } else {
                    bVisible = fLineEnd > fClipTop;
                }

                if (bVisible) {
                    if (m_bBGClear) {
                        line.Clear(Size().cx, line.Size().cy, 0.f);
                    }
                    line.Draw(Size().cx, line.Size().cy, 0.f);
                }

                ViewportOffset.y += iLineHeight;
            }
            PopViewportOrigin();
        }
    }

    //--------------------------------------------------------------------------

    void CText::SetLineCount(lineList::size_type count) {
        const auto clip = Clip();
        const auto clipSize =
          CSize{ clip.right - clip.left, clip.bottom - clip.top };

        // Grow if required
        while (static_cast<int>(m_Lines.size()) < count) {
            m_Lines.emplace_back(Context(), m_Font);
            auto& line = m_Lines.back();

            line.SetHAlign(m_eHAlign);
            line.SetHScroll(m_ScrollH.eMode, m_ScrollH.fSpeed, m_ScrollH.fDelay,
                            m_ScrollH.fGapFactor);
            line.ClipSize(clipSize);
            line.SetClear(m_bBGClear);
        }

        // Shrink if required
        while (static_cast<int>(m_Lines.size()) > count) { m_Lines.pop_back(); }
    }

    //--------------------------------------------------------------------------

    void CText::Update(int iWidth, int iHeight) {
        SetSize(iWidth, iHeight);

        m_eScrollLastStatus = ScrollStatus::None;

        if (m_bScrollStaggered) {
            m_iScrollLineID = 1;
            m_fTarget       = static_cast<float>(GetLineHeight(0));
        } else {
            m_iScrollLineID = 1;
            m_fTarget       = 0.f;
        }

        if (m_fLoopGapFactor > 0.f) {
            const auto fHeight    = static_cast<float>(iHeight);
            const auto fLineCount = static_cast<float>(m_Lines.size());
            m_fLoopGap = std::round(m_fLoopGapFactor * (fHeight / fLineCount));
        } else {
            m_fLoopGap = 0.f;
        }
        m_Scroller.Reset();
    }
} // namespace Windows::GDI
