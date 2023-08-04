#pragma once
#ifndef GUID_4E75BAEC_10B8_49D8_8EF1_15CB921E0A6D
#define GUID_4E75BAEC_10B8_49D8_8EF1_15CB921E0A6D
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
#include "Util/InterpolateUtil.h"
//--------------------------------------

namespace Audio::Samples {
    /**********************************************************************
     * InterpolatedDataT
     **********************************************************************/
    template <
        typename SampleTypeT,
        class AllocatorT = std::allocator<SampleTypeT>
    >
    class InterpolatedDataT :
        public ::util::interpolated_buffer<SampleTypeT, AllocatorT> {
    private:
        using base_type = ::util::interpolated_buffer<SampleTypeT, AllocatorT>;
        using this_type = InterpolatedDataT<SampleTypeT, AllocatorT>;

    public:
        using sample_type        = typename base_type::value_type;
        using sample_buffer_type = typename buffer_type;

    public:
        using base_type::base_type;

    private:
        using base_type::size;
        using base_type::total_size;
        using base_type::capacity;
        using base_type::total_capacity;
        using base_type::resize;
        using base_type::reserve;

    public:
        void            count(size_type num)                { base_type::resize(num); }
        [[nodiscard]]
        decltype(auto)  count()              const noexcept { return base_type::size(); }
    }; // template <...> class InterpolatedBufferT

