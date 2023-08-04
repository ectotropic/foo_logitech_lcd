#pragma once
#ifndef GUID_8BD8292A_3EBC_48B6_B280_67CE900266B9
#define GUID_8BD8292A_3EBC_48B6_B280_67CE900266B9
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
#include "GDI_TextFragment.h"
#include "GDI_Scroller.h"
//--------------------------------------

//--------------------------------------
//
#include <vector>
//--------------------------------------

namespace Windows::GDI::Text {
    //**************************************************************************
    // Align
    //**************************************************************************
    struct Align final {
        //**********************************************************************
        // Vertical
        //**********************************************************************
        SEQUENTIAL_NAMED_ENUM(Vertical,
                              SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Top, 0),
                                                     Centre,
                                                     Bottom),
                              SEQUENTIAL_NAMED_ENUM_STRINGS(L"Top",
                                                            L"Bottom",
                                                            L"Centre"));

        //**********************************************************************
        // Horizontal
        //**********************************************************************
        SEQUENTIAL_NAMED_ENUM(Horizontal,
                              SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Left, 0),
                                                     Centre,
                                                     Right),
                              SEQUENTIAL_NAMED_ENUM_STRINGS(L"Left",
                                                            L"Centre",
                                                            L"Right"));

    }; // struct Align final
} // namespace Windows::GDI::Text

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // CTextLine
    //**************************************************************************
    class CTextLine {
    private:
        using fragmentList = std::vector<CTextFragment>;

        CTextLine() = delete;
        CTextLine(CTextLine&) = delete;
        CTextLine& operator=(CTextLine&) = delete;

    public:
        CTextLine(CTextLine&&) = default;
        CTextLine& operator=(CTextLine&&) = default;

        CTextLine(_In_ HDC hDC,
                  _In_ HFONT hFont) noexcept :
            m_DC(hDC),
            m_Font(hFont) {
            assert(hDC);
            assert(hFont);
        }

        void SetText(_In_ const CSimpleString& str);
        void SetFont(_In_ const HFONT hFont) noexcept;


        void SetRenderMode(_In_ Text::RenderMode eRenderMode) noexcept;
        void   SetTrimMode(_In_ Text::TrimMode eTrimMode) noexcept;

        constexpr void SetHAlign(_In_ Text::Align::Horizontal eAlign) noexcept {
            m_eAlign = eAlign;
        }

        void SetHScroll(_In_ ScrollMode eMode,
                        _In_ float fSpeed,
                        _In_ float fDelay,
                        _In_ float fGapFactor) noexcept;

        void SetClear(_In_ bool bClear) noexcept;

        void PauseScroll() noexcept { m_Scroller.Pause(); }
        void ResumeScroll(_In_ bool bDelay) noexcept { m_Scroller.Resume(bDelay); }

        ScrollStatus Update(_In_ bool bShouldScroll);
        void Clear(_In_ int iWidth,
                   _In_ int iHeight,
                   _In_ float fInterp);
        void Draw(_In_ int iWidth,
                  _In_ int iHeight,
                  _In_ float fInterp);


        void OnChanged(_In_ GDIChange eWhat,
                       _In_ LPARAM lparam,
                       _In_ WPARAM wparam) noexcept;

    public:
        auto Context() const noexcept { return m_DC; }
        auto Context()       noexcept { return m_DC; }

        auto Font() const noexcept { return m_Font; }
        auto Font()       noexcept { return m_Font; }

        constexpr SIZE Size() const noexcept { return m_Size; }

        constexpr     SIZE ClipSize() const noexcept { return m_ClipSize; }
        /*constexpr*/ void ClipSize(_In_ SIZE size) noexcept { m_ClipSize = size; };

        constexpr auto    Ascent() const noexcept { return m_iAscent; }
        constexpr auto CharWidth() const noexcept { return m_iCharWidth; }

    private:
        void SetFragmentCount(_In_ fragmentList::size_type count);
        void UpdateMetrics() noexcept;

    private:
        fragmentList m_Fragments;

        CDCHandle m_DC{};

        CFontHandle m_Font{};
        int m_iAscent{ 0 };
        int m_iCharWidth{ 0 };

        Text::RenderMode m_eRenderMode{ Text::RenderMode::Default };
        Text::TrimMode   m_eTrimMode  { Text::TrimMode::None };

        bool m_bClear{ false };

        Text::Align::Horizontal m_eAlign{ Text::Align::Horizontal::Centre };
        float m_fOffset{ 0.f };

        CScroller m_Scroller{};
        float m_fLoopGapFactor{ 0.f };
        float m_fLoopGap{ 0.f };

        CSize m_Size{ 0, 0 };
        CSize m_ClipSize{ 0, 0 };
    }; // class CTextLine
} // namespace Windows::GDI

#endif // GUID_8BD8292A_3EBC_48B6_B280_67CE900266B9