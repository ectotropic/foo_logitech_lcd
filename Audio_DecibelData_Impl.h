#pragma once
#ifndef GUID_2926D6AE_BCF7_4C63_A96C_1879B2C84D1D
#define GUID_2926D6AE_BCF7_4C63_A96C_1879B2C84D1D
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
#include <type_traits>
#include "Util/InterpolateUtil.h"
//--------------------------------------

namespace Audio::dB {
    /**************************************************************************
     * InterpolatedDataT *
     **************************************************************************/
    template <typename DecibelTypeT>
    class InterpolatedDataT final :
        public ::util::interpolated_value<DecibelTypeT> {
    private:
        using base_type = ::util::interpolated_value<DecibelTypeT>;
        using this_type = InterpolatedDataT<DecibelTypeT>;

    public:
        using dB_type      = typename base_type::value_type;
        using decibel_type = typename dB_type;

    public:
        constexpr InterpolatedDataT() noexcept = default;
        constexpr InterpolatedDataT(dB_type prev,
                                    dB_type next) noexcept :
            base_type{ prev, next } {}
        constexpr InterpolatedDataT(dB_type dB) noexcept :
            base_type{ dB } {}

        constexpr InterpolatedDataT(const this_type& ) noexcept = default;
        constexpr InterpolatedDataT(      this_type&&) noexcept = default;

        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;
    }; // template <...> class InterpolatedDataT

    /**************************************************************************
     * InterpolatedPeakDataT *
     **************************************************************************/
    template <typename DecibelTypeT>
    class InterpolatedPeakDataT final :
        public ::util::interpolated_value<DecibelTypeT> {
    private:
        using base_type = ::util::interpolated_value<DecibelTypeT>;
        using this_type = InterpolatedPeakDataT<DecibelTypeT>;

    public:
        using dB_type      = typename base_type::value_type;
        using decibel_type = typename dB_type;
        using peak_type    = typename dB_type;

    public:
        InterpolatedPeakDataT() = default;
        InterpolatedPeakDataT(dB_type prev,
                              dB_type next,
                              peak_type decay,
                              peak_type minimum) noexcept :
            base_type{ ((decay > 0) ? prev : 0),
                       ((decay > 0) ? next : 0) },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(dB_type dB,
                              peak_type decay,
                              peak_type minimum) :
            base_type{ ((decay > 0) ? dB : 0) },
            m_DecayRate{ decay } {}

        constexpr InterpolatedPeakDataT(const this_type& ) noexcept = default;
        constexpr InterpolatedPeakDataT(      this_type&&) noexcept = default;

        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

    private:
        using base_type::update;

    public:
        constexpr void update(dB_type dB) noexcept {
            if (!have_peak()) { return; }
            m_Prev = m_Next;
            if (dB > m_Next) {
                m_Next = dB;
            } else if (m_Next > m_Minimum) {
                m_Next -= m_DecayRate;
            } else {
                m_Next = m_Minimum;
            }
        }

        constexpr void decay_rate(peak_type decay,
                                  peak_type minimum) noexcept {
            if (have_peak() && decay <= 0) { zero(); }
            m_DecayRate = decay;
            m_Minimum   = minimum;
        }

        constexpr void decay_rate(peak_type decay) noexcept {
            decay_rate(decay, m_Minimum);
        }

        constexpr void minimum(peak_type minimum) noexcept {
            decay_rate(m_DecayRate, minimum);
        }

        constexpr auto decay_rate() const noexcept { return m_DecayRate; }
        constexpr auto minimum   () const noexcept { return m_Minimum; }
        constexpr auto have_peak () const noexcept { return m_DecayRate > 0; }

    private:
        peak_type m_Minimum  { 0 };
        peak_type m_DecayRate{ 0 };
    }; // template <...> class InterpolatedPeakDataT

    /**************************************************************************
     * *
     **************************************************************************/
    template <typename DecibelTypeT>
    class InterpolatedChannelDataT final {
    private:
        using this_type = dB::InterpolatedChannelDataT<DecibelTypeT>;

