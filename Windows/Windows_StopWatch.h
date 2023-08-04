#pragma once
#ifndef GUID_5B34B779_0C21_411C_B1BD_86DB0667ED6C
#define GUID_5B34B779_0C21_411C_B1BD_86DB0667ED6C
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
#include <profileapi.h> // QueryPerformanceFrequency && QueryPerformanceCounter
//--------------------------------------

//--------------------------------------
//
#include <utility>
#include <type_traits>
//--------------------------------------

namespace Windows {
    //**************************************************************************
    // StopWatchT:
    // -----------
    //
    // Measure the time taken between "Start" and "Stop".
    //
    // REF: https://learn.microsoft.com/en-us/windows/win32/api/profileapi/
    //**************************************************************************
    template<typename PeriodT>
    class StopWatchT {
    private:
        using thisClass      = StopWatchT<PeriodT>;

    public:
        using period_type    = PeriodT;
        using frequency_type = decltype(LARGE_INTEGER::QuadPart);
        using tick_type      = decltype(LARGE_INTEGER::QuadPart);

    private:
        static auto DoQueryPerformanceFrequency() noexcept {
            LARGE_INTEGER frequency{ 0 };
            //  "On systems that run Windows XP or later, the function
            //   will always succeed and will thus never return zero."
            ::QueryPerformanceFrequency(&frequency);
            return frequency.QuadPart;
        }

    public:
        static auto SystemFrequency() noexcept {
            static const auto s_Frequency{
                thisClass::DoQueryPerformanceFrequency()
            };
            return s_Frequency;
        }

        static auto SystemPeriodSeconds() noexcept {
            static const auto s_PeriodSeconds{
                static_cast<period_type>(1) / thisClass::SystemFrequency()
            };
            return s_PeriodSeconds;
        }

        static auto SystemPeriodMilliseconds() noexcept {
            static const auto s_PeriodMilliseconds{
                static_cast<period_type>(1000) / thisClass::SystemFrequency()
            };
            return s_PeriodMilliseconds;
        }

        static auto SystemCounter() noexcept {
            LARGE_INTEGER counter{ 0 };
            //  "On systems that run Windows XP or later, the function
            //   will always succeed and will thus never return zero."
            ::QueryPerformanceCounter(&counter);
            return counter.QuadPart;
        }

        static auto TicksToSeconds(tick_type ticks) noexcept {
            return static_cast<period_type>(ticks) * thisClass::SystemPeriodSeconds();
        }

        static auto TicksToMilliseconds(tick_type ticks) noexcept {
            return static_cast<period_type>(ticks) * thisClass::SystemPeriodMilliseconds();
        }

        static auto SecondsToTicks(period_type seconds) noexcept {
            static const auto s_Factor{
                static_cast<double>(thisClass::SystemFrequency())
            };
            return static_cast<tick_type>(static_cast<double>(seconds) * s_Factor);
        }

        static auto MillisecondsToTicks(period_type milliseconds) noexcept {
            static const auto s_Factor{
                static_cast<period_type>(thisClass::SystemFrequency()) / static_cast<period_type>(1000)
            };
            return static_cast<tick_type>(milliseconds * s_Factor);
        }

    private:
        static constexpr auto GetElapsed(_In_ tick_type start,
                                         _In_ tick_type end = thisClass::SystemCounter()) noexcept {
            return end - start;
        }

    private:
        template <
            typename IntT = tick_type,
            std::enable_if_t<std::is_integral_v<IntT>, bool> = true
        >
        explicit constexpr StopWatchT(IntT ticks) noexcept :
            m_Ticks{ static_cast<tick_type>(ticks) } {};

    public:
        StopWatchT() noexcept = default;
        StopWatchT(const thisClass&) noexcept = default;

        void Reset() noexcept { m_Ticks = 0; }

        void Start() noexcept { m_Ticks = thisClass::SystemCounter(); }
        auto Stop () noexcept { return GetElapsed(std::exchange(m_Ticks, 0)); }

        auto GetElapsedTicks() const noexcept {
            return thisClass::GetElapsed(m_Ticks);
        }

        auto GetElapsedSeconds() const noexcept {
            return thisClass::TicksToSeconds(GetElapsedTicks());
        }

        auto GetElapsedMilliseconds() const noexcept {
            return thisClass::TicksToMilliseconds(GetElapsedTicks());
        }

        constexpr auto GetTicks () const noexcept { return m_Ticks; };
        constexpr auto IsRunning() const noexcept { return m_Ticks != 0; }

