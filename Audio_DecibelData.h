#pragma once
#ifndef GUID_8F0A5650_B15C_4A26_842A_F32A63553FDD
#define GUID_8F0A5650_B15C_4A26_842A_F32A63553FDD
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
#include "Audio_DecibelData_Impl.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <array>
//--------------------------------------

namespace Audio::dB {
    /**********************************************************************
     * InterpolatedChannelsT *
     **********************************************************************/
    template <typename DecibelTypeT, std::size_t ChannelCountT>
    class InterpolatedChannelsT final {
    private:
        using this_type = dB::InterpolatedChannelsT<DecibelTypeT, ChannelCountT>;

    public:
        using channel_data_type   = dB::InterpolatedChannelDataT<DecibelTypeT>;
        using channel_buffer_type = std::array<channel_data_type, ChannelCountT>;
        using dB_type             = typename channel_data_type::dB_type;
        using decibel_type        = typename channel_data_type::decibel_type;
        using peak_type           = typename channel_data_type::peak_type;
        using size_type           = typename channel_buffer_type::size_type;

    public:
        inline static constexpr const auto ChannelCount{ ChannelCountT };

    public:
        constexpr InterpolatedChannelsT() noexcept = default;

        constexpr InterpolatedChannelsT(const this_type& ) noexcept = default;
        constexpr InterpolatedChannelsT(      this_type&&) noexcept = default;

        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

    public:
        constexpr decltype(auto) begin  ()       noexcept { assert(valid()); return m_Channels.begin(); }
        constexpr decltype(auto) begin  () const noexcept { assert(valid()); return m_Channels.begin(); }
        constexpr decltype(auto) cbegin () const noexcept { assert(valid()); return m_Channels.cbegin(); }

        constexpr decltype(auto) end    ()       noexcept { assert(valid()); return m_Channels.end(); }
        constexpr decltype(auto) end    () const noexcept { assert(valid()); return m_Channels.end(); }
        constexpr decltype(auto) cend   () const noexcept { assert(valid()); return m_Channels.cend(); }

        constexpr decltype(auto) rbegin ()       noexcept { assert(valid()); return m_Channels.rbegin(); }
        constexpr decltype(auto) rbegin () const noexcept { assert(valid()); return m_Channels.rbegin(); }
        constexpr decltype(auto) crbegin() const noexcept { assert(valid()); return m_Channels.crbegin(); }

        constexpr decltype(auto) rend   ()       noexcept { assert(valid()); return m_Channels.rend(); }
        constexpr decltype(auto) rend   () const noexcept { assert(valid()); return m_Channels.rend(); }
        constexpr decltype(auto) crend  () const noexcept { assert(valid()); return m_Channels.crend(); }

        constexpr decltype(auto) front  ()       noexcept { assert(valid()); return m_Channels.front(); }
        constexpr decltype(auto) front  () const noexcept { assert(valid()); return m_Channels.front(); }
        constexpr decltype(auto) back   ()       noexcept { assert(valid()); return m_Channels.back(); }
        constexpr decltype(auto) back   () const noexcept { assert(valid()); return m_Channels.back(); }

        constexpr decltype(auto) data   ()       noexcept { assert(valid()); return m_Channels.data(); }
        constexpr decltype(auto) data   () const noexcept { assert(valid()); return m_Channels.data(); }

        constexpr decltype(auto) size   () const noexcept { assert(valid()); return m_Channels.size(); }

        void swap(this_type& other) noexcept {
            assert(valid()); assert(other.valid());
            using std::swap;
            swap(m_Channels, other.m_Channels);
        }

    public:
        constexpr void clear() noexcept { assert(valid()); for (auto& c : m_Channels) { c.clear(); } }
        constexpr void reset() noexcept { assert(valid()); for (auto& c : m_Channels) { c.reset(); } }
        constexpr void zero () noexcept { assert(valid()); for (auto& c : m_Channels) { c.zero(); } }

