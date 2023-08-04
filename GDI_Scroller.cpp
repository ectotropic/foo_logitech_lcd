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
#include "GDI_Scroller.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // CScroller
    //**************************************************************************
    ScrollStatus CScroller::Update(_In_ float fObjectSize,
                                   _In_ float fCanvasSize) noexcept {
        if (fCanvasSize > fObjectSize) { return ScrollStatus::None; }
        if (m_eMode == ScrollMode::None) { return ScrollStatus::None; }
        if (m_fSpeed == 0.f) { return ScrollStatus::None; }
        if (m_bUserPaused) { return ScrollStatus::Paused; }

        if (m_bPaused && !m_Delay.HasElapsed()) {
            return ScrollStatus::Paused;
        } else {
            m_Delay.Stop();
            m_bPaused = false;
        }

        auto& last = m_Scroll[ScrollLast];
        auto& curr = m_Scroll[ScrollNext];
        auto  next = curr;

        switch (m_eMode) {
        case ScrollMode::Loop: {
            switch (curr.m_eStatus) {
            case ScrollStatus::Finished: {
                curr.m_fOffset = next.m_fOffset = 0.f;
                next.m_eStatus = ScrollStatus::InProgress_Stage1;
                ProcessDelay();
                break;
            }
            case ScrollStatus::InProgress_Stage1: {
                next.m_fOffset -= m_fSpeed;
                const auto fDiff = fObjectSize - fCanvasSize;
                if (std::fabs(next.m_fOffset) > fDiff) {
                    next.m_eStatus = ScrollStatus::InProgress_Stage2;
                }
                break;
            }
            case ScrollStatus::InProgress_Stage2: {
                next.m_fOffset -= m_fSpeed;
                if (std::fabs(next.m_fOffset) >= fObjectSize) {
                    next.m_fOffset = std::copysign(fObjectSize,
                                                   next.m_fOffset);
                    next.m_eStatus = ScrollStatus::Finished;
                }
                break;
            }
            default:
                curr.m_eStatus = next.m_eStatus = ScrollStatus::InProgress_Stage1;
                break;
            }
            break;
        }
        case ScrollMode::Bounce: {
            switch (curr.m_eStatus) {
            case ScrollStatus::Finished: {
                curr.m_fOffset = next.m_fOffset = 0.f;
                next.m_eStatus = ScrollStatus::InProgress_Stage1;
                ProcessDelay();
                break;
            }
            case ScrollStatus::InProgress_Stage1: {
                next.m_fOffset -= m_fSpeed;
                const auto fDiff = fObjectSize - fCanvasSize;
                if (next.m_fOffset <= -fDiff) {
                    next.m_fOffset = -fDiff;
                    next.m_eStatus = ScrollStatus::InProgress_Stage2;
                }
                break;
            }
            case ScrollStatus::InProgress_Stage2: {
                if (last.m_eStatus == ScrollStatus::InProgress_Stage1) {
                    ProcessDelay();
                    if (m_bPaused) { break; }
                }
                next.m_fOffset += m_fSpeed;
                if (next.m_fOffset >= 0.f) {
                    next.m_fOffset = 0.f;
                    next.m_eStatus = ScrollStatus::Finished;
                }
                break;
            }
            default:
                curr.m_eStatus = next.m_eStatus = ScrollStatus::InProgress_Stage1;
                break;
            }
            break;
        }
        DEFAULT_UNREACHABLE;
        }

        m_Scroll[ScrollLast] = curr;
        m_Scroll[ScrollNext] = next;

        return last.m_eStatus;
    }
} // namespace Windows::GDI
