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
#include "CommonHeaders.h"
#include "Audio_DataManager.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DecibelData_Util.h"
#include "Audio_SampleData_Util.h"
//--------------------------------------

namespace Audio {
    /**************************************************************************
     * IAudioDataManager *
     **************************************************************************/
    void IAudioDataManager::Update(const Visualisation::RequestParams& request) {
        m_UsingData = request.Want;

        update_params params{};
        update_hint_params hints{};

        params.m_WantSpectrum = false;
        params.m_WantWaveform = false;

        if (m_UsingData & (vis_data_type::CombinedWaveform | vis_data_type::Waveform)) {
            params.m_WantWaveform = true;
            assert(request.Waveform.nSampleCountHint > 0);
            m_fnWaveformTransform = request.Waveform.fnTransform;
            m_Waveform.value_count(request.Waveform.nSampleCountHint);
            m_Waveform.peak_decay_rate(request.Waveform.fPeakDecayRate,
                                       request.Waveform.fPeakMininum);
        } else {
            m_Waveform.clear();
            m_fnWaveformTransform = {};
        }

        if (m_UsingData & (vis_data_type::CombinedDecibel | vis_data_type::Decibel)) {
            params.m_WantWaveform = true;
            m_fnDecibelTransform = request.Decibel.fnTransform;
            m_Decibel.peak_decay_rate(request.Decibel.fPeakDecayRate,
                                      request.Decibel.fPeakMininum);
        } else {
            m_Decibel.clear();
            m_fnDecibelTransform = {};
        }

        if (m_UsingData & (vis_data_type::CombinedSpectrum | vis_data_type::Spectrum)) {
            hints.m_SpectrumSize = request.Spectrum.nSampleCountHint;
            params.m_WantSpectrum = true;
            assert(request.Spectrum.nSampleCountHint > 0);
            m_fnSpectrumTransform = request.Spectrum.fnTransform;
            m_Spectrum.value_count(request.Spectrum.nSampleCountHint);
            m_Spectrum.peak_decay_rate(request.Spectrum.fPeakDecayRate,
                                       request.Spectrum.fPeakMininum);
        } else {
            m_Spectrum.clear();
            m_fnSpectrumTransform = {};
        }

        const auto elapsed{ m_UpdateTimer.GetElapsedSeconds() };
        params.m_Offset = elapsed;
        hints.m_Duration = elapsed;
        OnUpdate(params, hints);
        m_UpdateTimer.Start();
    }

    //--------------------------------------------------------------------------

    void IAudioDataManager::SetWaveformData(const waveform_sample_type* samples,
                                            size_type sampleCount,
                                            size_type channelCount) {
        if (m_UsingData & (vis_data_type::CombinedWaveform | vis_data_type::Waveform)) {
            if (samples == nullptr || sampleCount == 0 || channelCount == 0) {
                m_Waveform.zero();
                return;
            }

            const auto cUpdated = Samples::update_sample_data(m_Waveform,
                                                              samples, sampleCount, channelCount,
                                                              m_fnWaveformTransform);
            if ((cUpdated > 0) && (m_UsingData & vis_data_type::CombinedWaveform)) {
                auto combined{ Samples::get_combined_samples(m_Waveform, cUpdated) };
                m_Waveform.combined_update(std::move(combined));
            }
        }

        if (m_UsingData & (vis_data_type::CombinedDecibel | vis_data_type::Decibel)) {
            SetDecibelData(samples, sampleCount, channelCount);
        }
    }

    //--------------------------------------------------------------------------

    void IAudioDataManager::SetDecibelData(const dB_type* samples,
                                           size_type sampleCount,
                                           size_type channelCount) {
        if (samples == nullptr || sampleCount == 0 || channelCount == 0) {
            m_Decibel.zero();
            return;
        }

        static constexpr const auto CombineChannelIndex{ static_cast<size_type>(-1) };

        const auto dBChannelCount{ m_Decibel.channel_count() };
        if (channelCount > 1) {
            auto updateChannelCount{ std::min(dBChannelCount, channelCount) };
            size_type ch{ 0 };
            dB_type combined{ 0 };
            while (ch < updateChannelCount) {
                auto dB{ dB::dBFromWavedata(samples,
                                            sampleCount,
                                            channelCount) };
                combined += dB;
                if (m_fnDecibelTransform) { dB = m_fnDecibelTransform(ch, dB); }
                m_Decibel.channel_update(ch, dB);
                ++ch; ++samples;
            }

            if (m_UsingData & vis_data_type::CombinedDecibel) {
                combined /= static_cast<dB_type>(ch);
                if (m_fnDecibelTransform) {
                    combined = m_fnDecibelTransform(CombineChannelIndex,
                                                    combined);
                }
                m_Decibel.combined_update(combined);
            } else {
                m_Decibel.combined_zero();
            }
            while (ch < dBChannelCount) { m_Decibel.channel_zero(ch++); }
        } else {
            const auto dB{ dB::dBFromWavedata(samples, sampleCount, 1U) };
            for (size_type ch = 0; ch < dBChannelCount; ++ch) {
                if (m_fnDecibelTransform) {
                    m_Decibel.channel_update(ch, m_fnDecibelTransform(ch, dB));
                } else {
                    m_Decibel.channel_update(ch, dB);
                }
            }
            if (m_UsingData & vis_data_type::CombinedDecibel) {
                if (m_fnDecibelTransform) {
                    m_Decibel.combined_update(m_fnDecibelTransform(CombineChannelIndex, dB));
                } else {
                    m_Decibel.combined_update(dB);
                }
            }
        }
    }
    //--------------------------------------------------------------------------

    void IAudioDataManager::SetSpectrumData(const spectrum_sample_type* samples,
                                            size_type sampleCount,
                                            size_type channelCount) {
        if (m_UsingData & (vis_data_type::CombinedSpectrum | vis_data_type::Spectrum)) {
            if (samples == nullptr || sampleCount == 0 || channelCount == 0) {
                m_Spectrum.zero();
                return;
            }

            const auto  cUpdated = Samples::update_sample_data(m_Spectrum,
                                                               samples, sampleCount, channelCount,
                                                               m_fnSpectrumTransform);
            if ((cUpdated > 0) && (m_UsingData & vis_data_type::CombinedSpectrum)) {
                auto combined{ Samples::get_combined_samples(m_Spectrum, cUpdated) };
                m_Spectrum.combined_update(std::move(combined));
            }
        }
    }
} // namespace Audio