        constexpr void clear(size_type ch) noexcept { assert(valid()); m_Channels[ch].clear(); }
        constexpr void reset(size_type ch) noexcept { assert(valid()); m_Channels[ch].reset(); }
        constexpr void zero (size_type ch) noexcept { assert(valid()); m_Channels[ch].zero(); }

    public:
        [[nodiscard]]
        constexpr decltype(auto) channel_count() const noexcept {
            assert(valid());
            return m_Channels.size();
        }

        [[nodiscard]]
        decltype(auto) empty() const noexcept {
            assert(valid());
            return m_Channels[0].empty();
        }

    public:
        [[nodiscard]]
        decltype(auto) dB_prev(size_type ch)    const noexcept { assert(valid()); return m_Channels[ch].dB_prev(); }
        [[nodiscard]]
        decltype(auto) dB_next(size_type ch)          noexcept { assert(valid()); return m_Channels[ch].dB_next(); }
        [[nodiscard]]
        decltype(auto) dB_next(size_type ch)    const noexcept { assert(valid()); return m_Channels[ch].dB_next(); }
        [[nodiscard]]
        decltype(auto) dB_curr(size_type ch,
                               dB_type interp)  const noexcept { assert(valid());return m_Channels[ch].dB_curr(interp); }

    public:
        [[nodiscard]]
        decltype(auto) peak_prev(size_type ch)      const noexcept { assert(valid()); return m_Channels[ch].peak_prev(); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type ch)            noexcept { assert(valid()); return m_Channels[ch].peak_next(); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type ch)      const noexcept { assert(valid()); return m_Channels[ch].peak_next(); }
        [[nodiscard]]
        decltype(auto) peak_curr(size_type ch,
                                 peak_type interp)  const noexcept { assert(valid());return m_Channels[ch].peak_curr(interp); }

    public:
        void peak_decay_rate(peak_type decay,
                             peak_type minimum) noexcept {
            assert(valid());
            for (auto& c : m_Channels) {
                c.peak_decay_rate(decay, minimum);
            }
        }

        void peak_decay_rate(peak_type decay) noexcept {
            assert(valid());
            for (auto& c : m_Channels) {
                c.peak_decay_rate(decay);
            }
        }

        void peak_minimum(peak_type minimum) noexcept {
            assert(valid());
            for (auto& c : m_Channels) {
                c.peak_minimum(minimum);
            }
        }

        [[nodiscard]]
        constexpr decltype(auto) peak_decay_rate() const noexcept {
            assert(valid());
            return m_Channels[0].peak_decay_rate();
        }

        [[nodiscard]]
        constexpr decltype(auto) peak_minimum() const noexcept {
            assert(valid());
            return m_Channels[0].peak_minimum();
        }

        [[nodiscard]]
        decltype(auto) have_peaks() const noexcept {
            assert(valid());
            return m_Channels[0].have_peak();
        }

    public:
        [[nodiscard]]
        constexpr       auto& channels()       noexcept { assert(valid()); return m_Channels; }
        [[nodiscard]]
        constexpr const auto& channels() const noexcept { assert(valid()); return m_Channels; }

        [[nodiscard]]
        constexpr decltype(auto) at(size_type pos)       { assert(valid()); return m_Channels.at(pos); }
        [[nodiscard]]
        constexpr decltype(auto) at(size_type pos) const { assert(valid()); return m_Channels.at(pos); }

        [[nodiscard]]
        constexpr decltype(auto) operator[](size_type pos)       noexcept { assert(valid()); return m_Channels[pos]; }
        [[nodiscard]]
        constexpr decltype(auto) operator[](size_type pos) const noexcept { assert(valid()); return m_Channels[pos]; }

    public:
        constexpr void update(dB_type dB) noexcept {
            assert(valid());
            for (auto& c : m_Channels) { c.update(dB); }
        }

        constexpr void update(size_type ch,
                              dB_type dB) noexcept {
            assert(valid());
            m_Channels[ch].update(dB);
        }

