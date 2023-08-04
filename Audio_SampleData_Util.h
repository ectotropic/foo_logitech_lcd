#pragma once
#ifndef GUID_CA6B910B_3B48_4B75_AD8F_85C6147FECA1
#define GUID_CA6B910B_3B48_4B75_AD8F_85C6147FECA1
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
#include "Audio_SampleData.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
//--------------------------------------

namespace Audio::Samples::detail {
    //**************************************************************************
    // ChannelSamplesUtilImplT
    //**************************************************************************
    template <typename SampleTypeT>
    class ChannelSamplesUtilImplT final {
    private:
        using this_type = ChannelSamplesUtilImplT<SampleTypeT>;

    public:
        using channel_type       = ::Audio::Samples::InterpolatedChannelDataT<SampleTypeT>;
        using sample_buffer_type = typename channel_type::sample_buffer_type;
        using sample_type        = typename channel_type::sample_type;
        using peak_type          = typename channel_type::peak_type;
        using size_type          = typename channel_type::size_type;

    private:
        template <typename TypeT>
        using is_nullptr_type =
            std::is_same<TypeT, std::nullptr_t>;

        template <typename TypeT>
        inline static constexpr auto is_nullptr_type_v =
            is_nullptr_type<TypeT>::value;

        template <typename TypeT>
        using is_not_nullptr_type =
            std::negation<std::is_same<TypeT, std::nullptr_t>>;

        template <typename TypeT>
        inline static constexpr auto is_not_nullptr_type_v =
            is_not_nullptr_type<TypeT>::value;

    private:
        // targetCount < sourceCount
        template <typename TransformT>
        static void bin_data(channel_type& channel,
                             const sample_type* source,
                             size_type sourceSize,
                             size_type stride,
                             [[maybe_unused]] TransformT transform) noexcept {
            constexpr const auto HaveTransformT{ is_not_nullptr_type_v<TransformT> };
            constexpr auto fracMax{ static_cast<sample_type>(1) };

            auto target{ channel.data_for_sample_update() };

            const auto sampleCount        = sourceSize / stride;
            const auto samplesPerBinExact = static_cast<sample_type>(sampleCount) / static_cast<sample_type>(target.size());
            const auto samplesPerBinWhole = std::trunc(samplesPerBinExact);
            const auto samplesPerBinFrac  = samplesPerBinExact - samplesPerBinWhole;
            gsl_suppress(26467) // C26467: Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value.
            const auto samplesPerBin      = static_cast<size_type>(samplesPerBinWhole);
            assert(samplesPerBin > 0); assert(samplesPerBinWhole > 0);

            sample_type frac{ 0 };
            for (auto& bin : target) {
                sample_type acc{ 0 };
                for (size_type s = 0; s < samplesPerBin; ++s) {
                    acc += *source;
                    source += stride;
                }
                frac += samplesPerBinFrac;
                if (frac >= fracMax) {
                    frac -= fracMax;
                    acc += *source;
                    source += stride;
                    acc /= (samplesPerBinWhole + static_cast<sample_type>(1));
                } else {
                    acc /= samplesPerBinWhole;
                }

                if constexpr (HaveTransformT) {
                    bin = transform(acc);
                } else {
                    bin = acc;
                }
            }
        }

        //--------------------------------------------------

        // targetCount > sourceCount
        // TODO: Implement this properly (i.e. expand data to fill range)
        template <typename TransformT>
        static void expand_data(channel_type& channel,
                               const sample_type* source,
                               size_type sourceSize,
                               size_type stride,
                               [[maybe_unused]] TransformT transform) noexcept {
            constexpr const auto HaveTransformT{ is_not_nullptr_type_v<TransformT> };
            const auto sampleCount = sourceSize / stride;
            if (stride == 1 && !HaveTransformT) {
                channel.update_samples({ source, sourceSize });
            } else {
                auto data{ channel.data_for_sample_update() };
                size_type s{ 0 }, d{ 0 };
                while (s < sourceSize) {
                    if constexpr (HaveTransformT) {
                        data[d] = transform(source[s]);
                    } else {
                        data[d] = source[s];
                    }
                    ++d;
                    s += stride;
                }
                while (d < sampleCount) {
                    if constexpr (HaveTransformT) {
                        data[d] = transform(0);
                    } else {
                        data[d] = 0;
                    }
                    ++d;
                }
            }
        }

        //--------------------------------------------------

        // targetCount == sourceCount
        template <typename TransformT>
        static void copy_data(channel_type& channel,
                              const sample_type* source,
                              size_type sourceSize,
                              size_type stride,
                              [[maybe_unused]] TransformT transform) noexcept {
            constexpr const auto HaveTransformT{ is_not_nullptr_type_v<TransformT> };
            if (stride == 1 && !HaveTransformT) {
                channel.update_samples({ source, sourceSize });
            } else {
                auto data{ channel.data_for_sample_update() };
                for (auto& bin : data) {
                    if constexpr (HaveTransformT) {
                        bin = transform(*source);
                    } else {
                        bin = *source;
                    }
                    source += stride;
                }
            }
        }

        //--------------------------------------------------

    public:
        template <typename TransformT = std::nullptr_t>
        static void update(channel_type& channel,
                           const sample_type* source,
                           size_type sourceSize,
                           size_type stride,
                           [[maybe_unused]] TransformT transform = {}) noexcept {
            assert(source); assert(sourceSize > 0);
            assert(stride > 0 && stride < sourceSize);

            const auto targetCount{ channel.count() };
            const auto sampleCount{ sourceSize / stride };

            if (sampleCount == targetCount) {
                this_type::copy_data(channel,
                                     source, sourceSize, stride,
                                     transform);
            } else if (sampleCount > targetCount) {
                this_type::bin_data(channel,
                                    source, sourceSize, stride,
                                    transform);
            } else { // sampleCount < targetCount
                this_type::expand_data(channel,
                                       source, sourceSize, stride,
                                       transform);
            }
            channel.update_peaks_from_samples();
        }
   }; // template <...> struct ChannelSamplesUtilImplT final

