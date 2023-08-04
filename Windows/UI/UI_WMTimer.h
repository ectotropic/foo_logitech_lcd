#pragma once
#ifndef GUID_3F8BF36E_8ED1_476D_9697_33C7DBBB3C27
#define GUID_3F8BF36E_8ED1_476D_9697_33C7DBBB3C27
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
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // WMTimer
    //**************************************************************************
    class WMTimer final {
    private:
        using thisClass = WMTimer;

    public:
        using timer_handle_type = UINT_PTR;
        using period_type       = UINT;

        struct Period {
            static constexpr const period_type Minimum = USER_TIMER_MINIMUM;
            static constexpr const period_type Maximum = USER_TIMER_MAXIMUM;
        }; // struct Period

    private:
        static auto DestroyTimer(_In_ timer_handle_type hTimer) noexcept {
            const auto res = ::KillTimer(NULL, hTimer);
            WinAPIAssert(res);
            return res;
        }

        static auto CreateTimer(_Inout_ timer_handle_type& hTimer,
                                _In_    period_type        period_type) noexcept {
            auto hNewTimer = ::SetTimer(NULL, hTimer, period_type, NULL);
            WinAPIAssert(hNewTimer && (!hTimer || (hNewTimer == hTimer)));
            if (hNewTimer && (!hTimer || (hNewTimer == hTimer))) {
                hTimer = hNewTimer;
                return TRUE;
            } else {
                if (hNewTimer) { thisClass::DestroyTimer(hNewTimer); }
                if (hTimer)    { thisClass::DestroyTimer(hTimer); }
                return FALSE;
            }
        }

        static auto CreateTimer(_In_ period_type period_type) noexcept {
            timer_handle_type hTimer{ 0 };
            thisClass::CreateTimer(hTimer, period_type);
            return hTimer;
        }

    private:
        WMTimer(const thisClass&)          = delete; // No Copy
        WMTimer& operator=(const WMTimer&) = delete; // No Copy

    public:
        WMTimer() noexcept = default;

        WMTimer(period_type period_type) noexcept :
            m_Timer { thisClass::CreateTimer(period_type) },
            m_Period{ period_type } {}


        WMTimer(thisClass&& other) noexcept :
            m_Timer  { std::exchange(other.m_Timer  , 0) },
            m_Period { std::exchange(other.m_Period , 0) },
            m_bPaused{ std::exchange(other.m_bPaused, false) } {}

        WMTimer& operator=(WMTimer&& other) noexcept {
            Destroy();
            m_Timer   = std::exchange(other.m_Timer  , 0);
            m_Period  = std::exchange(other.m_Period , 0);
            m_bPaused = std::exchange(other.m_bPaused, false);
        }

        ~WMTimer() noexcept { Destroy(); }

    public:
        void Destroy() noexcept {
            if (m_Timer) {
                thisClass::DestroyTimer(m_Timer);
                m_Timer   = 0;
                m_Period  = 0;
                m_bPaused = false;
            }
        }

        auto Pause() noexcept {
            if (m_bPaused) { return TRUE; }
            assert(m_Timer);
            return thisClass::DestroyTimer(m_Timer);
        }

        auto Resume() noexcept {
            if (!m_bPaused) { return TRUE; }
            assert(m_Timer);
            return thisClass::CreateTimer(m_Timer, m_Period);
        }

        auto Period(period_type period_type) noexcept {
            m_Period = period_type;
            return thisClass::CreateTimer(m_Timer, m_Period) &&
                ((!m_bPaused) || Pause());
        }

        constexpr auto Paused() const noexcept { return m_bPaused; }
        constexpr auto Period() const noexcept { return m_Period; }
        constexpr auto Timer () const noexcept { return m_Timer; }

        explicit constexpr operator timer_handle_type() const noexcept {
            return m_Timer;
        }

        explicit constexpr operator bool() const noexcept {
            return m_Timer != 0;
        }

    private:
        bool              m_bPaused{ false };
        timer_handle_type m_Timer  { 0 };
        period_type       m_Period { 0 };
    }; // class WMTimer final
} // namespace Windows

#endif // GUID_3F8BF36E_8ED1_476D_9697_33C7DBBB3C27