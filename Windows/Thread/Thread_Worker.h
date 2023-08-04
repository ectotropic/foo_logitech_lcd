#pragma once
#ifndef GUID_8161682A_DDD1_468C_A7AA_B06E3D21EAC8
#define GUID_8161682A_DDD1_468C_A7AA_B06E3D21EAC8
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
#include "Windows/Thread/Thread_CriticalSection.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_StopWatch.h"
#include "Windows/UI/UI_WMTimer.h"
//--------------------------------------

//--------------------------------------
//
#include <cmath>
//--------------------------------------

namespace Windows::Thread {
    //**************************************************************************
    // Worker:
    // -------
    //
    // A base class for worker threads which process work in "ticks".
    //
    // Derived class is called to update/process at specified Hz, or as fast
    // as possible, while Windows messages are processed in-between these
    // calls.
    //
    // If updating/processing takes a significant period_type of time, response to
    // Windows messages will be delayed (likely causing issues).
    //
    // The calls to derived functions are always made from the thread Worker
    // belongs to; calls will not happen concurrently.
    //
    // Worker::Run should be called directly from the thread main function of
    // the thread Worker is to work in. Creation of the thread is expected to
    // have happened externally before this call.
    //
    //**************************************************************************
    class Worker {
    private:
        using WMTimer           = Windows::UI::WMTimer;
        using ProtectedThreadID = CriticalSectionProtectedVariableT<DWORD>;

        enum class ThreadWaitStatus {
            Continue,   // Keep going
            Quit,       // Windows Quit Message received
            Terminate,  // Request for termination, or an
                        // error occurred
        };

    public:
        using period_type    = float;
        using thread_id_type = typename ProtectedThreadID::valueType;

        enum class WorkerStatus {
            Continue = static_cast<int>(ThreadWaitStatus::Continue),
            Quit     = static_cast<int>(ThreadWaitStatus::Terminate),
        };

    protected:
        static constexpr period_type PeriodToFrequency(_In_ period_type fPeriod) noexcept {
            if (fPeriod <= 0) { return 0; }
            constexpr const auto fOneThousandth = 1.f / 1000.f;
            return std::floor(1.f / (fPeriod * fOneThousandth));
        }

        static constexpr period_type FrequencyToPeriod(_In_ period_type fFrequency) noexcept {
            if (fFrequency <= 0) { return 0; }
            return std::floor(1000.f * (1.f / fFrequency));
        }

    protected:
        Worker(_In_ bool bSleepPermitted = true) noexcept :
            m_bSleepPermitted{ bSleepPermitted } {}

    public:
        virtual ~Worker() noexcept = default;

        void Run ();
        void Stop() noexcept;

    private:
        WorkerStatus Tick  ();
        WorkerStatus Update();

    protected: // Derived Interface
        virtual WorkerStatus OnTick(float fInterp) = 0;

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual WorkerStatus OnUpdate() { return WorkerStatus::Continue; };

    private: // Message pump
        ThreadWaitStatus ThreadWait(_In_ float fMilliseconds);
        ThreadWaitStatus ProcessThreadMessages();

    protected: // Derived accessors
        constexpr bool GetSleepPermitted() const noexcept {
            return m_bSleepPermitted;
        }

        constexpr void SetSleepPermitted(bool bCanSleep) noexcept {
            m_bSleepPermitted = bCanSleep;
        }

        constexpr auto GetTickPeriod() const noexcept {
            return m_fTickPeriod;
        }
        constexpr auto GetTickFrequency() const noexcept {
            return PeriodToFrequency(m_fTickPeriod);
        }
        constexpr auto GetUpdatePeriod() const noexcept {
            return m_fUpdatePeriod;
        }
        constexpr auto GetUpdateFrequency() const noexcept {
            return PeriodToFrequency(m_fUpdatePeriod);
        }

        void SetPeriod(_In_ period_type fTickPeriod,
                       _In_ period_type fUpdatePeriod) noexcept;

        void SetFrequency(_In_ period_type fTickFrequency,
                          _In_ period_type fUpdateFrequency) noexcept {
            SetPeriod(FrequencyToPeriod(fTickFrequency),
                      FrequencyToPeriod(fUpdateFrequency));
        }

        void SetTickPeriod(_In_ period_type fPeriod) noexcept {
            SetPeriod(fPeriod, m_fUpdatePeriod);
        }

        void SetUpdatePeriod(_In_ period_type fPeriod) noexcept {
            SetPeriod(m_fTickPeriod, fPeriod);
        }

        void SetTickFrequency(_In_ period_type fFrequency) noexcept {
            SetFrequency(fFrequency, m_fUpdatePeriod);
        }

        void SetUpdateFrequency(_In_ period_type fFrequency) noexcept {
            SetFrequency(m_fTickPeriod, fFrequency);
        }

    public: // Accessors
        decltype(auto) GetThreadID() const noexcept { return m_dwThreadID.Get(); }
        bool           IsRunning  () const noexcept { return GetThreadID() != 0; }

    private:
        bool              m_bSleepPermitted{ true };
        period_type       m_fTickPeriod    { FrequencyToPeriod(30.f) };
        period_type       m_fUpdatePeriod  { FrequencyToPeriod(15.f) };
        WMTimer           m_UpdateTimer    {};
        StopWatch         m_InterpStopWatch{};
        ProtectedThreadID m_dwThreadID     { 0 };
    }; // class Worker
} // namespace Windows::Thread

#endif // GUID_8161682A_DDD1_468C_A7AA_B06E3D21EAC8