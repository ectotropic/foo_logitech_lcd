#pragma once
#ifndef GUID_EA5F7AE9_EF33_492F_A3F4_80928F8B6D7D
#define GUID_EA5F7AE9_EF33_492F_A3F4_80928F8B6D7D
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
#include "Audio_SampleData_Impl.h"
//--------------------------------------

namespace Audio::Samples {
    /**********************************************************************
     * InterpolatedChannelsT *
     **********************************************************************/
    template <
        typename SampleTypeT,
        std::size_t ChannelCountT,
        class AllocatorT = std::allocator<SampleTypeT>
    >
    class InterpolatedChannelsT final {
    private:
        using this_type = Samples::InterpolatedChannelsT<SampleTypeT, ChannelCountT, AllocatorT>;

    public:
        using channel_data_type      = Samples::InterpolatedChannelDataT<SampleTypeT, AllocatorT>;
        using channel_buffer_type    = std::array<channel_data_type, ChannelCountT>;
        using sample_type            = typename channel_data_type::sample_type;
        using sample_buffer_type     = typename channel_data_type::sample_buffer_type;
        using sample_span_type       = typename channel_data_type::sample_span_type;
        using sample_const_span_type = typename channel_data_type::sample_const_span_type;
        using samples_type           = typename channel_data_type::samples_type;
        using peak_type              = typename channel_data_type::peak_type;
        using peak_buffer_type       = typename channel_data_type::peak_buffer_type;
        using peaks_type             = typename channel_data_type::peaks_type;
        using peak_type              = typename channel_data_type::peak_type;
        using peak_span_type         = typename channel_data_type::peak_span_type;
        using peak_const_span_type   = typename channel_data_type::peak_const_span_type;
        using size_type              = typename channel_data_type::size_type;

    public:
        inline static constexpr const auto ChannelCount{ ChannelCountT };

    public:
        InterpolatedChannelsT()                   noexcept  = default;
        InterpolatedChannelsT(const this_type& )            = default;
        InterpolatedChannelsT(      this_type&&)  noexcept  = default;

        this_type& operator=(const this_type& )             = default;
        this_type& operator=(      this_type&&)   noexcept  = default;

    public:
        InterpolatedChannelsT(size_type num,
                              peak_type decay,
                              peak_type minimum) {
            channel_value_count(num);
            peak_decay_rate(decay, minimum);
        }

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

        constexpr decltype(auto) front  ()       noexcept { assert(valid());  return m_Channels.front(); }
        constexpr decltype(auto) front  () const noexcept { assert(valid()); return m_Channels.front(); }
        constexpr decltype(auto) back   ()       noexcept { assert(valid()); return m_Channels.back(); }
        constexpr decltype(auto) back   () const noexcept { assert(valid()); return m_Channels.back(); }

        constexpr decltype(auto) data   ()       noexcept { assert(valid()); return m_Channels.data(); }
        constexpr decltype(auto) data   () const noexcept { assert(valid()); return m_Channels.data(); }

        constexpr decltype(auto) size   () const noexcept { assert(valid()); return m_Channels.size(); }

    public:
        void swap(this_type& other) noexcept {
            assert(valid()); assert(other.valid());
            using std::swap;
            swap(m_Channels, other.m_Channels);
        }

        void clear        () noexcept { assert(valid()); for (auto& c : m_Channels) { c.clear(); } }
        void shrink_to_fit()          { assert(valid()); for (auto& c : m_Channels) { c.shrink_to_fit(); } }
        void reset        ()          { assert(valid()); for (auto& c : m_Channels) { c.reset(); } }
        void zero_samples () noexcept { assert(valid()); for (auto& c : m_Channels) { c.zero_samples(); } }
        void zero_peaks   () noexcept { assert(valid()); for (auto& c : m_Channels) { c.zero_peaks(); } }
        void zero         () noexcept { assert(valid()); for (auto& c : m_Channels) { c.zero(); } }

        void clear        (size_type ch) noexcept { assert(valid()); m_Channels[ch].clear(); }
        void shrink_to_fit(size_type ch)          { assert(valid()); m_Channels[ch].shrink_to_fit(); }
        void reset        (size_type ch)          { assert(valid()); m_Channels[ch].reset(); }
        void zero_samples (size_type ch) noexcept { assert(valid()); m_Channels[ch].zero_samples(); }
        void zero_peaks   (size_type ch) noexcept { assert(valid()); m_Channels[ch].zero_peaks(); }
        void zero         (size_type ch) noexcept { assert(valid()); m_Channels[ch].zero(); }

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

