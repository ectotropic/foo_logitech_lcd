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
#include "foobar/foobar_sdk.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/foobar_audio_data_manager.h"
#include "foobar/foobar_string_util.h"
//--------------------------------------

//--------------------------------------
//
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_StopWatch.h"
#include "Image_ImageLoader.h"
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include <cstdint>
//--------------------------------------

namespace {
    //**************************************************************************
    // Helpers
    //**************************************************************************
    inline constexpr auto GetNextPowerOfTwo(unsigned int val) noexcept {
        if (val <= 1) { return 2U; }

        --val;

        val = (val >>  1) | val;
        val = (val >>  2) | val;
        val = (val >>  4) | val;
        val = (val >>  8) | val;
        val = (val >> 16) | val;

        return val + 1;
    }

    //------------------------------------------------------

    inline auto GetAlbumArtTypeID(AlbumArtType type) noexcept {
        namespace fb_art = ::foobar::metadata::album_art;
        switch (type) {
            case AlbumArtType::FrontCover: return fb_art::id_helper::cover_front;
            case AlbumArtType::BackCover:  return fb_art::id_helper::cover_back;
            case AlbumArtType::Disc:       return fb_art::id_helper::disc;
            case AlbumArtType::Artist:     return fb_art::id_helper::artist;
            case AlbumArtType::Icon:       return fb_art::id_helper::icon;
            HintNoDefault();
        }
    }

    //------------------------------------------------------

    inline auto GetAlbumArtTypeIDList(AlbumArtType type) {
        return foobar::make_list(GetAlbumArtTypeID(type));
    }
} // namespace <anonymous>

//==============================================================================

