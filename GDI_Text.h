#pragma once
#ifndef GUID_6E018332_34C7_4D85_8C9E_6BB59E236EE0
#define GUID_6E018332_34C7_4D85_8C9E_6BB59E236EE0
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

//--------------------------------------
//
#include "GDI_Drawable.h"
#include "GDI_TextLine.h"
#include "GDI_Scroller.h"
//--------------------------------------

namespace Windows::GDI {
    struct ScrollParams {
        ScrollMode eMode{ ScrollMode::None };
        float fSpeed{ 0.f };
        float fDelay{ 0.f };
        float fGapFactor{ 0.f };
    };

    class CText : public CDrawable {
    private:
        using thisClass = CText;
        using baseClass = CDrawable;

        using lineList = std::vector<CTextLine>;

    public:
        CText(_In_ HDC hDC);
        virtual ~CText() = default;

        void SetText(_In_reads_z_(nLength) LPCTSTR szText,
                     _In_ int nLength,
                     _In_ int nMaxLines = -1);
        void SetFont(_In_ const LOGFONT& logFont,
                     _In_ bool bAllowOverlap = false);

        void SetVAlign(_In_ Text::Align::Vertical eAlign) noexcept { m_eVAlign = eAlign; }
        void SetVScroll(_In_ const ScrollParams& params) noexcept;
        void SetVScroll(_In_ ScrollMode eMode,
                        _In_ float fSpeed,
                        _In_ float fDelay,
                        _In_ float fGapFactor) noexcept {
            SetVScroll(ScrollParams{ eMode, fSpeed, fDelay, fGapFactor });
        }

        void SetHAlign(_In_ Text::Align::Horizontal eAlign) noexcept;
        void SetHScroll(_In_ const ScrollParams& params) noexcept;
        void SetHScroll(_In_ ScrollMode eMode,
                        _In_ float fSpeed,
                        _In_ float fDelay,
                        _In_ float fGapFactor) noexcept {
            SetHScroll(ScrollParams{ eMode, fSpeed, fDelay, fGapFactor });
        }

        void SetBGClear(bool bClear) noexcept;

        void Reset() noexcept;
        void Update();

        explicit operator bool() const noexcept { return !m_Lines.empty(); }

    protected:
        virtual void OnDraw(float fInterp) override;
        virtual void OnChanged(_In_ GDIChange eWhat,
                               _In_ LPARAM lparam,
                               _In_ WPARAM wparam) noexcept override;

    private:
        void Update(int iWidth, int iHeight);

        void DrawLines(int iOffsetX, int iOffsetY);
        void SetLineCount(lineList::size_type iCount);
        void ResumeLineScroll(bool bDelay) noexcept;

        /*constexpr*/ auto GetLineHeight(const CTextLine& line) const noexcept {
            return m_bAllowOverlap ? line.Ascent() : line.Size().cy;
        }
        /*constexpr*/ auto GetLineHeight(int i) const noexcept {
            if (i >= 0 && i < static_cast<int>(m_Lines.size())) {
                return GetLineHeight(m_Lines[i]);
            } else {
                return 0L;
            }
        }

    private:
        CFont m_Font{};
        bool m_bAllowOverlap{ false };

    private:
        Text::Align::Vertical m_eVAlign{ Text::Align::Vertical::Centre };
        CScroller m_Scroller;
        float m_fLoopGapFactor{ 0.f };
        float m_fLoopGap{ 0.f };

    private:
        Text::Align::Horizontal m_eHAlign{ Text::Align::Horizontal::Centre };
        ScrollParams m_ScrollH{};

        float m_fTarget{ .0f };
        bool m_bBGClear{ false };

    private:
        lineList m_Lines;

        bool m_bScrollStaggered{ false };

        int m_iScrollLineID{ 0 };
        ScrollStatus m_eScrollLastStatus{ ScrollStatus::None };
    };
} // namespace Windows::GDI

#endif // GUID_6E018332_34C7_4D85_8C9E_6BB59E236EE0