    /**********************************************************************
     * InterpolatedPeakDataT
     **********************************************************************/
    template <
        typename PeakTypeT,
        class AllocatorT = std::allocator<PeakTypeT>
    >
    class InterpolatedPeakDataT final :
        public ::util::interpolated_buffer<PeakTypeT, AllocatorT> {
    private:
        using base_type = ::util::interpolated_buffer<PeakTypeT, AllocatorT>;
        using this_type = InterpolatedPeakDataT<PeakTypeT, AllocatorT>;

    public:
        using peak_buffer_type   = typename base_type::buffer_type;
        using peak_type          = typename base_type::value_type;
        using sample_buffer_type = typename base_type::buffer_type;
        using sample_type        = typename base_type::value_type;

    public:
        InterpolatedPeakDataT()                  noexcept = default;
        InterpolatedPeakDataT(const this_type& )          = default;
        InterpolatedPeakDataT(      this_type&&) noexcept = default;

        this_type& operator=(const this_type& )          = default;
        this_type& operator=(      this_type&&) noexcept = default;

    public:
        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum) :
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum,
                                 const sample_buffer_type& prev,
                                 const sample_buffer_type& next) :
            base_type  { ((decay > 0) ? prev : samples_type{}),
                         ((decay > 0) ? next : samples_type{}) },
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum,
                                 sample_buffer_type&& prev,
                                 sample_buffer_type&& next) noexcept :
            base_type  { ((decay > 0) ? std::forward<sample_buffer_type>(prev) : samples_type{}),
                         ((decay > 0) ? std::forward<sample_buffer_type>(next) : samples_type{}) },
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum,
                                 const buffer_type& samples) :
            base_type  { ((decay > 0) ? samples : samples_type{}) },
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum,
                                 buffer_type&& samples) :
            base_type  { ((decay > 0) ? std::forward<buffer_type>(samples) : samples_type{}) },
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

        InterpolatedPeakDataT(peak_type decay,
                                 peak_type minimum,
                                 size_type num) :
            base_type  { ((decay > 0) ? num : 0) },
            m_Minimum  { minimum },
            m_DecayRate{ decay } {}

    private:
        using base_type::size;
        using base_type::total_size;
        using base_type::capacity;
        using base_type::total_capacity;
        using base_type::resize;
        using base_type::reserve;
        using base_type::data_for_update;
        using base_type::update;

    public:
        void            count(size_type num)                { base_type::resize(num); }
        [[nodiscard]]
        decltype(auto)  count()              const noexcept { return base_type::size(); }

    public:
        [[nodiscard]]
        constexpr auto  have_peaks() const noexcept { return m_DecayRate > 0; }

        void decay_rate(peak_type decay,
                        peak_type minimum) noexcept {
            if (have_peaks() && decay <= 0) { zero(); }
            m_DecayRate = decay;
            m_Minimum   = minimum;
        }

        void decay_rate(peak_type decay) noexcept {
            decay_rate(decay, m_Minimum);
        }

        void minimum(peak_type minimum) noexcept {
            decay_rate(m_DecayRate, minimum);
        }

        [[nodiscard]]
        constexpr auto decay_rate() const noexcept { return m_DecayRate; }
        [[nodiscard]]
        constexpr auto minimum   () const noexcept { return m_Minimum; }

    public:
        [[nodiscard]]
        auto data_for_update() noexcept {
            assert(valid());
            ::util::copy_n(m_Prev,
                             m_Next.data(),
                             m_Next.size());
            return span_type{ m_Next };
        }

        void update(const_span_type data) noexcept {
            assert(data);
            if (!have_peaks()) { return; }
            auto peaks{ data_for_update() };
            const auto maxCount{ std::min(peaks.size(), data.size()) };
            size_type i{ 0 };
            while (i < maxCount) {
                if (data[i] > peaks[i]) {
                    peaks[i] = data[i];
                } else if (peaks[i] > m_Minimum) {
                    peaks[i] -= m_DecayRate;
                } else {
                    peaks[i] = m_Minimum;
                }
                ++i;
            }
            while (i < count()) {
                if (peaks[i] > m_Minimum) {
                    peaks[i] -= m_DecayRate;
                } else {
                    peaks[i] = m_Minimum;
                }
                ++i;
            }
        }

        void update(const peak_buffer_type& data) noexcept {
            update(const_span_type{ data });
        }

        void update(peak_buffer_type&& data) {
            assert(valid());
            if (data.size() != m_Next.size()) {
                data.resize(m_Next.size(), 0);
            }
            update(const_span_type{ data });
        }

    private:
        peak_type m_Minimum  { 0 };
        peak_type m_DecayRate{ 0 };
    };

    /**************************************************************************
     * InterpolatedChannelDataT
     **************************************************************************/
    template <
        typename SampleTypeT,
        class AllocatorT = std::allocator<SampleTypeT>
    >
    class InterpolatedChannelDataT final {
    private:
        using this_type = InterpolatedChannelDataT<SampleTypeT, AllocatorT>;

    public:
        using samples_type           = Samples::InterpolatedDataT<SampleTypeT, AllocatorT>;
        using peaks_type             = Samples::InterpolatedPeakDataT<SampleTypeT, AllocatorT>;
        using sample_buffer_type     = typename samples_type::buffer_type;
        using sample_type            = typename samples_type::sample_type;
        using sample_span_type       = typename samples_type::span_type;
        using sample_const_span_type = typename samples_type::const_span_type;
        using size_type              = typename samples_type::size_type;
        using peak_buffer_type       = typename peaks_type::buffer_type;
        using peak_type              = typename peaks_type::peak_type;
        using peak_span_type         = typename peaks_type::span_type;
        using peak_const_span_type   = typename peaks_type::const_span_type;

    public:
        InterpolatedChannelDataT()                  noexcept = default;
        InterpolatedChannelDataT(const this_type& )          = default;
        InterpolatedChannelDataT(      this_type&&) noexcept = default;

        this_type& operator=(const this_type& )          = default;
        this_type& operator=(      this_type&&) noexcept = default;

    public:
        InterpolatedChannelDataT(const sample_buffer_type& samples,
                                 peak_type decay,
                                 peak_type minimum) :
            m_Samples{ samples },
            m_Peaks  { decay, minimum, samples } {}

        InterpolatedChannelDataT(const sample_buffer_type& prev,
                                 const sample_buffer_type& next,
                                 peak_type decay,
                                 peak_type minimum) :
            m_Samples{ prev, next },
            m_Peaks  { decay, minimum, prev, next } {}

        InterpolatedChannelDataT(size_type num,
                                 peak_type decay,
                                 peak_type minimum) :
            m_Samples{ num },
            m_Peaks  { decay, minimum, num } {}

    public:
        void swap(this_type& other) noexcept {
            assert(valid()); assert(other.valid());
            using std::swap;
            swap(m_Samples, other.m_Samples);
            swap(m_Peaks  , other.m_Peaks);
        }

        void clear          () noexcept { assert(valid()); m_Samples.clear(); m_Peaks.clear(); }
        void shrink_to_fit  ()          { assert(valid()); m_Samples.shrink_to_fit(); m_Peaks.shrink_to_fit(); }
        void reset          ()          { assert(valid()); m_Samples.reset(); m_Peaks.reset(); }
        void zero_samples   () noexcept { assert(valid()); m_Samples.zero(); }
        void zero_peaks     () noexcept { assert(valid()); m_Peaks.zero();   }
        void zero           () noexcept { zero_samples(); zero_peaks(); }

    public:
        [[nodiscard]]
        decltype(auto) empty() const noexcept {
            assert(valid());
            return m_Samples.empty();
        }

        //--------------------------------------------------
        // `count`:
        // ----------------------
        //
        // Want to ensure that all buffers **always**
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
        decltype(auto) count() const noexcept {
            assert(valid());
            return m_Samples.count();
        }

        void count(size_type num) {
            assert(valid());
            if (num == count()) { return; }
            auto samples{ std::move(m_Samples) };
            auto peaks  { std::move(m_Peaks)   };
            samples.count(num);
            peaks.count(num);
            m_Samples = std::move(samples);
            m_Peaks   = std::move(peaks);
            assert(valid());
        }

    public:
        [[nodiscard]]
        decltype(auto) samples_prev() const noexcept { assert(valid()); return m_Samples.prev(); }
        [[nodiscard]]
        decltype(auto) samples_next()       noexcept { assert(valid()); return m_Samples.next(); }
        [[nodiscard]]
        decltype(auto) samples_next() const noexcept { assert(valid()); return m_Samples.next(); }

        [[nodiscard]]
        decltype(auto) sample_prev(size_type pos) const noexcept { assert(valid()); return m_Samples.prev(pos); }
        [[nodiscard]]
        decltype(auto) sample_next(size_type pos)       noexcept { assert(valid()); return m_Samples.next(pos); }
        [[nodiscard]]
        decltype(auto) sample_next(size_type pos) const noexcept { assert(valid()); return m_Samples.next(pos); }

        [[nodiscard]]
        decltype(auto) samples_curr(sample_type interp) const {
            assert(valid());
            return m_Samples.curr(interp);
        }

        [[nodiscard]]
        decltype(auto) sample_curr(size_type pos,
                                   sample_type interp) const noexcept {
            assert(valid());
            return m_Samples.curr(pos, interp);
        }

        [[nodiscard]]
        decltype(auto) sample_curr_at(size_type pos,
                                      sample_type interp) const noexcept {
            assert(valid());
            return m_Samples.curr_at(pos, interp);
        }

    public:
        [[nodiscard]]
        decltype(auto) peaks_prev() const noexcept { assert(valid()); return m_Peaks.prev(); }
        [[nodiscard]]
        decltype(auto) peaks_next()       noexcept { assert(valid()); return m_Peaks.next(); }
        [[nodiscard]]
        decltype(auto) peaks_next() const noexcept { assert(valid()); return m_Peaks.next(); }

        [[nodiscard]]
        decltype(auto) peak_prev(size_type pos) const noexcept { assert(valid()); return m_Peaks.prev(pos); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type pos)       noexcept { assert(valid()); return m_Peaks.next(pos); }
        [[nodiscard]]
        decltype(auto) peak_next(size_type pos) const noexcept { assert(valid()); return m_Peaks.next(pos); }

        [[nodiscard]]
        decltype(auto) peaks_curr(sample_type interp) const {
            assert(valid());
            return m_Peaks.curr(interp);
        }

        [[nodiscard]]
        decltype(auto) peak_curr(size_type pos,
                                 peak_type interp) const noexcept {
            assert(valid());
            return m_Peaks.curr(pos, interp);
        }

        [[nodiscard]]
        decltype(auto) peak_curr_at(size_type pos,
                                    peak_type interp) const noexcept {
            assert(valid());
            return m_Peaks.curr_at(pos, interp);
        }

    public:
        constexpr void peak_decay_rate(peak_type decay,
                                       peak_type minimum) noexcept {
            assert(valid());
            m_Peaks.decay_rate(decay, minimum);
        }

        constexpr void peak_decay_rate(peak_type decay) noexcept {
            assert(valid());
            m_Peaks.decay_rate(decay);
        }

        constexpr void peak_minimum(peak_type minimum) noexcept {
            assert(valid());
            m_Peaks.peak_minimum(minimum);
        }

        [[nodiscard]]
        constexpr decltype(auto) peak_decay_rate() const noexcept { assert(valid()); return m_Peaks.decay_rate(); }
        [[nodiscard]]
        constexpr decltype(auto) peak_minimum   () const noexcept { assert(valid()); return m_Peaks.minimum(); }
        [[nodiscard]]
        constexpr decltype(auto) have_peaks     () const noexcept { assert(valid()); return m_Peaks.have_peaks(); }

    public:
        void update_peaks(peak_const_span_type peaks) {
            assert(valid());
            if (have_peaks()) {
                m_Peaks.update(peaks);
            }
        }

        void update_peaks(const peak_buffer_type& peaks) {
            assert(valid());
            if (have_peaks()) {
                m_Peaks.update(peaks);
            }
        }

        void update_peaks_from_samples() noexcept {
            assert(valid());
            if (have_peaks()) {
                m_Peaks.update(m_Samples.next());
            }
        }

    public:
        void update_samples(sample_const_span_type samples) noexcept {
            assert(valid());
            m_Samples.update(samples);
        }

        void update_samples(const sample_buffer_type& samples) {
            assert(valid());
            m_Samples.update(samples);
        }

        void update_samples(sample_buffer_type&& samples) noexcept {
            assert(valid());
            m_Samples.update(std::forward<sample_buffer_type>(samples));
        }

    public:
        void update(sample_const_span_type samples) {
            assert(valid());
            update_samples(samples);
            update_peaks_from_samples();
        }

        void update(const sample_buffer_type& samples) {
            assert(valid());
            update_samples(samples);
            update_peaks_from_samples();
        }

        void update(sample_buffer_type&& samples) noexcept {
            assert(valid());
            update_samples(std::forward<sample_buffer_type>(samples));
            update_peaks_from_samples();
        }

    public:
        [[nodiscard]]
        auto data_for_sample_update() noexcept {
            assert(valid());
            return m_Samples.data_for_update();
        }

        [[nodiscard]]
        auto data_for_peak_update() noexcept {
            assert(valid());
            return m_Peaks.data_for_update();
        }

    private:
        [[nodiscard]]
        bool valid() const noexcept {
            return m_Samples.count() == m_Peaks.count();
        }

    private:
        samples_type m_Samples{};
        peaks_type   m_Peaks  {};
    };
} // namespace Audio::Samples

#endif // GUID_4E75BAEC_10B8_49D8_8EF1_15CB921E0A6D