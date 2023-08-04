#pragma once
#ifndef GUID_4C1FC41B_53DD_436F_9AA9_8501EC60901B
#define GUID_4C1FC41B_53DD_436F_9AA9_8501EC60901B
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
#include "Windows/Windows_StopWatch.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/SequentialEnum.h"
//--------------------------------------

//--------------------------------------
//
#include <cmath> // std::copysign, std::fabs
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // ScrollMode
    //**************************************************************************
    SEQUENTIAL_NAMED_ENUM(ScrollMode,
                          SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(None, 0), Loop, Bounce),
                          SEQUENTIAL_NAMED_ENUM_STRINGS(L"None", L"Loop", L"Bounce"));

    //**************************************************************************
    // ScrollStatus
    //**************************************************************************
    SEQUENTIAL_ENUM(ScrollStatus,
                    SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(None, 0),
                                           Paused,
                                           InProgress_Stage1,
                                           InProgress_Stage2,
                                           Target,
                                           Finished));

    //**************************************************************************
    // CScroller
    //**************************************************************************
    class CScroller {
    public:
        ScrollStatus Update(_In_ float fObjectSize,
                            _In_ float fCanvasSize) noexcept;

        ScrollStatus Update(_In_ float fObjectSize,
                            _In_ float fCanvasSize,
                            _In_ float fTarget) noexcept {
            const auto eStatus = Update(fObjectSize,
                                        fCanvasSize);
            if (eStatus == ScrollStatus::Paused ||
                eStatus == ScrollStatus::None ||
                fTarget <= 0.f) {
                return eStatus;
            }

            auto& next = m_Scroll[ScrollNext];
            if (std::fabs(next.m_fOffset) >= fTarget) {
                next.m_fOffset = std::copysign(fTarget, next.m_fOffset);
                return ScrollStatus::Target;
            } else {
                return eStatus;
            }
        }

        constexpr void Reset() noexcept {
            m_Scroll[ScrollLast] = m_Scroll[ScrollNext] = {};
            m_bPaused = false;
        }

        //-------------------------------------
        // Pause
        constexpr void Pause(_In_ bool bPause = true) { m_bUserPaused = bPause; };
        void Resume(_In_ bool bDelay = false) noexcept {
            if (m_bUserPaused) {
                m_bUserPaused = false;
                if (bDelay) {
                    ProcessDelay();
                }
            }
        }
        //-------------------------------------

        //-------------------------------------
        // Mode
        constexpr void SetMode(_In_ ScrollMode eMode) noexcept {
            if (m_eMode != eMode) {
                m_eMode = eMode;
                Reset();
            }
        }
        constexpr auto GetMode() const noexcept { return m_eMode; }
        //-------------------------------------

        //-------------------------------------
        // Speed
        constexpr void SetSpeed(_In_ float fSpeed) noexcept {
            if (m_fSpeed != fSpeed) {
                m_fSpeed = fSpeed;
                Reset();
            }
        }
        constexpr auto GetSpeed() const noexcept { return m_fSpeed; }
        //-------------------------------------

        //-------------------------------------
        // Delay
        constexpr void SetDelay(_In_ float fDelay)       noexcept { m_Delay.Reset(TimedEvent::Duration::Seconds{ fDelay }); }
        constexpr auto GetDelay()                  const noexcept { return m_Delay.GetDurationSeconds(); }
        //-------------------------------------

        constexpr auto GetStatus() const noexcept { return m_Scroll[ScrollLast].m_eStatus; }

        constexpr auto GetScrollOffset() const noexcept { return m_Scroll[ScrollLast].m_fOffset; }

        constexpr auto GetOffset(_In_ float fInterp) const noexcept {
            const auto fDiff = m_Scroll[ScrollNext].m_fOffset - m_Scroll[ScrollLast].m_fOffset;
            return m_Scroll[ScrollLast].m_fOffset + (fDiff * fInterp);
        }

        constexpr auto GetOffset() const noexcept {
            return m_Scroll[ScrollLast].m_fOffset;
        }

        constexpr bool    IsPaused() const noexcept { return m_bPaused || m_bUserPaused; }
        constexpr bool IsScrolling() const noexcept { return (m_eMode != ScrollMode::None) && (m_fSpeed != 0.f); };
        constexpr bool   IsLooping() const noexcept { return m_eMode == ScrollMode::Loop; };
        constexpr bool  IsBouncing() const noexcept { return m_eMode == ScrollMode::Bounce; };

    private:
        void ProcessDelay() noexcept {
            if (m_Delay.HasDuration()) {
                m_bPaused = true;
                m_Delay.Start();
            }
        }

    private:
        ScrollMode m_eMode{ ScrollMode::Loop };
        float      m_fSpeed{ 0.f };

        enum { ScrollLast = 0, ScrollNext, _scrollCount };
        struct {
            float        m_fOffset{ 0.f };
            ScrollStatus m_eStatus{ ScrollStatus::None };
        } m_Scroll[_scrollCount]{ };

        TimedEvent m_Delay      { };
        bool        m_bPaused    { false };
        bool        m_bUserPaused{ false };
    }; // class CScroller
} // namespace Windows::GDI

#endif // GUID_4C1FC41B_53DD_436F_9AA9_8501EC60901B