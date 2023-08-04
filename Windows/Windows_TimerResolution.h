#pragma once
#ifndef GUID_39BB2481_0286_419E_86BD_4D54E1E7CDF6
#define GUID_39BB2481_0286_419E_86BD_4D54E1E7CDF6
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
#include "Windows/Windows_Core.h"
//--------------------------------------

//--------------------------------------
//
#include <timeapi.h>
//--------------------------------------

//--------------------------------------
//
#include <cassert>
//--------------------------------------

namespace Windows {
    //**************************************************************************
    // TimerResolution:
    // ----------------
    //
    // Set the multimedia timer to a specific resolution.
    //
    // Setting the timer resolution affects many time dependent actions,
    // including "wait" functions.
    //
    // REF: https://learn.microsoft.com/en-us/windows/win32/api/timeapi/
    //**************************************************************************
    class TimerResolution final {
    public:
        using period_type = decltype(TIMECAPS::wPeriodMin);

    private:
        static auto timeGetDevCaps() noexcept {
            TIMECAPS caps{};
            ZeroMemory(&caps, sizeof(TIMECAPS));
            [[maybe_unused]]
            const auto mmres = ::timeGetDevCaps(&caps, sizeof(TIMECAPS));
            assert(mmres == MMSYSERR_NOERROR);
            if (mmres != MMSYSERR_NOERROR) {
                ZeroMemory(&caps, sizeof(TIMECAPS));
            }
            return caps;
        }

    public:
        struct Resolution final {
            static auto Minimum() noexcept {
                static const auto s_PeriodMin{ timeGetDevCaps().wPeriodMin };
                return s_PeriodMin;
            }

            static auto Maximum() noexcept {
                static const auto s_PeriodMax{ timeGetDevCaps().wPeriodMax };
                return s_PeriodMax;
            }
        }; // struct Resolution final

    private:
        TimerResolution(TimerResolution&)            = delete; // No Copy
        TimerResolution& operator=(TimerResolution&) = delete; // No Copy

    public:
        TimerResolution() noexcept = default;

        // NOTE: "A lower value specifies a higher (more accurate) resolution."
        TimerResolution(period_type resolution) noexcept :
            m_Resolution{ resolution } {
        }

        bool BeginTimePeriod() const noexcept {
            const auto mmres = ::timeBeginPeriod(m_Resolution);
            assert(mmres == MMSYSERR_NOERROR);
            return mmres == MMSYSERR_NOERROR;
        }

        bool EndTimePeriod() const noexcept {
            const auto mmres = ::timeEndPeriod(m_Resolution);
            assert(mmres == MMSYSERR_NOERROR);
            return mmres == MMSYSERR_NOERROR;
        }

        constexpr void SetResolution(period_type resolution) noexcept {
            m_Resolution = resolution;
        }

        constexpr auto GetResolution() const noexcept {
            return m_Resolution;
        }

    private:
        period_type m_Resolution{ 0 };
    }; // class TimerResolution final

    //**************************************************************************
    // ScopedTimerResolution
    //**************************************************************************
    class ScopedTimerResolution final {
    public:
        using resolution_type = typename TimerResolution;
        using period_type     = typename resolution_type::period_type;

    private:
        ScopedTimerResolution(const ScopedTimerResolution& )            = delete; // No Copy
        ScopedTimerResolution(      ScopedTimerResolution&&)            = delete; // No Move
        ScopedTimerResolution& operator=(const ScopedTimerResolution& ) = delete; // No Copy
        ScopedTimerResolution& operator=(      ScopedTimerResolution&&) = delete; // No Move

    public:
        ScopedTimerResolution(period_type resolution) noexcept :
            m_TimerResolution{ resolution } {
            m_TimerResolution.BeginTimePeriod();
        }

        ~ScopedTimerResolution() noexcept {
            m_TimerResolution.EndTimePeriod();
        }

    private:
        resolution_type m_TimerResolution{ };
    }; // class ScopedTimerResolution final
} // namespace Windows

#endif // GUID_39BB2481_0286_419E_86BD_4D54E1E7CDF6