    //**************************************************************************
    // SamplesUtilImplT
    //**************************************************************************
    template <
        typename SampleTypeT,
        std::size_t CountT
    >
    class SamplesUtilImplT final {
    private:
        using this_type = SamplesUtilImplT<SampleTypeT, CountT>;

    public:
        using channels_util_type            = ::Audio::Samples::detail::ChannelSamplesUtilImplT<SampleTypeT>;
        using channels_type                 = ::Audio::Samples::InterpolatedChannelsT<SampleTypeT, CountT>;
        using sample_buffer_type            = typename channels_util_type::sample_buffer_type;
        using sample_type                   = typename channels_util_type::sample_type;
        using peak_type                     = typename channels_util_type::peak_type;
        using single_channel_type           = typename channels_util_type::channel_type;
        using size_type                     = typename channels_util_type::size_type;

    public:
        template <typename TransformT>
        static auto update(channels_type& channels,
                           const sample_type* source,
                           size_type sourceSize,
                           size_type stride,
                           TransformT transform) {
            const auto channelCount{ channels.channel_count() };
            auto maxCount{ std::min(channelCount, stride) };
            size_type ch{ 0 };
            while (ch < maxCount) {
                if (transform) {
                    channels_util_type::update(
                        channels[ch],
                        source, sourceSize, stride,
                        [ch,&transform](sample_type s)
                            -> sample_type {
                            return transform(ch, s);
                        }
                    );
                } else {
                    channels_util_type::update(
                        channels[ch],
                        source, sourceSize, stride
                    );
                }
                ++ch;
                if (stride > 1) { ++source; }
            }
            const auto cUpdated{ ch };
            while (ch < channelCount) {
                channels[ch].zero();
                ++ch;
            }
            return cUpdated;
        }
    }; // template <...> class SamplesUtilImplT final
} // namespace Audio::Samples::detail

//==============================================================================

namespace Audio::Samples {
    //**************************************************************************
    // update_sample_data
    //**************************************************************************
    template <
        typename SampleTypeT,
        typename... ArgPackT
    >
    inline void update_sample_data(::Audio::Samples::InterpolatedChannelDataT<SampleTypeT>& channel,
                                   ArgPackT&& ...argpack) {
        using util_type = Audio::Samples::detail::SampleDataUtilImplT<SampleTypeT>;
        util_type::update(channel, source, std::forward<ArgPackT>(argpack)...);
    }

    //--------------------------------------------------------------------------

    template <
        typename SampleTypeT,
        std::size_t CountT,
        typename... ArgPackT
    >
    inline auto update_sample_data(::Audio::Samples::InterpolatedChannelsT<SampleTypeT, CountT>& channels,
                                   ArgPackT&& ...argpack) {
        using util_type = ::Audio::Samples::detail::SamplesUtilImplT<SampleTypeT, CountT>;
        return util_type::update(channels, std::forward<ArgPackT>(argpack)...);
    }

    //--------------------------------------------------------------------------

    template <
        typename SampleTypeT,
        std::size_t CountT,
        typename... ArgPackT
    >
    inline auto update_sample_data(Samples::SampleDataT<SampleTypeT, CountT>& channels,
                                   ArgPackT&& ...argpack) {
        using util_type = Samples::detail::SamplesUtilImplT<SampleTypeT, CountT>;
        return util_type::update(channels.channel_data(), std::forward<ArgPackT>(argpack)...);
    }

    //**************************************************************************
    // get_combined_samples
    //**************************************************************************
    template <
        typename SampleTypeT,
        std::size_t CountT
    >
    inline [[nodiscard]]
    auto get_combined_samples(const ::Audio::Samples::InterpolatedChannelsT<SampleTypeT, CountT>& channels,
                              std::size_t channelCount = CountT) {
        using sample_type        = typename Samples::InterpolatedChannelsT<SampleTypeT, CountT>::sample_type;
        using sample_buffer_type = typename Samples::InterpolatedChannelsT<SampleTypeT, CountT>::sample_buffer_type;
        using size_type          = typename Samples::InterpolatedChannelsT<SampleTypeT, CountT>::size_type;

        channelCount = std::min(channelCount, channels.channel_count());
        const auto channelWeight = static_cast<sample_type>(1.) / static_cast<sample_type>(channelCount);
        const auto sampleCount   = channels.channel_value_count();
        sample_buffer_type combinedSamples{};
        combinedSamples.resize(sampleCount, 0);
        for (size_type ch = 0; ch < channelCount; ++ch) {
            const auto& channelSamples = channels.samples_next(ch);
            for (size_type s = 0; s < sampleCount; ++s) {
                combinedSamples[s] += channelSamples[s] * channelWeight;
            }
        }
        return combinedSamples;
    }

    //--------------------------------------------------------------------------

    template <
        typename SampleTypeT,
        std::size_t CountT
    >
    inline [[nodiscard]]
    decltype(auto) get_combined_samples(const ::Audio::Samples::SampleDataT<SampleTypeT, CountT>& channels,
                                        std::size_t channelCount = CountT) {
        return ::Audio::Samples::get_combined_samples(channels.channel_data(),
                                                      channelCount);
    }
} // namespace Audio::Samples

#endif // GUID_CA6B910B_3B48_4B75_AD8F_85C6147FECA1