        //--------------------------------------------------
        // `channel_value_count`:
        // ----------------------
        //
        // Want to ensure that all channels **always**
        // have the same size, even on error.
        //
        // Simplest solution is to do the reallocation
        // in temporary variables, as is done here. This
        // should be no different in performance than
        // simply resizing the buffers, but ensures both
        // buffers maintain the same size at all times.
        //
        // Unfortunately if an error does occur the
        // buffers will be left empty, but avoiding this
        // requires more complex code which is likely
        // overkill for a worst case scenario which is
        // likely to be fatal anyway.
        //
        // NOTE: Assumes `move` is `noexcept`
        [[nodiscard]]
        decltype(auto) channel_value_count() const noexcept {
            assert(valid());
            return m_Channels[0].count();
        }

        void channel_value_count(size_type num) {
            assert(valid());
            if (num == channel_value_count()) { return; }
            auto temp{ std::move(m_Channels) };
            for (auto& c : temp) { c.count(num); }
            m_Channels = std::move(temp);
            assert(valid());
        }
        //--------------------------------------------------

    public:
        [[nodiscard]]
        decltype(auto) data_for_update(size_type ch) noexcept {
            assert(valid());
            return m_Channels[ch].data_for_update();
        }

        void update(size_type ch, const sample_buffer_type& samples) {
            assert(valid());
            m_Channels[ch].update(samples);
        }

        void update(size_type ch, sample_buffer_type&& samples) {
            assert(valid());
            m_Channels[ch].update(std::forward<sample_buffer_type>(samples));
        }

        void update(size_type ch,
                    sample_const_span_type samples) noexcept {
            assert(valid());
            m_Channels[ch].update(samples);
        }

    public:
        [[nodiscard]]
        decltype(auto) samples_prev(size_type ch) const noexcept { assert(valid()); return m_Channels[ch].samples_prev(); }
        [[nodiscard]]
        decltype(auto) samples_next(size_type ch)       noexcept { assert(valid()); return m_Channels[ch].samples_next(); }
        [[nodiscard]]
        decltype(auto) samples_next(size_type ch) const noexcept { assert(valid()); return m_Channels[ch].samples_next(); }

        [[nodiscard]]
        decltype(auto) sample_prev(size_type ch,
                                   size_type pos) const noexcept { assert(valid()); return m_Channels[ch].sample_prev(pos); }
        [[nodiscard]]
        decltype(auto) sample_next(size_type ch,
                                   size_type pos)       noexcept { assert(valid()); return m_Channels[ch].sample_next(pos); }
        [[nodiscard]]
        decltype(auto) sample_next(size_type ch,
                                   size_type pos) const noexcept { assert(valid()); return m_Channels[ch].sample_next(pos); }

        [[nodiscard]]
        decltype(auto) samples_curr(size_type ch,
                                    sample_type interp) const {
            assert(valid());
            return m_Channels[ch].samples_curr(interp);
        }

        [[nodiscard]]
        decltype(auto) sample_curr(size_type ch,
                                   size_type pos,
                                   sample_type interp) const noexcept {
            assert(valid());
            return m_Channels[ch].sample_curr(pos, interp);
        }

        [[nodiscard]]
        decltype(auto) sample_curr_at(size_type ch,
                                      size_type pos,
                                      sample_type interp) const noexcept {
            assert(valid());
            return m_Channels[ch].sample_curr_at(pos, interp);
        }

    public:
        [[nodiscard]]
        decltype(auto) peaks_prev(size_type ch) const noexcept { assert(valid()); return m_Channels[ch].peaks_prev(); }
        [[nodiscard]]
        decltype(auto) peaks_next(size_type ch)       noexcept { assert(valid()); return m_Channels[ch].peaks_next(); }
        [[nodiscard]]
        decltype(auto) peaks_next(size_type ch) const noexcept { assert(valid()); return m_Channels[ch].peaks_next(); }

        [[nodiscard]]
        decltype(auto) peak_prev(size_type ch,
                                 size_type pos) const noexcept { assert(valid()); return m_Channels[ch].peak_prev(pos); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type ch,
                                 size_type pos)       noexcept { assert(valid()); return m_Channels[ch].peak_next(pos); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type ch,
                                 size_type pos) const noexcept { assert(valid()); return m_Channels[ch].peak_next(pos); }

        [[nodiscard]]
        decltype(auto) peaks_curr(size_type ch,
                                  peak_type interp) const {
            assert(valid());
            return m_Channels[ch].peaks_curr(interp);
        }