    public:
        constexpr operator const channel_buffer_type& () const noexcept { return channels(); }
        constexpr operator       channel_buffer_type& ()       noexcept { return channels(); }

    private:
        bool valid() const noexcept {
            const auto peak_decay_rate{ m_Channels[0].peak_decay_rate() };
            const auto peak_minimum   { m_Channels[0].peak_minimum() };
            for (const auto& c : m_Channels) {
                if (peak_decay_rate != c.peak_decay_rate()) { return false; }
                if (peak_minimum    != c.peak_minimum())    { return false; }
            }
            return true;
        }

    private:
        channel_buffer_type m_Channels;
    };

    /**********************************************************************
     * DecibelDataT *
     **********************************************************************/
    template <typename DecibelTypeT, std::size_t ChannelCountT>
    class DecibelDataT final {
    private:
        using this_type = dB::DecibelDataT<DecibelTypeT, ChannelCountT>;

    public:
        using channels_type      = dB::InterpolatedChannelsT<DecibelTypeT, ChannelCountT>;
        using combined_type      = dB::InterpolatedChannelDataT<DecibelTypeT>;
        using channel_data_type  = typename channels_type::channel_data_type;
        using channel_buffer_type = typename channels_type::channel_buffer_type;
        using dB_type            = typename channels_type::dB_type;
        using decibel_type       = typename channels_type::decibel_type;
        using size_type          = typename channels_type::size_type;
        using peak_type          = typename channels_type::peak_type;

    public:
        inline static constexpr const auto ChannelCount{ ChannelCountT };

    public:
        constexpr DecibelDataT() noexcept = default;

        constexpr DecibelDataT(const this_type& ) noexcept = default;
        constexpr DecibelDataT(      this_type&&) noexcept = default;

        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

    public:
        constexpr void channels_clear   ()              noexcept { m_Channels.clear(); }
        constexpr void channels_reset   ()              noexcept { m_Channels.reset(); }
        constexpr void channels_zero    ()              noexcept { m_Channels.zero(); }

        constexpr void channel_clear    (size_type ch)  noexcept { m_Channels.clear(ch); }
        constexpr void channel_reset    (size_type ch)  noexcept { m_Channels.reset(ch); }
        constexpr void channel_zero     (size_type ch)  noexcept { m_Channels.zero(ch); }

        constexpr void combined_clear   ()              noexcept { m_Combined.clear(); }
        constexpr void combined_reset   ()              noexcept { m_Combined.reset(); }
        constexpr void combined_zero    ()              noexcept { m_Combined.zero(); }

        constexpr void clear            ()              noexcept { channels_clear(); combined_clear(); }
        constexpr void reset            ()              noexcept { channels_reset(); combined_reset(); }
        constexpr void zero             ()              noexcept { channels_zero();  combined_zero(); }

        void peak_decay_rate(peak_type decay,
                             peak_type minimum) noexcept {
            m_Channels.peak_decay_rate(decay, minimum);
            m_Combined.peak_decay_rate(decay, minimum);
        }

        void peak_decay_rate(peak_type decay) noexcept {
            m_Channels.peak_decay_rate(decay);
            m_Combined.peak_decay_rate(decay);
        }

        void peak_minimum(peak_type minimum) noexcept {
            m_Channels.peak_minimum(minimum);
            m_Combined.peak_minimum(minimum);
        }

        void channel_update(size_type ch, dB_type dB) noexcept {
            m_Channels.update(ch, dB);
        }

        void combined_update(dB_type dB) noexcept {
            m_Combined.update(dB);
        }

    public:
              auto&     combined_data           ()                        noexcept { return m_Combined; }
        const auto&     combined_data           ()                  const noexcept { return m_Combined; }

        decltype(auto)  combined                ()                        noexcept { return combined_data(); }
        decltype(auto)  combined                ()                  const noexcept { return combined_data(); }