namespace foobar {
    //**************************************************************************
    // foobar_audio_data_manager
    //**************************************************************************
    void foobar_audio_data_manager::OnInitialise() {
        m_pVisStream = ::foobar::visualisation::create_v3_stream();
        play_callback_manager::get()->register_callback(this,
                                                        play_callback::flag_on_playback_starting |
                                                        play_callback::flag_on_playback_new_track |
                                                        play_callback::flag_on_playback_stop |
                                                        play_callback::flag_on_playback_seek |
                                                        play_callback::flag_on_playback_pause |
                                                        play_callback::flag_on_playback_edited |
                                                        play_callback::flag_on_playback_dynamic_info_track |
                                                        play_callback::flag_on_playback_time,
                                                        true);
        now_playing_album_art_notify_manager::get()->add(this);
        OnUpdateConfig();
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::OnUninitialise() {
        now_playing_album_art_notify_manager::get()->remove(this);
        play_callback_manager::get()->unregister_callback(this);
        m_pVisStream.reset();
        m_pCurrentTrack.release();
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::OnUpdateConfig() {
        const auto& genConfig = ::Config::GeneralConfig::get();

        fb_album_art_id_list art_id_list{};
        if (genConfig.m_Canvas.m_Wallpaper.m_Mode == WallpaperMode::AlbumArt &&
            genConfig.m_Canvas.m_Wallpaper.m_AlbumArtType != AlbumArtType::Any) {
            art_id_list = std::move(GetAlbumArtTypeIDList(genConfig.m_Canvas.m_Wallpaper.m_AlbumArtType));
        }
        m_AlbumArtTypeIDList = art_id_list;

        const auto& trackDetailsConfig = ::Config::TrackDetailsConfig::get();

        std::array<fb_script_source, TrackDetailsType::count()> scripts{};
        for (const auto p : TrackDetailsType{}) {
            scripts[p] = std::move(to_utf8(trackDetailsConfig[p].m_Text.m_Format));
        }
        m_TitleFormatter.assign(scripts);

        on_metadata_change(m_pCurrentTrack, false);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    void foobar_audio_data_manager::OnUpdate(const update_params& params,
                                             const update_hint_params& hints) {
        namespace fb_vis = foobar::visualisation;

        fb_cached_metadata cached_metadata{};
        {
            const auto lock{ m_CriticalSection.ScopedLock() };
            using std::swap;
            swap(cached_metadata, m_CachedMetadata);
        }

        SetTrackChanged(cached_metadata.has_track_changed());

        if (cached_metadata.has_play_state_changed()) {
            SetPlayState(cached_metadata.get_stopped(),
                         cached_metadata.get_paused());
        }

        if (cached_metadata.has_playback_time_changed()) {
            SetPlaybackTime(static_cast<duration_type>(cached_metadata.get_playback_time()));
        }

        if (cached_metadata.has_track_length_changed()) {
            SetTrackDuration(static_cast<duration_type>(cached_metadata.get_track_length()));
        }

        if (cached_metadata.has_text_data_changed()) {
            const auto& text_data{ cached_metadata.get_text_data() };
            track_details_pages page_data{};
            for (const auto page : TrackDetailsType{}) {
                foobar::convert_string(page_data[page], text_data[page]);
            }
            SetTrackDetails(std::move(page_data));
        }

        if (cached_metadata.has_album_art_changed()) {
            const auto& p_art{ cached_metadata.get_album_art() };
            image_data_type image{};
            if (p_art.is_valid()) {
                image = ::Image::ImageLoader::Load(p_art->get_ptr(),
                                                   p_art->get_size());
            }
            SetAlbumArt(std::move(image));
        }

        if (params.m_WantWaveform || params.m_WantSpectrum) {
            fb_duration_type absTime{ 0 };
            if (! m_pVisStream->get_absolute_time(absTime)) {
                absTime = 0;
            }
            auto offset{ absTime + static_cast<double>(params.m_Offset) };
            const auto length{ GetTrackLength() };
            if (offset > length) { offset = length; }

            if (params.m_WantWaveform) {
                constexpr const fb_duration_type minimum_duratation{ .05 };
                constexpr const fb_duration_type maximum_duratation{ .5  };
                fb_duration_type duratation{ hints.m_Duration };
                if (duratation > maximum_duratation) {
                    duratation = maximum_duratation;
                } else if (duratation < minimum_duratation) {
                    duratation = minimum_duratation;
                }
                const auto data{ fb_vis::get_audio_data(m_pVisStream, offset, duratation) };
                SetWaveformData(data.get_data(),
                                data.get_data_size(),
                                data.get_channel_count());
            }

            if (params.m_WantSpectrum) {
                const auto sizeHint{ static_cast<unsigned int>(hints.m_SpectrumSize) };
                // Count will be half FFT size, so double requested count
                const auto fftSize = GetNextPowerOfTwo(sizeHint * 2);
                const auto data{ fb_vis::get_spectrum_data(m_pVisStream, offset, fftSize) };
                SetSpectrumData(data.get_data(),
                                data.get_data_size(),
                                data.get_channel_count());
            }
        }
    }

    //------------------------------------------------------
    //------------------------------------------------------

    void foobar_audio_data_manager::on_metadata_change(metadb_handle_ptr p_track,
                                                       bool p_new_track) {
        namespace fb_art = ::foobar::metadata::album_art;

        if (!p_track.is_valid()) { return; }

        fb_formatted_array formatted{};
        m_TitleFormatter.format(formatted, p_track);

        album_art_data_ptr art{};
        if (m_AlbumArtTypeIDList.size() > 0) {
            art = fb_art::query_single(p_track, m_AlbumArtTypeIDList);
        }

        const auto length{ p_track->get_length() };
        {
            const auto lock{ m_CriticalSection.ScopedLock() };
            if (p_new_track) {
                m_CachedMetadata.on_playback_new_track(formatted, length, art);
            } else {
                m_CachedMetadata.on_playback_edited(formatted, length, art);
            }
        }

        m_pCurrentTrack = p_track;
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_new_track(metadb_handle_ptr p_track) {
        on_metadata_change(p_track, true);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_edited(metadb_handle_ptr p_track) {
        on_metadata_change(p_track, false);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_time(double p_time) noexcept {
        const auto lock{ m_CriticalSection.ScopedLock() };
        m_CachedMetadata.on_playback_time(p_time);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_seek(double p_time) noexcept {
        const auto lock{ m_CriticalSection.ScopedLock() };
        m_CachedMetadata.on_playback_time(p_time);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_stop(play_control::t_stop_reason /*reason*/) noexcept {
        const auto lock{ m_CriticalSection.ScopedLock() };
        m_CachedMetadata.on_playback_stop();
        m_pCurrentTrack.release();
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_dynamic_info_track(const file_info& pInfo) {
        if (!m_pCurrentTrack.is_valid()) { return; }

        fb_formatted_array formatted{};
        m_TitleFormatter.format(formatted, m_pCurrentTrack, &pInfo);

        {
            const auto lock{ m_CriticalSection.ScopedLock() };
            m_CachedMetadata.on_playback_dynamic_info(formatted, pInfo.get_length());
        }
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_starting(play_control::t_track_command /*p_command*/, bool p_paused) noexcept {
        const auto lock{ m_CriticalSection.ScopedLock() };
        m_CachedMetadata.on_playback_starting(p_paused);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_playback_pause(bool p_state) noexcept {
        const auto lock{ m_CriticalSection.ScopedLock() };
        m_CachedMetadata.on_playback_pause(p_state);
    }

    //------------------------------------------------------

    void foobar_audio_data_manager::on_album_art(album_art_data::ptr p_art) {
        const auto genConfig = ::Config::GeneralConfig::get();
        if (m_AlbumArtTypeIDList.size() == 0) {
            const auto lock{ m_CriticalSection.ScopedLock() };
            m_CachedMetadata.on_album_art(p_art);
        }
    }
} // namespace foobar