    public:
        using channel_type      = dB::InterpolatedDataT<DecibelTypeT>;
        using channel_peak_type = dB::InterpolatedPeakDataT<DecibelTypeT>;
        using dB_type           = typename channel_type::dB_type;
        using decibel_type      = typename channel_type::decibel_type;
        using peak_type         = typename channel_peak_type::peak_type;

    public:
        InterpolatedChannelDataT() = default;

        InterpolatedChannelDataT(dB_type dB,
                                 peak_type peakDecay,
                                 peak_type peakMinimum) :
            m_dB  { dB },
            m_Peak{ dB, peakDecay, peakMinimum } {}

        InterpolatedChannelDataT(dB_type prev,
                                 dB_type next,
                                 peak_type peakDecay,
                                 peak_type peakMinimum) :
            m_dB  { prev, next },
            m_Peak{ prev, next, peakDecay, peakMinimum } {}

        constexpr InterpolatedChannelDataT(const this_type& ) noexcept = default;
        constexpr InterpolatedChannelDataT(      this_type&&) noexcept = default;

        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

    public:
        void swap(this_type& other) noexcept {
            using std::swap;
            swap(m_dB,   other.m_dB);
            swap(m_Peak, other.m_Peak);
        }

        constexpr void dB_clear     () noexcept { m_dB.clear(); }
        constexpr void peak_clear   () noexcept { m_Peak.clear(); }
        constexpr void clear        () noexcept { dB_clear(); peak_clear(); }

        constexpr void dB_reset     () noexcept { m_dB.reset(); }
        constexpr void peak_reset   () noexcept { m_Peak.reset(); }
        constexpr void reset        () noexcept { dB_reset(); peak_reset(); }

        constexpr void dB_zero      () noexcept { m_dB.zero(); }
        constexpr void peak_zero    () noexcept { m_Peak.zero(); }
        constexpr void zero         () noexcept { dB_zero(); peak_zero(); }

        constexpr void dB_update    (dB_type dB) noexcept { m_dB.update(dB); }
        constexpr void peak_update  (dB_type dB) noexcept { m_Peak.update(dB); }
        constexpr void update       (dB_type dB) noexcept { dB_update(dB); peak_update(dB); }

        void peak_decay_rate(peak_type decay,
                             peak_type minimum) noexcept {
            m_Peak.decay_rate(decay, minimum);
        }

        void peak_decay_rate(peak_type decay) noexcept {
            m_Peak.decay_rate(decay);
        }

        void peak_minimum(peak_type minimum) noexcept {
            m_Peak.peak_minimum(minimum);
        }

    public:
        decltype(auto) dB_prev          ()                  const noexcept { return m_dB.prev(); }
        decltype(auto) dB_next          ()                        noexcept { return m_dB.next(); }
        decltype(auto) dB_next          ()                  const noexcept { return m_dB.next(); }
        decltype(auto) dB_curr          (dB_type interp)    const noexcept { return m_dB.curr(interp); }

        decltype(auto) peak_prev        ()                  const noexcept { return m_Peak.prev(); }
        decltype(auto) peak_next        ()                        noexcept { return m_Peak.next(); }
        decltype(auto) peak_next        ()                  const noexcept { return m_Peak.next(); }
        decltype(auto) peak_curr        (dB_type interp)    const noexcept { return m_Peak.curr(interp); }

        decltype(auto) peak_decay_rate  ()                  const noexcept { return m_Peak.decay_rate(); }
        decltype(auto) peak_minimum     ()                  const noexcept { return m_Peak.minimum(); }

        decltype(auto) have_peak        ()                  const noexcept { return m_Peak.have_peak(); }

    private:
        channel_type      m_dB  {};
        channel_peak_type m_Peak{};
    }; // template <...> class InterpolatedChannelDataT final
} // namespace Audio::dB

#endif // GUID_2926D6AE_BCF7_4C63_A96C_1879B2C84D1D