        decltype(auto)  combined_dB_prev        ()                  const noexcept { return m_Combined.dB_prev(); }
        decltype(auto)  combined_dB_next        ()                        noexcept { return m_Combined.dB_next(); }
        decltype(auto)  combined_dB_next        ()                  const noexcept { return m_Combined.dB_next(); }
        decltype(auto)  combined_dB_curr        (dB_type interp)    const noexcept { return m_Combined.dB_curr(interp); }

        decltype(auto)  combined_peak_prev      ()                  const noexcept { return m_Combined.peak_prev(); }
        decltype(auto)  combined_peak_next      ()                        noexcept { return m_Combined.peak_next(); }
        decltype(auto)  combined_peak_next      ()                  const noexcept { return m_Combined.peak_next(); }
        decltype(auto)  combined_peak_curr      (dB_type interp)    const noexcept { return m_Combined.peak_curr(interp); }

    public:
              auto&     channel_data            ()                        noexcept { return m_Channels; }
        const auto&     channel_data            ()                  const noexcept { return m_Channels; }

              auto&     channel                 (size_type ch)            noexcept { return m_Channels[ch]; }
        const auto&     channel                 (size_type ch)      const noexcept { return m_Channels[ch]; }

        decltype(auto)  channels                ()                        noexcept { return m_Channels.channels(); }
        decltype(auto)  channels                ()                  const noexcept { return m_Channels.channels(); }

        decltype(auto)  channel_dB_prev         (size_type ch)      const noexcept { return channel(ch).dB_prev(); }
        decltype(auto)  channel_dB_next         (size_type ch)            noexcept { return channel(ch).dB_next(); }
        decltype(auto)  channel_dB_next         (size_type ch)      const noexcept { return channel(ch).dB_next(); }
        decltype(auto)  channel_dB_curr         (size_type ch,
                                                 dB_type interp)    const noexcept { return channel(ch).dB_curr(interp); }

        decltype(auto)  channel_peak_prev       (size_type ch)      const noexcept { return channel(ch).peak_prev(); }
        decltype(auto)  channel_peak_next       (size_type ch)            noexcept { return channel(ch).peak_next(); }
        decltype(auto)  channel_peak_next       (size_type ch)      const noexcept { return channel(ch).peak_next(); }
        decltype(auto)  channel_peak_curr       (size_type ch,
                                                 peak_type interp)  const noexcept { return channel(ch).peak_curr(interp); }

        decltype(auto)  channel_count           ()                  const noexcept { return m_Channels.channel_count(); }

        decltype(auto)  peak_decay_rate         ()                  const noexcept { assert(valid()); return m_Channels.peak_decay_rate(); }
        decltype(auto)  peak_minimum            ()                  const noexcept { assert(valid()); return m_Channels.peak_minimum(); }

        decltype(auto)  have_peaks              ()                  const noexcept { assert(valid()); return m_Channels.have_peaks(); }

    public:
        constexpr operator const channels_type&       () const noexcept { return channel_data(); }
        constexpr operator       channels_type&       ()       noexcept { return channel_data(); }

        constexpr operator const combined_type&       () const noexcept { return combined_data(); }
        constexpr operator       combined_type&       ()       noexcept { return combined_data(); }

        constexpr operator const channel_buffer_type& () const noexcept { return channels(); }
        constexpr operator       channel_buffer_type& ()       noexcept { return channels(); }

    private:
        bool valid() const noexcept {
            if (m_Channels.have_peaks()      != m_Combined.have_peak())       { return false; }
            if (m_Channels.peak_decay_rate() != m_Combined.peak_decay_rate()) { return false; }
            if (m_Channels.peak_minimum()    != m_Combined.peak_minimum())    { return false; }
            return true;
        }

    private:
        channels_type m_Channels{};
        combined_type m_Combined{};
    }; // template <...> class DecibelDataT final
} // namespace Audio::dB

#endif // GUID_8F0A5650_B15C_4A26_842A_F32A63553FDD