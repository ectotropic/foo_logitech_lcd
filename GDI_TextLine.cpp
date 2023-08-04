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
#include "GDI_TextLine.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI_Util.h"
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // CTextLine
    //**************************************************************************
    void CTextLine::UpdateMetrics() noexcept {
        m_Size.cx = 0; m_Size.cy = 0;
        m_iAscent = 0; m_iCharWidth = 0;
        m_fLoopGap = 0.f;

        for (auto& fragment : m_Fragments) {
            const auto cxFrag = fragment.Size().cx;
            const auto cyFrag = fragment.Size().cy;

            const auto cxFragChar = fragment.CharWidth();
            const auto cyFragChar = fragment.Ascent();

            m_Size.cx += cxFrag;
            m_iCharWidth += cxFragChar;
            if (cyFrag     > m_Size.cy) { m_Size.cy = cyFrag; }
            if (cyFragChar > m_iAscent) { m_iAscent = cyFragChar; }
        }

        const auto fCharWidth = static_cast<float>(m_iCharWidth) / static_cast<float>(m_Fragments.size());
        m_iCharWidth = static_cast<int>(std::round(fCharWidth));
        if (m_fLoopGapFactor > 0.f) {
            m_fLoopGap = fCharWidth * m_fLoopGapFactor;
        }

        const auto cxLine = static_cast<float>(Size().cx);
        const auto cxClip = static_cast<float>(ClipSize().cx);

        if (cxLine < cxClip) {
            switch (m_eAlign) {
            case Text::Align::Horizontal::Left:   m_fOffset = 0.f; break;
            case Text::Align::Horizontal::Centre: m_fOffset = std::max(0.f, (cxClip - cxLine) * 0.5f); break;
            case Text::Align::Horizontal::Right:  m_fOffset = std::max(0.f, (cxClip - cxLine)); break;
                DEFAULT_UNREACHABLE;
            }
        } else {
            m_fOffset = 0.f;
        }
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetText(_In_ const CSimpleString& strLine) {
        // TODO: create fragments which can have
        //       different fonts/colors. Split
        //       source text into fragments where
        //       the control text:
        //          $font("name",size[,weight])
        //          $rgb(r,g,b)
        //       is present
        //	std::vector<CString> fragments = split(fragments, strLine, boost::is_any_of("$font,$rgb"));
        std::vector<CString> fragments{ strLine };

        SetFragmentCount(fragments.size());

        assert(m_Fragments.size() == fragments.size());

        for (decltype(fragments.size()) i = 0; i < fragments.size(); ++i) {
            m_Fragments[i].SetText(fragments[i],
                                   m_Font,
                                   m_DC.GetTextColor(),
                                   m_DC.GetBkColor(),
                                   0);
        }
        UpdateMetrics();
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetFont(_In_ const HFONT hFont) noexcept {
        for (auto& fragment : m_Fragments) {
            fragment.DefaultFont(hFont);
        }
        UpdateMetrics();
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetRenderMode(_In_ Text::RenderMode eRenderMode) noexcept {
        m_eRenderMode = eRenderMode;
        for (auto& fragment : m_Fragments) {
            fragment.SetRenderMode(m_eRenderMode);
        }
        UpdateMetrics();
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetTrimMode(_In_ Text::TrimMode eTrimMode) noexcept {
        m_eTrimMode = eTrimMode;
        for (auto& fragment : m_Fragments) {
            fragment.SetTrimMode(m_eTrimMode);
        }
        UpdateMetrics();
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetClear(_In_ bool bClear) noexcept {
        m_bClear = bClear;
    }

    //--------------------------------------------------------------------------

    void CTextLine::OnChanged(_In_ GDIChange eWhat,
                              _In_ LPARAM lparam,
                              _In_ WPARAM wparam) noexcept {
        switch (eWhat) {
        case GDIChange::Clip: {
            const RECT& clip = *(RECT*)lparam;
            const SIZE clipSize = {
                clip.right - clip.left,
                clip.bottom - clip.top
            };
            ClipSize(clipSize);
            break;
        }
        }
        for (auto& fragment : m_Fragments) {
            fragment.OnChanged(eWhat, lparam, wparam);
        }
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetHScroll(_In_ ScrollMode eMode,
                               _In_ float fSpeed,
                               _In_ float fDelay,
                               _In_ float fGapFactor) noexcept {
        m_Scroller.SetMode(eMode);
        m_Scroller.SetSpeed(fSpeed);
        m_Scroller.SetDelay(fDelay);
        if ((eMode == ScrollMode::Loop) && (fGapFactor > 0.f)) {
            m_fLoopGapFactor = fGapFactor;
            m_fLoopGap = static_cast<float>(m_iCharWidth) * m_fLoopGapFactor;
        } else {
            m_fLoopGapFactor = m_fLoopGap = 0.f;
        }
    }

    //--------------------------------------------------------------------------

    ScrollStatus CTextLine::Update(_In_ bool bShouldScroll) {
        if (!m_Scroller.IsScrolling()) { return ScrollStatus::None; }

        const auto cxLine = Size().cx;
        const auto cxClip = ClipSize().cx;
        if (cxLine < cxClip) { return ScrollStatus::None; }

        m_Scroller.Pause(!bShouldScroll);

        const auto cxfLine = static_cast<float>(cxLine);
        const auto cxfClip = static_cast<float>(cxClip);
        return m_Scroller.Update(cxfLine + m_fLoopGap + (static_cast<float>(m_iCharWidth) * .5f), cxfClip);
    }

    //--------------------------------------------------------------------------

    void CTextLine::Clear(_In_ int iWidth,
                          _In_ int iHeight,
                          _In_ float fInterp) {
        if (!m_bClear) { return; }

        POINT oldViewportOrg{ 0,0 };
        WinAPIVerify(m_DC.GetViewportOrg(&oldViewportOrg));

        const auto fOffset = m_fOffset + m_Scroller.GetOffset(fInterp);
        const auto nOffset = static_cast<int>(std::round(fOffset));

        WinAPIVerify(m_DC.OffsetViewportOrg(nOffset, 0));
        for (auto& fragment : m_Fragments) {
            const auto fragmentWidth = fragment.Size().cx;
            const auto iMaxWidth = iWidth > fragmentWidth ? fragmentWidth : iWidth;
            fragment.Clear(iMaxWidth, iHeight);
            WinAPIVerify(m_DC.OffsetViewportOrg(fragment.Size().cx, 0));
        }

        if (m_Scroller.IsLooping() &&
            m_Scroller.GetStatus() == ScrollStatus::InProgress_Stage2) {
            const auto nLoopGap = static_cast<int>(std::round(m_fLoopGap));
            WinAPIVerify(m_DC.OffsetViewportOrg(nLoopGap, 0));
            for (auto& fragment : m_Fragments) {
                const auto fragmentWidth = fragment.Size().cx;
                const auto iMaxWidth = iWidth > fragmentWidth ? fragmentWidth : iWidth;
                fragment.Clear(iMaxWidth, iHeight);
                WinAPIVerify(m_DC.OffsetViewportOrg(fragment.Size().cx, 0));
            }
        }

        WinAPIVerify(m_DC.SetViewportOrg(oldViewportOrg));
    }

    //--------------------------------------------------------------------------

    void CTextLine::Draw(_In_ int iWidth,
                         _In_ int iHeight,
                         _In_ float fInterp) {
        POINT oldViewportOrg{ 0,0 };
        WinAPIVerify(m_DC.GetViewportOrg(&oldViewportOrg));

        const auto fOffset = m_fOffset + m_Scroller.GetOffset(fInterp);
        const auto nOffset = static_cast<int>(std::round(fOffset));

        WinAPIVerify(m_DC.OffsetViewportOrg(nOffset, 0));
        for (auto& fragment : m_Fragments) {
            const auto fragmentWidth = fragment.Size().cx;
            const auto iMaxWidth = iWidth > fragmentWidth ? fragmentWidth : iWidth;
            fragment.Draw(iMaxWidth, iHeight);
            WinAPIVerify(m_DC.OffsetViewportOrg(fragmentWidth, 0));
        }

        if (m_Scroller.IsLooping() &&
            m_Scroller.GetStatus() == ScrollStatus::InProgress_Stage2) {
            const auto nLoopGap = static_cast<int>(std::round(m_fLoopGap));
            WinAPIVerify(m_DC.OffsetViewportOrg(nLoopGap, 0));
            for (auto& fragment : m_Fragments) {
                const auto fragmentWidth = fragment.Size().cx;
                const auto iMaxWidth = iWidth > fragmentWidth ? fragmentWidth : iWidth;
                fragment.Draw(iMaxWidth, iHeight);
                WinAPIVerify(m_DC.OffsetViewportOrg(fragmentWidth, 0));
            }
        }

        WinAPIVerify(m_DC.SetViewportOrg(oldViewportOrg));
    }

    //--------------------------------------------------------------------------

    void CTextLine::SetFragmentCount(_In_ fragmentList::size_type count) {
        //Grow if required
        while (m_Fragments.size() < count) {
            m_Fragments.emplace_back(Context(), m_Font);
            auto& fragment = m_Fragments.back();

            fragment.SetRenderMode(m_eRenderMode);
            fragment.SetTrimMode(m_eTrimMode);
        }

        //Shrink if required
        while (static_cast<int>(m_Fragments.size()) > count) {
            m_Fragments.pop_back();
        }
    }
} // namespace Windows::GDI
