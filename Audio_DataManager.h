#pragma once
#ifndef GUID_65F02D44_9EDE_4A00_9F40_BD28CF37F3A2
#define GUID_65F02D44_9EDE_4A00_9F40_BD28CF37F3A2
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
#include "Util/SequentialEnum.h"
#include "Util/Singleton.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/Visualisation_Params.h"

#include "Config/Config_TrackDetails.h"

#include "Audio_DecibelData.h"
#include "Audio_SampleData.h"

#include "Image_ImageData.h"
//--------------------------------------

//--------------------------------------
//
#include <functional>
#include <memory>
#include <string>
//--------------------------------------

namespace Audio {
    //**************************************************************************
    // Channel
    //**************************************************************************
    SEQUENTIAL_NAMED_ENUM(Channel,
                          SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Left, 0),
                                                 Right),
                          SEQUENTIAL_NAMED_ENUM_STRINGS(L"Left",
                                                        L"Right"));

    //**************************************************************************
    // PlayState
    //**************************************************************************
    SEQUENTIAL_NAMED_ENUM(PlayState,
                          SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Stopped, 0),
                                                 Playing,
                                                 Paused),
                          SEQUENTIAL_NAMED_ENUM_STRINGS(L"Stopped",
                                                        L"Playing",
                                                        L"Paused"));

    //**************************************************************************
    // IAudioDataManager
    //**************************************************************************
    class IAudioDataManager :
        public ::util::singleton<IAudioDataManager> {
    private:
        using stop_watch = ::Windows::StopWatch;

    public:
        inline static constexpr const auto ChannelCount         { Channel::count() };
        inline static constexpr const auto TrackDetailsPageCount{ TrackDetailsType::count() };

    public:
        using image_data_type    = ::Image::ImageData::Compact;
        using play_state_type    = PlayState;
        using channel_type       = Channel;
        using vis_data_type      = ::Visualisation::DataType;
        using duration_type      = float;
        using interpolation_type = float;
        using string_type        = std::wstring;
        using size_type          = std::size_t;

        using request_params              = Visualisation::RequestParams;

        using spectrum_sample_type        = typename request_params::spectrum_sample_type;
        using spectrum_peak_type          = typename request_params::spectrum_peak_type;
        using spectrum_transform_type     = typename request_params::spectrum_transform_type;
        using spectrum_data_type          = Samples::SampleDataT<spectrum_sample_type, ChannelCount>;
        using spectrum_sample_buffer_type = typename spectrum_data_type::sample_buffer_type;

        using waveform_sample_type        = typename request_params::waveform_sample_type;
        using waveform_peak_type          = typename request_params::waveform_peak_type;
        using waveform_transform_type     = typename request_params::waveform_transform_type;
        using waveform_data_type          = Samples::SampleDataT<waveform_sample_type, ChannelCount>;
        using waveform_sample_buffer_type = typename waveform_data_type::sample_buffer_type;

        using dB_type                     = typename request_params::dB_type;
        using decibel_type                = typename request_params::decibel_type;
        using dB_peak_type                = typename request_params::dB_peak_type;
        using dB_transform                = typename request_params::dB_transform_type;
        using dB_data_type                = dB::DecibelDataT<dB_type, ChannelCount>;

        using track_details_pages         = std::array<string_type, TrackDetailsPageCount>;

    public:
        struct update_params final {
            duration_type m_Offset      { 0 };
            bool          m_WantSpectrum{ false };
            bool          m_WantWaveform{ false };
        };

        struct update_hint_params final {
            duration_type m_Duration    { 0 };
            size_type     m_SpectrumSize{ 0 };
        };

    public:
        static void destroy() noexcept {
            auto pMgr{ singleton::instance_peek() };
            if (pMgr) {
                try {
                    pMgr->Uninitialise();
                } catch (...) {
                    assert(false);
                }
            }
            singleton::destroy();
        }

    protected:
        IAudioDataManager() noexcept = default;
        ~IAudioDataManager() noexcept = default;

    public:
        [[nodiscard]]
        virtual bool IsValid() const = 0;

    protected:
        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void OnInitialise() {}

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void OnUninitialise() {}

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void OnUpdateConfig() {}

        virtual void OnUpdate(const update_params& params,
                              const update_hint_params& hints) = 0;

    public:
        constexpr auto  GetPlayState        ()                  const noexcept { return m_ePlayState; }
        constexpr bool  HasTrackChanged     ()                  const noexcept { return m_bTrackChanged; }
        constexpr auto  GetTrackTime        ()                  const noexcept { return m_fTrackTime; }
        constexpr auto  GetTrackLength      ()                  const noexcept { return m_fTrackLength; }
        constexpr bool  HasTrackTextChanged ()                  const noexcept { return m_bTrackDetailsChanged; }
                  auto& GetTrackText        ()                  const noexcept { return m_TrackDetails; }
                  auto& GetTrackText        (size_type index)   const noexcept { return m_TrackDetails[index]; }
        constexpr bool  HasAlbumArtChanged  ()                  const noexcept { return m_bAlbumArtChanged; }
                  auto& GetAlbumArt         ()                  const noexcept { return m_AlbumArt; }

    public:
        void Initialise  () { OnInitialise();   }
        void Uninitialise() { OnUninitialise(); }

        void StartFrame() noexcept {}
        void EndFrame  () noexcept {
            m_bTrackChanged = false;
            m_bTrackDetailsChanged = false;
            m_bAlbumArtChanged = false;
        }

        void UpdateConfig() { OnUpdateConfig(); }

        void Update(const Visualisation::RequestParams& params);

    public:
        //-------------------------------------------------
        // Waveform
        [[nodiscard]]
        decltype(auto) GetWaveform(channel_type ch,
                                   interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Waveform);
            assert(ch >= 0 && ch < m_Waveform.channel_count());
            assert(!m_Waveform.channel(ch).empty());
            return m_Waveform.channel_samples_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetWaveformPeaks(channel_type ch,
                                        interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Waveform);
            assert(ch >= 0 && ch < m_Waveform.channel_count());
            assert(m_Waveform.have_peaks());
            return m_Waveform.channel_peaks_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetWaveform(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedWaveform);
            assert(!m_Waveform.combined().empty());
            return m_Waveform.combined_samples_curr(interp);
        }
        [[nodiscard]]
        decltype(auto) GetWaveformPeaks(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedWaveform);
            assert(m_Waveform.have_peaks());
            return m_Waveform.combined_peaks_curr(interp);
        }
        //-------------------------------------------------

        //-------------------------------------------------
        // dB
        [[nodiscard]]
        decltype(auto) GetDB(channel_type ch,
                             interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Decibel);
            assert(ch >= 0 && ch < m_Decibel.channel_count());
            return m_Decibel.channel_dB_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetDBPeaks(channel_type ch,
                                  interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Decibel);
            assert(ch >= 0 && ch < m_Decibel.channel_count());
            assert(m_Decibel.have_peaks());
            return m_Decibel.channel_peak_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetDB(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedDecibel);
            return m_Decibel.combined_dB_curr(interp);
        }
        [[nodiscard]]
        decltype(auto) GetDBPeaks(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedDecibel);
            assert(m_Decibel.have_peaks());
            return m_Decibel.combined_peak_curr(interp);
        }
        //-------------------------------------------------

        //-------------------------------------------------
        // Spectrum
        [[nodiscard]]
        decltype(auto) GetSpectrum(channel_type ch,
                                   interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Spectrum);
            assert(ch >= 0 && ch < m_Spectrum.channel_count());
            assert(!m_Spectrum.channel(ch).empty());
            return m_Spectrum.channel_samples_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetSpectrumPeaks(channel_type ch,
                                        interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::Spectrum);
            assert(ch >= 0 && ch < m_Spectrum.channel_count());
            assert(m_Spectrum.have_peaks());
            return m_Spectrum.channel_peaks_curr(ch, interp);
        }
        [[nodiscard]]
        decltype(auto) GetSpectrum(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedSpectrum);
            assert(!m_Spectrum.combined().empty());
            return m_Spectrum.combined_samples_curr(interp);
        }
        [[nodiscard]]
        decltype(auto) GetSpectrumPeaks(interpolation_type interp) const {
            assert(m_UsingData & vis_data_type::CombinedSpectrum);
            assert(m_Spectrum.combined().have_peaks());
            return m_Spectrum.combined_peaks_curr(interp);
        }
        //-------------------------------------------------

    protected:
        constexpr void SetPlayState(play_state_type state) noexcept {
            m_ePlayState = state;
        }

        constexpr void SetPlayState(bool stopped, bool paused) noexcept {
            if (stopped) {
                m_ePlayState = play_state_type::Stopped;
            } else if (paused) {
                m_ePlayState = play_state_type::Paused;
            } else {
                m_ePlayState = play_state_type::Playing;
            }
        }

        constexpr void SetTrackChanged(bool changed = true) noexcept {
            m_bTrackChanged = changed;
        }

        constexpr void SetTrackDuration(duration_type duration) noexcept {
            m_fTrackLength = duration;
        }

        constexpr void SetPlaybackTime(duration_type current) noexcept {
            m_fTrackTime   = current;
        }

        void SetTrackDetails(const track_details_pages& details) noexcept {
            m_bTrackDetailsChanged = true;
            m_TrackDetails = details;
        }

        void SetTrackDetails(track_details_pages&& details) noexcept {
            m_bTrackDetailsChanged = true;
            m_TrackDetails = std::forward<track_details_pages>(details);
        }

        void SetAlbumArt(const image_data_type& image) {
            m_bAlbumArtChanged = true;
            m_AlbumArt = image;
        }

        void SetAlbumArt(image_data_type&& image) noexcept {
            m_bAlbumArtChanged = true;
            m_AlbumArt = std::forward<image_data_type>(image);
        }

        void SetWaveformData(const waveform_sample_type* samples,
                             size_type sampleCount,
                             size_type channelCount);

        void SetDecibelData(const dB_type* samples,
                            size_type sampleCount,
                            size_type channelCount);

        void SetSpectrumData(const spectrum_sample_type* samples,
                             size_type sampleCount,
                             size_type channelCount);

    private:
        stop_watch          m_UpdateTimer         {};

        play_state_type     m_ePlayState          { play_state_type::Stopped };
        bool                m_bTrackChanged       { false };
        duration_type       m_fTrackTime          { 0 };
        duration_type       m_fTrackLength        { 0 };
        bool                m_bTrackDetailsChanged{ false };
        track_details_pages m_TrackDetails        { };
        bool                m_bAlbumArtChanged    { false };
        image_data_type     m_AlbumArt            { };

        vis_data_type       m_UsingData           { vis_data_type::None };

        waveform_data_type  m_Waveform            { };
        waveform_transform_type  m_fnWaveformTransform { };

        dB_data_type        m_Decibel             { };
        dB_transform        m_fnDecibelTransform  { };

        spectrum_data_type  m_Spectrum            { };
        spectrum_transform_type  m_fnSpectrumTransform { };
    }; // class IAudioDataManager
} // namespace Audio

#endif // GUID_65F02D44_9EDE_4A00_9F40_BD28CF37F3A2