        [[nodiscard]]
        decltype(auto) peak_curr(size_type ch,
                                 size_type pos,
                                 peak_type interp) const noexcept {
            assert(valid());
            return m_Channels[ch].peak_curr(pos, interp);
        }

        [[nodiscard]]
        decltype(auto) peak_curr_at(size_type ch,
                                    size_type pos,
                                    peak_type interp) const noexcept {
            assert(valid());
            return m_Channels[ch].peak_curr_at(pos, interp);
        }

    public:
        void peak_decay_rate(peak_type decay,
                             peak_type minimum) noexcept {
            assert(valid());
            for (auto& c : m_Channels) { c.peak_decay_rate(decay, minimum); }
        }

        void peak_decay_rate(peak_type decay) {
            assert(valid());
            for (auto& c : m_Channels) { c.peak_decay_rate(decay); }
        }

        void peak_minimum(peak_type minimum) {
            assert(valid());
            for (auto& c : m_Channels) { c.peak_minimum(decay); }
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
        constexpr decltype(auto) have_peaks() const noexcept {
            assert(valid());
            return m_Channels[0].have_peaks();
        }

    public:
        [[nodiscard]]
              auto& channels()       noexcept { assert(valid()); return m_Channels; }
        [[nodiscard]]
        const auto& channels() const noexcept { assert(valid()); return m_Channels; }

        [[nodiscard]]
        decltype(auto) at(size_type pos)       { assert(valid()); return m_Channels.at(pos); }
        [[nodiscard]]
        decltype(auto) at(size_type pos) const { assert(valid()); return m_Channels.at(pos); }

        [[nodiscard]]
        decltype(auto) operator[](size_type pos)       noexcept { assert(valid()); return m_Channels[pos]; }
        [[nodiscard]]
        decltype(auto) operator[](size_type pos) const noexcept { assert(valid()); return m_Channels[pos]; }

    public:
        constexpr operator const channel_buffer_type& () const noexcept { return m_Channels; }
        constexpr operator       channel_buffer_type& ()       noexcept { return m_Channels; }

    private:
        [[nodiscard]]
        bool valid() const noexcept {
            const auto num_samples = m_Channels[0].count();
            for (const auto& c : m_Channels) {
                if (num_samples != c.count()) { return false; }
            }
            return true;
        }

    private:
        channel_buffer_type m_Channels;
    };

    /**********************************************************************
     * SampleDataT *
     **********************************************************************/
    template <
        typename SampleTypeT,
        std::size_t ChannelCountT,
        class AllocatorT = std::allocator<SampleTypeT>
    >
    class SampleDataT final {
    private:
        using this_type = Samples::SampleDataT<SampleTypeT, ChannelCountT, AllocatorT>;

    public:
        using channels_type          = Samples::InterpolatedChannelsT<SampleTypeT, ChannelCountT, AllocatorT>;
        using combined_type          = Samples::InterpolatedChannelDataT<SampleTypeT, AllocatorT>;
        using channel_data_type      = typename channels_type::channel_data_type;
        using channel_buffer_type    = typename channels_type::channel_buffer_type;
        using sample_type            = typename channels_type::sample_type;
        using sample_buffer_type     = typename channels_type::sample_buffer_type;
        using sample_span_type       = typename channels_type::sample_span_type;
        using sample_const_span_type = typename channels_type::sample_const_span_type;
        using samples_type           = typename channels_type::samples_type;
        using peak_type              = typename channels_type::peak_type;
        using peak_buffer_type       = typename channels_type::peak_buffer_type;
        using peaks_type             = typename channels_type::peaks_type;
        using peak_type              = typename channels_type::peak_type;
        using peak_span_type         = typename channels_type::peak_span_type;
        using peak_const_span_type   = typename channels_type::peak_const_span_type;
        using size_type              = typename channels_type::size_type;

    public:
        inline static constexpr const auto ChannelCount{ ChannelCountT };

    public:
        constexpr SampleDataT() noexcept = default;

        constexpr SampleDataT(const this_type& ) noexcept = default;
        constexpr SampleDataT(      this_type&&) noexcept = default;

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

        [[nodiscard]]
        decltype(auto) value_count() const noexcept {
            assert(valid());
            return m_Channels[0].count();
        }

        void value_count(size_type num) {
            assert(valid());
            if (num == value_count()) { return; }
            auto channels{ std::move(m_Channels) };
            auto combined{ std::move(m_Combined) };
            channels.channel_value_count(num);
            combined.count(num);
            m_Channels = std::move(channels);
            m_Combined = std::move(combined);
            assert(valid());
        }

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

