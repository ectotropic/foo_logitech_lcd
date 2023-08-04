#pragma once
#ifndef GUID_88E2DFF8_7D01_46F7_9FAD_032DD30B9368
#define GUID_88E2DFF8_7D01_46F7_9FAD_032DD30B9368
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

namespace foobar::metadata {
    //**************************************************************************
    // cached_data
    //**************************************************************************
    class cached_data final {
    public:
        using duration_type       = double;
        using string_type         = ::pfc::string8;
        using flags_type          = ::t_uint32;
        using text_data_type      = ::pfc::array_t<string_type>;
        using album_art_data_type = typename ::album_art_data::ptr;

    public:
        enum {
            changed_none          = 0,
            changed_play_state    = 1 << 0,
            changed_track         = 1 << 1,
            changed_playback_time = 1 << 2,
            changed_track_length  = 1 << 3,
            changed_text_data     = 1 << 4,
            changed_album_art     = 1 << 5,
        };

    public:
        cached_data() noexcept = default;

        void swap(cached_data& other) {
            using std::swap;
            swap(m_flags        , other.m_flags);
            swap(m_stopped      , other.m_stopped);
            swap(m_paused       , other.m_paused);
            swap(m_playback_time, other.m_playback_time);
            swap(m_length       , other.m_length);
            swap(m_text_data    , other.m_text_data);
            swap(m_album_art    , other.m_album_art);
        }

        void reset() {
            cached_data temp{};
            swap(temp);
        }

        void on_playback_starting(bool p_paused) noexcept {
            m_stopped = false;
            m_paused  = p_paused;
            m_flags |= changed_play_state;
        }

        void on_playback_pause(bool p_state) noexcept {
            m_stopped = false;
            m_paused  = p_state;
            m_flags |= changed_play_state;
        }

        void on_playback_stop() noexcept {
            m_stopped       = true;
            m_paused        = false;
            m_playback_time = 0;
            m_flags |= changed_play_state | changed_playback_time;
        }

        void on_playback_time(duration_type p_time) noexcept {
            m_playback_time = p_time;
            m_flags |= changed_playback_time;
        }

        void on_playback_new_track(text_data_type p_metadata,
                                   duration_type p_length,
                                   album_art_data_type p_album_art = {}) {
            m_text_data = p_metadata;
            m_flags |= changed_text_data | changed_track;
            m_length = p_length;
            m_flags |= changed_track_length;
            m_album_art = p_album_art;
            m_flags |= changed_album_art;
        }

        void on_playback_edited(text_data_type p_metadata,
                                duration_type p_length,
                                album_art_data_type p_album_art = {}) {
            m_text_data = p_metadata;
            m_flags |= changed_text_data;
            m_length = p_length;
            m_flags |= changed_track_length;
            m_album_art = p_album_art;
            m_flags |= changed_album_art;
        }

        void on_playback_dynamic_info(text_data_type p_metadata,
                                      duration_type p_length) {
            m_text_data = p_metadata;
            m_flags |= changed_text_data;
            m_length = p_length;
            m_flags |= changed_track_length;
        }

        void on_album_art(album_art_data_type p_album_art) noexcept {
            m_album_art = p_album_art;
            m_flags |= changed_album_art;
        }

        constexpr bool has_changed(flags_type p_flags) const noexcept {
            return m_flags & p_flags;
        }

        constexpr bool has_changed() const noexcept {
            return m_flags != changed_none;
        }

        constexpr bool has_track_changed() const noexcept {
            return has_changed(changed_track);
        }

        constexpr bool has_play_state_changed() const noexcept {
            return has_changed(changed_play_state);
        }

        constexpr bool has_playback_time_changed() const noexcept {
            return has_changed(changed_playback_time);
        }

        constexpr bool has_track_length_changed() const noexcept {
            return has_changed(changed_track_length);
        }

        constexpr bool has_text_data_changed() const noexcept {
            return has_changed(changed_text_data);
        }

        constexpr bool has_album_art_changed() const noexcept {
            return has_changed(changed_album_art);
        }

        constexpr auto get_flags() const noexcept {
            return m_flags;
        }

        constexpr auto get_stopped() const noexcept {
            return m_stopped;
        }

        constexpr auto get_paused() const noexcept {
            return m_paused;
        }

        constexpr auto get_playback_time() const noexcept {
            return m_playback_time;
        }

        constexpr auto get_track_length() const noexcept {
            return m_length;
        }

        constexpr const auto& get_text_data() const noexcept {
            return m_text_data;
        }

        constexpr const auto& get_album_art() const noexcept {
            return m_album_art;
        }

    private:
        flags_type          m_flags        { changed_none };
        bool                m_stopped      { true };
        bool                m_paused       { false };
        duration_type       m_playback_time{ 0 };
        duration_type       m_length       { 0 };
        text_data_type      m_text_data    {};
        album_art_data_type m_album_art    {};
    }; // class cached_data final
} // namespace foobar

#endif // GUID_88E2DFF8_7D01_46F7_9FAD_032DD30B9368
