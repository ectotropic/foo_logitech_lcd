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
#include "Windows/Thread/Thread_Worker.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_TimerResolution.h"
#include "Windows/Windows_MessageQueue.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

namespace {
    //**************************************************************************
    // ScopeExit class
    //**************************************************************************
    template <typename FuncT>
    class ScopeExit final {
    public:
        template <typename FuncT>
        ScopeExit(FuncT&& func) noexcept :
            m_fnExit{ std::forward<FuncT>(func) } {}

        ~ScopeExit() noexcept { m_fnExit(); }

        ScopeExit(const ScopeExit& ) = delete;
        ScopeExit(      ScopeExit&&) = delete;

    private:
        FuncT m_fnExit;
    }; // template <...> class ScopeExit final

    //------------------------------------------------------

    template <typename FuncT>
    inline [[nodiscard]] decltype(auto) AtScopeExit(FuncT&& fn) noexcept {
        return ScopeExit<FuncT>{std::forward<FuncT>(fn)};
    }
} // namespace <anonymous>

//==============================================================================

namespace Windows::Thread {
    //**************************************************************************
    // Worker
    //**************************************************************************
    void Worker::SetPeriod(_In_ period_type fTickPeriod,
                           _In_ period_type fUpdatePeriod) noexcept {
        assert(fTickPeriod > 0); if (fTickPeriod <= 0) { return; }
        m_fTickPeriod = fTickPeriod;
        if (fUpdatePeriod <= 0|| std::fabs(fTickPeriod - fUpdatePeriod) < 1.f) {
            m_fUpdatePeriod = m_fTickPeriod;
            m_UpdateTimer.Pause();
        } else {
            m_fUpdatePeriod = fUpdatePeriod;
            gsl_suppress(26467) // C26467: Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value.
            m_UpdateTimer.Period(static_cast<WMTimer::period_type>(std::floor(m_fUpdatePeriod)));
        }
    }

    //------------------------------------------------------

    Worker::WorkerStatus Worker::Update() {
        m_InterpStopWatch.Start();
        return OnUpdate();
    }

    //------------------------------------------------------

    Worker::WorkerStatus Worker::Tick() {
        if (m_UpdateTimer.Paused()) {
            if (Update() == WorkerStatus::Continue) {
                return OnTick(0.f);
            } else {
                return WorkerStatus::Quit;
            }
        } else {
            const auto elapsedMS = m_InterpStopWatch.GetElapsedMilliseconds();

            auto fInterpolation = elapsedMS / m_fUpdatePeriod;
            if (fInterpolation > 1.f) { fInterpolation = 1.f; }
            if (fInterpolation < 0.f) { fInterpolation = 0.f; }
            return OnTick(fInterpolation);
        }
    }

    //------------------------------------------------------

    Worker::ThreadWaitStatus Worker::ProcessThreadMessages() {
        MSG msg;
        ZeroMemory(&msg, sizeof(MSG));
        auto status{ ThreadWaitStatus::Continue };
        while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            auto msgStatus{ ThreadWaitStatus::Continue };
            switch (msg.message) {
                case WM_QUIT:
                    msgStatus = ThreadWaitStatus::Quit;
                    break;

                case WM_USER_TERMINATE:
                    msgStatus = ThreadWaitStatus::Terminate;
                    break;

                case WM_TIMER: {
                    if (msg.hwnd == NULL) {
                        msgStatus = (Update() == WorkerStatus::Continue)
                            ? ThreadWaitStatus::Continue
                            : ThreadWaitStatus::Terminate;
                    }
                    break;
                }

                default: break;
            }
            if (status == ThreadWaitStatus::Continue) {
                status = msgStatus;
            }
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        return status;
    }

    //------------------------------------------------------

    Worker::ThreadWaitStatus Worker::ThreadWait(_In_ float fMilliseconds) {
        // Setting the time period_type here causes the wait below to be
        // significantly more accurate. Without this the wait can be
        // tens of milliseconds off the requested duration.
        ScopedTimerResolution timerResolution{ TimerResolution::Resolution::Minimum() };
        StopWatch             sleepStopWatch{};

        auto status{ ThreadWaitStatus::Continue };
        do {
            sleepStopWatch.Start();
            DWORD dwWait = 0;
            if (fMilliseconds > 0) {
                gsl_suppress(26467) // C26467: Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value.
                dwWait = static_cast<DWORD>(std::floor(fMilliseconds));
            }
            switch (::MsgWaitForMultipleObjects(0,               /*nCount*/
                                                NULL,            /*pHandles*/
                                                FALSE,           /*fWaitAll*/
                                                dwWait,          /*dwMilliseconds*/
                                                QS_ALLEVENTS)) { /*dwWakeMask*/
                case WAIT_TIMEOUT:
                    fMilliseconds = 0;
                    break;

                case WAIT_OBJECT_0: {
                    status = ProcessThreadMessages();
                    fMilliseconds -= sleepStopWatch.GetElapsedMilliseconds();
                    break;
                }

                DEFAULT_UNREACHABLE;
            }
        } while ((status == ThreadWaitStatus::Continue) &&
                 (fMilliseconds > 0));
        return status;
    }

    //--------------------------------------------------------------------------
    //  Primary Public Interface
    //--------------------------------------------------------------------------

    void Worker::Run() {
        const auto scopeExit{
            ::AtScopeExit(
                [&]() noexcept {
                    m_UpdateTimer.Destroy();
                    m_dwThreadID.Exchange(0);
                }
            )
        };

        const auto dwThreadID = ::GetCurrentThreadId();
        m_dwThreadID.Set(dwThreadID);

        StopWatch tickTimer{};

        period_type fWait{ 0 };
        ThreadWaitStatus status = ThreadWait(fWait);
        while (status == ThreadWaitStatus::Continue) {
            tickTimer.Start();
            if (Tick() == WorkerStatus::Continue) {
                const auto fDuration = tickTimer.GetElapsedMilliseconds();
                if (m_bSleepPermitted && (fDuration < m_fTickPeriod)) {
                    fWait = m_fTickPeriod - fDuration;
                } else {
                    fWait = 0;
                }
                status = ThreadWait(fWait);
            } else {
                status = ThreadWaitStatus::Terminate;
            }
        }

        // Terminate may occur with messages still to be
        // processed, to ensure clean exit, need to process
        // them now.
        //
        // NOTE:
        //  - assumes all objects associated with the
        //    message queue have been disposed of already,
        //    any objects that remain may not be able to
        //    clean themselves up correctly.
        if (status == ThreadWaitStatus::Terminate) {
            PostTerminateMessageThreadWindow(dwThreadID);
            MSG msg;
            ZeroMemory(&msg, sizeof(MSG));
            while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
    }

    //------------------------------------------------------

    void Worker::Stop() noexcept {
        const auto dwThreadID = GetThreadID();
        if (dwThreadID != 0) {
            WinAPIVerify(PostTerminateMessageThread(dwThreadID, EXIT_SUCCESS));
        }
    }
} // namespace Windows::Thread