        explicit constexpr operator bool() const noexcept { return IsRunning(); }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr thisClass operator-(OtherT other) const noexcept {
            return thisClass{ m_Ticks - thisClass{ other }.m_Ticks };
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr thisClass operator+(OtherT other) const noexcept {
            return thisClass{ m_Ticks + thisClass{ other }.m_Ticks };
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr thisClass& operator-=(OtherT other) noexcept {
            m_Ticks -= thisClass{ other }.m_Ticks;
            return *this;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr thisClass& operator+=(OtherT other) noexcept {
            m_Ticks += thisClass{ other }.m_Ticks;
            return *this;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator==(OtherT other) const noexcept {
            return m_Ticks == thisClass{ other }.m_Ticks;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator!=(OtherT other) const noexcept {
            return m_Ticks != thisClass{ other }.m_Ticks;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator<=(OtherT other) const noexcept {
            return m_Ticks <= thisClass{ other }.m_Ticks;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator>=(OtherT other) const noexcept {
            return m_Ticks >= thisClass{ other }.m_Ticks;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator<(OtherT other) const noexcept {
            return m_Ticks < thisClass{ other }.m_Ticks;
        }

        template <
            typename OtherT = thisClass,
            std::enable_if_t<std::is_constructible_v<OtherT, thisClass>, bool> = true
        >
        constexpr auto operator>(OtherT other) const noexcept {
            return m_Ticks > thisClass{ other }.m_Ticks;
        }

    private:
        tick_type m_Ticks{ 0 };
    }; // template <...> class StopWatchT final

    //**************************************************************************
    // TimedEventT:
    // -----------
    //
    // Determine if a specific duration has passed (i.e. a timed event is due).
    //
    // Duration is measured in "ticks" for enhanced accuracy.
    //**************************************************************************
    template <typename PeriodT>
    class TimedEventT final {
    private:
        using thisClass = TimedEventT<PeriodT>;

    public:
        using stop_watch_type = StopWatchT<PeriodT>;
        using period_type     = typename stop_watch_type::period_type;
        using frequency_type  = typename stop_watch_type::frequency_type;
        using tick_type       = typename stop_watch_type::tick_type;

        struct Duration final {
            struct Seconds final {
                Seconds() noexcept = default;
                Seconds(const Seconds&) noexcept = default;
                explicit constexpr Seconds(period_type duration) noexcept :
                    m_Duration{ duration } {}

                period_type m_Duration{ 0 };
            }; // struct Seconds final

            struct Milliseconds final {
                Milliseconds() noexcept = default;
                Milliseconds(const Milliseconds&) noexcept = default;
                explicit constexpr Milliseconds(period_type duration) noexcept :
                    m_Duration{ duration } {}

                period_type m_Duration{ 0 };
            }; // struct Milliseconds final

            struct Ticks final {
                Ticks() noexcept = default;
                Ticks(const Ticks&) noexcept = default;
                explicit constexpr Ticks(period_type duration) noexcept :
                    m_Duration{ duration } {}
                explicit constexpr Ticks(Seconds duration) noexcept :
                    m_Duration{ stop_watch_type::SecondsToTicks(duration.m_Duration) } {}
                explicit constexpr Ticks(Milliseconds duration) noexcept :
                    m_Duration{ stop_watch_type::MillisecondsToTicks(duration.m_Duration) } {}

                tick_type m_Duration{ 0 };
            }; // struct Ticks final
        }; // struct Duration final

        using duration_ticks        = typename Duration::Ticks;
        using duration_seconds      = typename Duration::Seconds;
        using duration_milliseconds = typename Duration::Milliseconds;

    public:
        TimedEventT() noexcept = default;
        TimedEventT(const TimedEventT& ) noexcept = default;
        TimedEventT(      TimedEventT&&) noexcept = default;

        explicit constexpr TimedEventT(duration_ticks duration) noexcept :
            m_Duration{ duration.m_Duration } {}

        explicit constexpr TimedEventT(duration_seconds duration) noexcept :
            TimedEventT{ duration_ticks{ duration } } {}

        explicit constexpr TimedEventT(duration_milliseconds duration) noexcept :
            TimedEventT{ duration_ticks{ duration } } {}

        constexpr void Reset() noexcept {
            m_StopWatch.Reset();
        }

        constexpr void Reset(duration_ticks duration) noexcept {
            Reset();
            m_Duration = duration.m_Duration;
        }

        constexpr void Reset(duration_seconds duration) noexcept {
            Reset(duration_ticks{ duration });
        }

        constexpr void Reset(duration_milliseconds duration) noexcept {
            Reset(duration_ticks{ duration });
        }

        void Start() noexcept {
            m_StopWatch.Start();
        }

        decltype(auto) Stop() noexcept {
            return m_StopWatch.Stop();
        }

        decltype(auto) GetElapsedTicks() const noexcept {
            return m_StopWatch.GetElapsedTicks();
        }

        decltype(auto) GetElapsedSeconds() const noexcept {
            return m_StopWatch.GetElapsedSeconds();
        }

        decltype(auto) GetElapsedMilliseconds() const noexcept {
            return m_StopWatch.GetElapsedMilliseconds();
        }

        decltype(auto) IsRunning() const noexcept {
            return m_StopWatch.IsRunning();
        }

        constexpr bool HasDuration() const noexcept {
            return m_Duration > 0;
        }

        constexpr bool HasElapsed() const noexcept {
            return !IsRunning() || GetElapsedTicks() >= m_Duration;
        }

        constexpr auto GetDurationTicks() const noexcept {
            return m_Duration;
        }

        constexpr auto GetDurationSeconds() const noexcept {
            return stop_watch_type::TicksToSeconds(m_Duration);
        }

        constexpr auto GetDurationMilliseconds() const noexcept {
            return stop_watch_type::TicksToMilliseconds(m_Duration);
        }

    private:
        tick_type       m_Duration { 0 };
        stop_watch_type m_StopWatch{ };
    }; // template <...> class TimedEventT final

    //**************************************************************************
    // Specialisations
    //**************************************************************************
    using StopWatch  = StopWatchT<float>;
    using TimedEvent = TimedEventT<float>;
} // namespace Windows

#endif // GUID_5B34B779_0C21_411C_B1BD_86DB0667ED6C