    public:
        [[nodiscard]]
        decltype(auto) channel_data_for_update(size_type ch) noexcept {
            assert(valid());
            return m_Channels[ch].data_for_update();
        }

        void channel_update(size_type ch,
                            const sample_buffer_type& samples) {
            assert(valid());
            m_Channels[ch].update(samples);
        }

        void channel_update(size_type ch,
                            sample_buffer_type&& samples) {
            assert(valid());
            m_Channels[ch].update(std::forward<sample_buffer_type>(samples));
        }

        void channel_update(size_type ch,
                            sample_const_span_type samples) noexcept {
            assert(valid());
            m_Channels[ch].update(samples);
        }

        [[nodiscard]]
        decltype(auto) combined_data_for_update() noexcept {
            assert(valid());
            return m_Combined.data_for_update();
        }

        void combined_update(const sample_buffer_type& samples) {
            assert(valid());
            m_Combined.update(samples);
        }

        void combined_update(sample_buffer_type&& samples) noexcept {
            assert(valid());
            m_Combined.update(std::forward<sample_buffer_type>(samples));
        }

        void combined_update(sample_const_span_type samples) noexcept {
            assert(valid());
            m_Combined.update(samples);
        }

    public:
              auto&     combined_data           ()                        noexcept { return m_Combined; }
        const auto&     combined_data           ()                  const noexcept { return m_Combined; }

        decltype(auto)  combined                ()                        noexcept { return combined_data(); }
        decltype(auto)  combined                ()                  const noexcept { return combined_data(); }

        decltype(auto)  combined_samples_prev    ()                  const noexcept { return m_Combined.samples_prev(); }
        decltype(auto)  combined_samples_next    ()                        noexcept { return m_Combined.samples_next(); }
        decltype(auto)  combined_samples_next    ()                  const noexcept { return m_Combined.samples_next(); }
        decltype(auto)  combined_samples_curr    (sample_type interp)    const { return m_Combined.samples_curr(interp); }

        decltype(auto)  combined_peaks_prev      ()                  const noexcept { return m_Combined.peaks_prev(); }
        decltype(auto)  combined_peaks_next      ()                        noexcept { return m_Combined.peaks_next(); }
        decltype(auto)  combined_peaks_next      ()                  const noexcept { return m_Combined.peaks_next(); }
        decltype(auto)  combined_peaks_curr      (sample_type interp)    const { return m_Combined.peaks_curr(interp); }

    public:
              auto&     channel_data            ()                        noexcept { return m_Channels; }
        const auto&     channel_data            ()                  const noexcept { return m_Channels; }

              auto&     channel                 (size_type ch)            noexcept { return m_Channels[ch]; }
        const auto&     channel                 (size_type ch)      const noexcept { return m_Channels[ch]; }

        decltype(auto)  channels                ()                        noexcept { return m_Channels.channels(); }
        decltype(auto)  channels                ()                  const noexcept { return m_Channels.channels(); }

        decltype(auto)  channel_samples_prev     (size_type ch)      const noexcept { return channel(ch).samples_prev(); }
        decltype(auto)  channel_samples_next     (size_type ch)            noexcept { return channel(ch).samples_next(); }
        decltype(auto)  channel_samples_next     (size_type ch)      const noexcept { return channel(ch).samples_next(); }
        decltype(auto)  channel_samples_curr     (size_type ch,
                                                  sample_type interp) const         { return channel(ch).samples_curr(interp); }

        decltype(auto)  channel_peaks_prev       (size_type ch)      const noexcept { return channel(ch).peaks_prev(); }
        decltype(auto)  channel_peaks_next       (size_type ch)            noexcept { return channel(ch).peaks_next(); }
        decltype(auto)  channel_peaks_next       (size_type ch)      const noexcept { return channel(ch).peaks_next(); }
        decltype(auto)  channel_peaks_curr       (size_type ch,
                                                  peak_type interp)  const          { return channel(ch).peaks_curr(interp); }

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
            if (m_Channels.have_peaks()      != m_Combined.have_peaks())       { return false; }
            if (m_Channels.peak_decay_rate() != m_Combined.peak_decay_rate()) { return false; }
            if (m_Channels.peak_minimum()    != m_Combined.peak_minimum())    { return false; }
            return true;
        }

    private:
        channels_type m_Channels{};
        combined_type m_Combined{};
    }; // template <...> class SampleDataT final
} // namespace Audio::Samples

#endif // GUID_EA5F7AE9_EF33_492F_A3F4_80928F8B6D7D