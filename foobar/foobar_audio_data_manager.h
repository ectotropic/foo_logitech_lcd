#pragma once
#ifndef GUID_A1747D11_3A20_4AB4_A121_1BFB8EB38D9D
#define GUID_A1747D11_3A20_4AB4_A121_1BFB8EB38D9D
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
#include "foobar/foobar_core_util.h"
#include "foobar/foobar_formatter_util.h"
#include "foobar/foobar_visualisation_util.h"
#include "foobar/foobar_album_art_util.h"
#include "foobar/foobar_cached_metadata.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Thread/Thread_CriticalSection.h"
#include "Config/Config_TrackDetails.h"
#include "Config/Config_General.h"
//--------------------------------------

namespace foobar {
    //**************************************************************************
    // foobar_audio_data_manager
    //**************************************************************************
    class foobar_audio_data_manager final :
        public ::play_callback,
        public ::now_playing_album_art_notify,
        public ::Audio::IAudioDataManager {
    public:
        using fb_duration_type            = double;
        using fb_sample_type              = ::audio_sample;
        using fb_size_type                = ::t_size;
        using fb_title_formatter          = ::foobar::metadata::title_formatter;
        using fb_script_source            = typename fb_title_formatter::script_source_type;
        using fb_formatted                = typename fb_title_formatter::formatted_type;
        using fb_formatted_array          = ::pfc::array_t<fb_formatted>;
        using fb_cached_metadata          = ::foobar::metadata::cached_data;
        using fb_metadata_ptr             = ::metadb_handle_ptr;
        using fb_visualisation_stream_ptr = ::foobar::visualisation::stream_ptr_t;
        using fb_album_art_id_list        = ::foobar::metadata::album_art::id_list_t;

    private:
        using critical_section            = ::Windows::Thread::CriticalSection;

    public:
        static void initialise() {
            IAudioDataManager::initialise<foobar_audio_data_manager>();
            auto pMgr{ IAudioDataManager::instance_peek() };
            if (pMgr) { pMgr->Initialise(); }
        }

    public:
        foobar_audio_data_manager(singleton_constructor_tag /*tag*/) noexcept {};
        ~foobar_audio_data_manager() noexcept = default;

    public: // play_callback
        virtual     void    on_playback_new_track           (metadb_handle_ptr p_track)                   override;
        virtual     void    on_playback_edited              (metadb_handle_ptr p_track)                   override;
        virtual     void    on_playback_time                (double p_time)                      noexcept override;
        virtual     void    on_playback_seek                (double p_time)                      noexcept override;
        virtual     void    on_playback_stop                (play_control::t_stop_reason reason) noexcept override;
        virtual     void    on_playback_dynamic_info_track  (const file_info& pInfo)                      override;
        virtual     void    on_playback_starting            (play_control::t_track_command p_command,
                                                             bool p_paused)                      noexcept override;
        virtual     void    on_playback_pause               (bool p_state)                       noexcept override;

        // (unused)
        virtual     void    on_volume_change                (float /*p_new_val*/)                noexcept override {}
        virtual     void    on_playback_dynamic_info        (const file_info& /*pInfo*/)         noexcept override {}

    private:
        void on_metadata_change(metadb_handle_ptr p_track,
                                bool p_new_track);

    public: // now_playing_album_art_notify
        virtual     void    on_album_art                    (album_art_data::ptr data)                    override;

    protected: // IAudioDataManager
        virtual void OnInitialise  () override;
        virtual void OnUninitialise() override;

        virtual void OnUpdateConfig() override;

        virtual void OnUpdate(const update_params& params,
                              const update_hint_params& hints)  override;

        virtual bool IsValid() const noexcept override  {
            return m_pVisStream.is_valid();
        }

    private:
        fb_visualisation_stream_ptr m_pVisStream        {};
        fb_metadata_ptr             m_pCurrentTrack     {};
        fb_title_formatter          m_TitleFormatter    {};
        fb_cached_metadata          m_CachedMetadata    {};
        fb_album_art_id_list        m_AlbumArtTypeIDList{};

    private:
        mutable critical_section m_CriticalSection{};
    }; // class foobar_audio_data_manager final
} // namespace foobar

#endif // GUID_A1747D11_3A20_4AB4_A121_1BFB8EB38D9D