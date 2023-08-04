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
#include "foobar/Config/foobar_config_general.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/Config/foobar_config_general.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using native_config_type = typename foobar::Config::GeneralConfig::native_config_type;

    inline static constexpr const auto vGeneralConfigVersion{ native_config_type::Version };
    inline static constexpr const auto nGeneralConfigSize   { sizeof(native_config_type)  };

    using cfg_id_type = GUID;

    //**************************************************************************
    // cfg_ids
    //**************************************************************************
    struct cfg_ids final {
        //---------------------------------------
        struct cfg_display_ids final {
            cfg_id_type m_bVSync             { 0 };
            cfg_id_type m_bForceToForeground { 0 };
            cfg_id_type m_bBackgroundMode    { 0 };
            cfg_id_type m_bBackgroundOnPause { 0 };
            cfg_id_type m_bPopUpOnTrackChange{ 0 };
        }; // struct cfg_display_ids final
        //---------------------------------------

        struct cfg_canvas_ids final {
            struct cfg_wallpaper_ids final {
                cfg_id_type m_Mode         { 0 };
                cfg_id_type m_AlbumArtType { 0 };
                cfg_id_type m_File         { 0 };
                cfg_id_type m_bStretchToFit{ 0 };
            }; // struct cfg_wallpaper_ids final

            cfg_id_type       m_bPreferHardwareCanvas{ 0 };
            cfg_id_type       m_bUseTrailEffect      { 0 };
            cfg_wallpaper_ids m_Wallpaper            { };
        }; // struct cfg_canvas_ids final

        struct cfg_visualisation_ids final {
            struct cfg_start_ids final {
                cfg_id_type m_bRememberLast{ 0 };
                cfg_id_type m_LastMode     { 0 };
                cfg_id_type m_LastIndex    { 0 };
            }; // struct cfg_start_ids final

            struct cfg_auto_change_ids final {
                cfg_id_type m_bEnable       { 0 };
                cfg_id_type m_fChangeSeconds{ 0 };
            };

            struct cfg_on_track_change_ids {
                cfg_id_type m_bShowTrackDetails{ 0 };
                cfg_id_type m_fShowSeconds     { 0 };
            }; // struct cfg_on_track_change_ids final

            cfg_id_type             m_DisplayMode  { 0 };
            cfg_start_ids           m_Start        { 0 };
            cfg_auto_change_ids     m_AutoChange   { 0 };
            cfg_on_track_change_ids m_OnTrackChange{ 0 };
        }; // struct cfg_visualisation_ids final

        cfg_id_type           m_bExpertMode  { 0 };
        cfg_display_ids       m_Display      { };
        cfg_canvas_ids        m_Canvas       { };
        cfg_visualisation_ids m_Visualisation{ };
    }; // struct cfg_ids final

    //**************************************************************************
    // cfg_general
    //**************************************************************************
    class cfg_general final {
    public:
        using native_config = typename native_config_type;

        //---------------------------------------
        class cfg_display final {
        public:
            using native_config = decltype(native_config::m_Display);
            using cfg_ids       = decltype(cfg_ids::m_Display);

        public:
            cfg_display(const cfg_ids& ids,
                        const native_config& defaults);

            void cfg_save   (const native_config& native);
            void cfg_load   (      native_config& native) const;
            void cfg_default(      native_config& native) const noexcept;

        private:
            cfg_bool m_bVSync;
            cfg_bool m_bForceToForeground;
            cfg_bool m_bBackgroundMode;
            cfg_bool m_bBackgroundOnPause;
            cfg_bool m_bPopUpOnTrackChange;
        }; // class cfg_display final
        //---------------------------------------

        //---------------------------------------
        class cfg_canvas final {
        public:
            using native_config = decltype(native_config::m_Canvas);
            using cfg_ids       = decltype(cfg_ids::m_Canvas);

            //-------------------------
            class cfg_wallpaper final {
            public:
                using native_config = decltype(native_config::m_Wallpaper);
                using cfg_ids       = decltype(cfg_ids::m_Wallpaper);
                using cfg_mode      = foobar::Config::cfg_type_t<decltype(native_config::m_Mode)>;
                using cfg_album_art = foobar::Config::cfg_type_t<decltype(native_config::m_AlbumArtType)>;

            public:
                cfg_wallpaper(const cfg_ids& ids,
                              const native_config& defaults);

                void cfg_save   (const native_config& native);
                void cfg_load   (      native_config& native) const;
                void cfg_default(      native_config& native) const noexcept;

            private:
                cfg_mode      m_Mode;
                cfg_album_art m_AlbumArtType;
                cfg_string    m_File;
                cfg_bool      m_bStretchToFit;
            }; // class cfg_wallpaper final
            //-------------------------

        public:
            cfg_canvas(const cfg_ids& ids,
                       const native_config& defaults);

            void cfg_save   (const native_config& native);
            void cfg_load   (      native_config& native) const;
            void cfg_default(      native_config& native) const noexcept;

        private:
            cfg_bool      m_bPreferHardwareCanvas;
            cfg_bool      m_bUseTrailEffect;
            cfg_wallpaper m_Wallpaper;
        }; // class cfg_canvas final
        //---------------------------------------

        //---------------------------------------
        class cfg_visualisation final {
        public:
            using native_config    = decltype(native_config::m_Visualisation);
            using cfg_ids          = decltype(cfg_ids::m_Visualisation);
            using cfg_display_mode = foobar::Config::cfg_type_t<decltype(native_config::m_DisplayMode)>;

            //-------------------------
            class cfg_auto_change final {
            public:
                using native_config = decltype(native_config::m_AutoChange);
                using cfg_ids       = decltype(cfg_ids::m_AutoChange);

            public:
                cfg_auto_change(const cfg_ids& ids,
                                const native_config& defaults);

                void cfg_save   (const native_config& native);
                void cfg_load   (      native_config& native) const;
                void cfg_default(      native_config& native) const noexcept;

            private:
                cfg_bool  m_bEnable;
                cfg_float m_fChangeSeconds;
            }; // class cfg_auto_change final
            //-------------------------

            //-------------------------
            class cfg_start final {
            public:
                using native_config = decltype(native_config::m_Start);
                using cfg_ids       = decltype(cfg_ids::m_Start);
                using cfg_mode      = foobar::Config::cfg_type_t<decltype(native_config::m_LastMode)>;

            public:
                cfg_start(const cfg_ids& ids,
                          const native_config& defaults);

                void cfg_save   (const native_config& native);
                void cfg_load   (      native_config& native) const;
                void cfg_default(      native_config& native) const noexcept;

            private:
                cfg_bool m_bRememberLast;
                cfg_mode m_LastMode;
                cfg_uint m_LastIndex;
            }; // class cfg_start final
            //-------------------------

            //-------------------------
            class cfg_on_track_change final {
            public:
                using native_config = decltype(native_config::m_OnTrackChange);
                using cfg_ids = decltype(cfg_ids::m_OnTrackChange);

            public:
                cfg_on_track_change(const cfg_ids& ids,
                                    const native_config& defaults);

                void cfg_save   (const native_config& native);
                void cfg_load   (      native_config& native) const;
                void cfg_default(      native_config& native) const noexcept;

            private:
                cfg_bool  m_bShowTrackDetails;
                cfg_float m_fShowSeconds;
            }; // class cfg_on_track_change final
            //-------------------------

        public:
            cfg_visualisation(const cfg_ids& ids,
                              const native_config& defaults);

            void cfg_save   (const native_config& native);
            void cfg_load   (      native_config& native) const;
            void cfg_default(      native_config& native) const noexcept;

        private:
            cfg_display_mode    m_DisplayMode;
            cfg_start           m_Start;
            cfg_auto_change     m_AutoChange;
            cfg_on_track_change m_OnTrackChange;
        }; // class cfg_visualisation final
        //---------------------------------------

    public:
        cfg_general(const cfg_ids& ids,
                    const native_config& defaults);

        void cfg_save   (const native_config& native);
        void cfg_load   (      native_config& native) const;
        void cfg_default(      native_config& native) const noexcept;

    private:
        cfg_bool          m_bExpertMode;
        cfg_display       m_Display;
        cfg_canvas        m_Canvas;
        cfg_visualisation m_Visualisation;
    }; // class cfg_general final

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_general(const cfg_ids& ids,
                             const native_config& defaults) :
        m_bExpertMode  { ids.m_bExpertMode   , defaults.m_bExpertMode },
        m_Display      { ids.m_Display       , defaults.m_Display },
        m_Canvas       { ids.m_Canvas        , defaults.m_Canvas },
        m_Visualisation{ ids.m_Visualisation , defaults.m_Visualisation } {}

    //------------------------------------------------------

    void cfg_general::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bExpertMode  , native.m_bExpertMode);
        foobar::Config::cfg_save(m_Display      , native.m_Display);
        foobar::Config::cfg_save(m_Canvas       , native.m_Canvas);
        foobar::Config::cfg_save(m_Visualisation, native.m_Visualisation);
    }

    //------------------------------------------------------

    void cfg_general::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bExpertMode  , m_bExpertMode);
        foobar::Config::cfg_load(native.m_Display      , m_Display);
        foobar::Config::cfg_load(native.m_Canvas       , m_Canvas);
        foobar::Config::cfg_load(native.m_Visualisation, m_Visualisation);
    }

    //------------------------------------------------------

    void cfg_general::cfg_default(native_config& native) const noexcept {
        native = native_config{};
        foobar::Config::cfg_default(native.m_Display      , m_Display);
        foobar::Config::cfg_default(native.m_Canvas       , m_Canvas);
        foobar::Config::cfg_default(native.m_Visualisation, m_Visualisation);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_canvas::cfg_canvas(const cfg_ids& ids,
                                        const native_config& defaults) :
        m_bPreferHardwareCanvas{ ids.m_bPreferHardwareCanvas, defaults.m_bPreferHardwareCanvas },
        m_bUseTrailEffect      { ids.m_bUseTrailEffect      , defaults.m_bUseTrailEffect },
        m_Wallpaper            { ids.m_Wallpaper            , defaults.m_Wallpaper } {}

    //------------------------------------------------------


    void cfg_general::cfg_canvas::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bPreferHardwareCanvas, native.m_bPreferHardwareCanvas);
        foobar::Config::cfg_save(m_bUseTrailEffect      , native.m_bUseTrailEffect);
        foobar::Config::cfg_save(m_Wallpaper            , native.m_Wallpaper);
    }

    //------------------------------------------------------

    void cfg_general::cfg_canvas::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bPreferHardwareCanvas, m_bPreferHardwareCanvas);
        foobar::Config::cfg_load(native.m_bUseTrailEffect      , m_bUseTrailEffect);
        foobar::Config::cfg_load(native.m_Wallpaper            , m_Wallpaper);
    }

    //------------------------------------------------------

    void cfg_general::cfg_canvas::cfg_default(native_config& native) const noexcept {
        native = native_config{};
        foobar::Config::cfg_default(native.m_Wallpaper, m_Wallpaper);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_display::cfg_display(const cfg_ids& ids,
                                          const native_config& defaults) :
        m_bVSync             { ids.m_bVSync             , defaults.m_bVSync },
        m_bForceToForeground { ids.m_bForceToForeground , defaults.m_bForceToForeground },
        m_bBackgroundMode    { ids.m_bBackgroundMode    , defaults.m_bBackgroundMode },
        m_bBackgroundOnPause { ids.m_bBackgroundOnPause , defaults.m_bBackgroundOnPause },
        m_bPopUpOnTrackChange{ ids.m_bPopUpOnTrackChange, defaults.m_bPopUpOnTrackChange } {}

    //------------------------------------------------------

    void cfg_general::cfg_display::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bVSync             , native.m_bVSync);
        foobar::Config::cfg_save(m_bForceToForeground , native.m_bForceToForeground);
        foobar::Config::cfg_save(m_bBackgroundMode    , native.m_bBackgroundMode);
        foobar::Config::cfg_save(m_bBackgroundOnPause , native.m_bBackgroundOnPause);
        foobar::Config::cfg_save(m_bPopUpOnTrackChange, native.m_bPopUpOnTrackChange);
    }

    //------------------------------------------------------

    void cfg_general::cfg_display::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bVSync             , m_bVSync);
        foobar::Config::cfg_load(native.m_bForceToForeground , m_bForceToForeground);
        foobar::Config::cfg_load(native.m_bBackgroundMode    , m_bBackgroundMode);
        foobar::Config::cfg_load(native.m_bBackgroundOnPause , m_bBackgroundOnPause);
        foobar::Config::cfg_load(native.m_bPopUpOnTrackChange, m_bPopUpOnTrackChange);
    }

    //------------------------------------------------------

    void cfg_general::cfg_display::cfg_default(native_config& native) const noexcept {
        native = native_config{};
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_canvas::cfg_wallpaper::cfg_wallpaper(const cfg_ids& ids,
                                                          const native_config& defaults) :
        m_Mode         { ids.m_Mode         , defaults.m_Mode },
        m_AlbumArtType { ids.m_AlbumArtType , defaults.m_AlbumArtType },
        m_File         { ids.m_File         , foobar::to_utf8(defaults.m_File) },
        m_bStretchToFit{ ids.m_bStretchToFit, defaults.m_bStretchToFit } {}

    //------------------------------------------------------

    void cfg_general::cfg_canvas::cfg_wallpaper::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_Mode         , native.m_Mode);
        foobar::Config::cfg_save(m_AlbumArtType , native.m_AlbumArtType);
        foobar::Config::cfg_save(m_File         , native.m_File);
        foobar::Config::cfg_save(m_bStretchToFit, native.m_bStretchToFit);
    }

    //------------------------------------------------------

    void cfg_general::cfg_canvas::cfg_wallpaper::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_Mode         , m_Mode);
        foobar::Config::cfg_load(native.m_AlbumArtType , m_AlbumArtType);
        foobar::Config::cfg_load(native.m_File         , m_File);
        foobar::Config::cfg_load(native.m_bStretchToFit, m_bStretchToFit);
    }

    //------------------------------------------------------

    void cfg_general::cfg_canvas::cfg_wallpaper::cfg_default(native_config& native) const noexcept {
        native = native_config{};
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_visualisation::cfg_visualisation(const cfg_ids& ids,
                                                      const native_config& defaults) :
        m_DisplayMode  { ids.m_DisplayMode  , defaults.m_DisplayMode },
        m_Start        { ids.m_Start        , defaults.m_Start },
        m_AutoChange   { ids.m_AutoChange   , defaults.m_AutoChange },
        m_OnTrackChange{ ids.m_OnTrackChange, defaults.m_OnTrackChange } {}

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_DisplayMode           , native.m_DisplayMode);
        foobar::Config::cfg_save(m_Start                 , native.m_Start);
        foobar::Config::cfg_save(m_AutoChange            , native.m_AutoChange);
        foobar::Config::cfg_save(m_OnTrackChange         , native.m_OnTrackChange);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_DisplayMode  , m_DisplayMode);
        foobar::Config::cfg_load(native.m_Start        , m_Start);
        foobar::Config::cfg_load(native.m_AutoChange   , m_AutoChange);
        foobar::Config::cfg_load(native.m_OnTrackChange, m_OnTrackChange);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_default(native_config& native) const noexcept {
        native = native_config{};
        foobar::Config::cfg_default(native.m_Start,         m_Start);
        foobar::Config::cfg_default(native.m_AutoChange,    m_AutoChange);
        foobar::Config::cfg_default(native.m_OnTrackChange, m_OnTrackChange);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_visualisation::cfg_start::cfg_start(const cfg_ids& ids,
                                                         const native_config& defaults) :
        m_bRememberLast{ ids.m_bRememberLast, defaults.m_bRememberLast },
        m_LastMode     { ids.m_LastMode     , defaults.m_LastMode },
        m_LastIndex    { ids.m_LastIndex    , defaults.m_LastIndex } {}

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_start::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bRememberLast, native.m_bRememberLast);
        foobar::Config::cfg_save(m_LastMode     , native.m_LastMode);
        foobar::Config::cfg_save(m_LastIndex    , native.m_LastIndex);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_start::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bRememberLast, m_bRememberLast);
        foobar::Config::cfg_load(native.m_LastMode     , m_LastMode);
        foobar::Config::cfg_load(native.m_LastIndex    , m_LastIndex);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_start::cfg_default(native_config& native) const noexcept {
        native = native_config{};
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_visualisation::cfg_auto_change::cfg_auto_change(const cfg_ids& ids,
                                                                             const native_config& defaults) :
        m_bEnable       { ids.m_bEnable       , defaults.m_bEnable },
        m_fChangeSeconds{ ids.m_fChangeSeconds, defaults.m_fChangeSeconds } {}

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_auto_change::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bEnable,        native.m_bEnable);
        foobar::Config::cfg_save(m_fChangeSeconds, native.m_fChangeSeconds);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_auto_change::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bEnable,        m_bEnable);
        foobar::Config::cfg_load(native.m_fChangeSeconds, m_fChangeSeconds);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_auto_change::cfg_default(native_config& native) const noexcept {
        native = native_config{};
    }

    //------------------------------------------------------
    //------------------------------------------------------

    cfg_general::cfg_visualisation::cfg_on_track_change::cfg_on_track_change(const cfg_ids& ids,
                                                                             const native_config& defaults) :
        m_bShowTrackDetails{ ids.m_bShowTrackDetails    , defaults.m_bShowTrackDetails },
        m_fShowSeconds     { ids.m_fShowSeconds, defaults.m_fShowSeconds } {}

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_on_track_change::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bShowTrackDetails, native.m_bShowTrackDetails);
        foobar::Config::cfg_save(m_fShowSeconds, native.m_fShowSeconds);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_on_track_change::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bShowTrackDetails, m_bShowTrackDetails);
        foobar::Config::cfg_load(native.m_fShowSeconds, m_fShowSeconds);
    }

    //------------------------------------------------------

    void cfg_general::cfg_visualisation::cfg_on_track_change::cfg_default(native_config& native) const noexcept {
        native = native_config{};
    }
}

/******************************************************************************
 ******************************************************************************
 ******************************************************************************/

namespace {
    //**************************************************************************
    // Config IDs
    //**************************************************************************
    static constexpr const cfg_ids id_GeneralConfig{
        cfg_id_type{ 0xb95d647c, 0xdcad, 0x46a0, { 0x97, 0xcc, 0xba, 0xda, 0x7f, 0xc9, 0x5f, 0x6c } },
        cfg_ids::cfg_display_ids{
            cfg_id_type{ 0x651a321d, 0x70cd, 0x44f4, { 0x9c, 0xb4, 0xb7, 0x99, 0x54, 0x6b, 0x90, 0x7b } },
            cfg_id_type{ 0x2d67d78f, 0x38dc, 0x40e3, { 0x81, 0xd6, 0xa0, 0x50, 0x08, 0x9a, 0x43, 0xa4 } },
            cfg_id_type{ 0xad285780, 0x976a, 0x4d93, { 0xb9, 0x57, 0xa0, 0x2e, 0x39, 0x11, 0x43, 0xd2 } },
            cfg_id_type{ 0x8dfb0108, 0x8a98, 0x4b3d, { 0x87, 0xc3, 0x26, 0xeb, 0x1b, 0xcc, 0xed, 0x0f } },
            cfg_id_type{ 0xb725e821, 0x5508, 0x47ae, { 0xa6, 0x45, 0x92, 0x7b, 0x95, 0x68, 0xff, 0x3f } },
        },
        cfg_ids::cfg_canvas_ids{
            cfg_id_type{ 0x84fcf324, 0x368e, 0x4bbc, { 0x87, 0x03, 0x39, 0xed, 0xfb, 0x5a, 0xe1, 0x8a } },
            cfg_id_type{ 0xa22ebdda, 0x7fb3, 0x4dd6, { 0xa1, 0xb9, 0x7e, 0x88, 0x20, 0xb2, 0x37, 0x0c } },
            cfg_ids::cfg_canvas_ids::cfg_wallpaper_ids{
                cfg_id_type{ 0x5eb12d20, 0xb2a1, 0x4a67, { 0x8a, 0x96, 0xb1, 0x76, 0x40, 0x9b, 0xb2, 0xc0 } },
                cfg_id_type{ 0x43f9fe3e, 0x6ccb, 0x469f, { 0xaf, 0x50, 0xb7, 0x2b, 0x74, 0x36, 0xb6, 0xe7 } },
                cfg_id_type{ 0x585b782e, 0x35d4, 0x40a7, { 0x85, 0x0f, 0xa2, 0x56, 0xd3, 0xd0, 0xf1, 0xa3 } },
                cfg_id_type{ 0xb8efeef7, 0x1b82, 0x4711, { 0xb2, 0x03, 0x71, 0x5c, 0x17, 0xb4, 0xed, 0x46 } },
            },
        },
        cfg_ids::cfg_visualisation_ids{
            cfg_id_type{ 0x6a7021cd, 0x5322, 0x4b86, { 0x85, 0x92, 0x2b, 0xe1, 0x73, 0xa3, 0xed, 0x22 } },
            cfg_ids::cfg_visualisation_ids::cfg_start_ids{
                cfg_id_type{ 0xe6c234f1, 0xc9bf, 0x417d, { 0x9b, 0x52, 0x8b, 0xd6, 0x18, 0x4c, 0x85, 0x23 } },
                cfg_id_type{ 0x85cf3f0c, 0xf95c, 0x467b, { 0x93, 0xb0, 0x14, 0xa1, 0x60, 0x89, 0xef, 0xe7 } },
                cfg_id_type{ 0xc395c508, 0xbdeb, 0x4a0e, { 0xb6, 0x88, 0x36, 0xfd, 0xc3, 0x52, 0x73, 0x53 } },
            },
            cfg_ids::cfg_visualisation_ids::cfg_auto_change_ids{
                cfg_id_type{ 0x965bd450, 0x4775, 0x4703, { 0xb7, 0x56, 0xbf, 0xa8, 0xdc, 0xb8, 0x7f, 0x48 } },
                cfg_id_type{ 0xb74e7ea3, 0x824c, 0x43dd, { 0xad, 0x1c, 0x44, 0xdb, 0x8f, 0x1d, 0xbe, 0x85 } },
            },
            cfg_ids::cfg_visualisation_ids::cfg_on_track_change_ids{
                cfg_id_type{ 0x965bd450, 0x4775, 0x4703, { 0xb7, 0x56, 0xbf, 0xa8, 0xdc, 0xb8, 0x7f, 0x48 } },
                cfg_id_type{ 0xb74e7ea3, 0x824c, 0x43dd, { 0xad, 0x1c, 0x44, 0xdb, 0x8f, 0x1d, 0xbe, 0x85 } },
            },
        },
    };

    //------------------------------------------------------

    static constexpr const cfg_id_type id_GeneralConfigVersion{ 0xfc295fe5, 0xdadb, 0x44a2, { 0xb5, 0x2d, 0x37, 0x37, 0xcb, 0x5f, 0xb9, 0x72 } };
    static constexpr const cfg_id_type id_GeneralConfigSize   { 0x2595e881, 0xb485, 0x46de, { 0x89, 0x97, 0x6c, 0xe6, 0x12, 0xe1, 0x1d, 0x7a } };

    //**************************************************************************
    // Storage
    //**************************************************************************
    static cfg_general cfg_GeneralConfig{ id_GeneralConfig, native_config_type{} };

    //------------------------------------------------------

    cfg_uint cfg_GeneralConfigVersion{ id_GeneralConfigVersion, vGeneralConfigVersion };
    cfg_uint cfg_GeneralConfigSize   { id_GeneralConfigSize,    nGeneralConfigSize    };
} // namespace <anonymous>

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // GeneralConfig
    //**************************************************************************
    GeneralConfig::native_config_type GeneralConfig::load() {
        native_config_type native{};
        if (cfg_GeneralConfigVersion == vGeneralConfigVersion &&
            cfg_GeneralConfigSize    == nGeneralConfigSize) {
            foobar::Config::cfg_load(native, cfg_GeneralConfig);
        } else {
            // Convert config to newer version when possible...
            SPDLOG_ERROR("Error loading config, version/size mismatch: expected v{}/{}b, got v{}/{}b. Config has been reset.",
                         vGeneralConfigVersion,
                         nGeneralConfigSize,
                         cfg_GeneralConfigVersion.get_value(),
                         cfg_GeneralConfigSize.get_value());
            set_default(native);
            foobar::Config::cfg_save(cfg_GeneralConfig, native);
        }
        return native;
    }

    //--------------------------------------------------------------------------

    void GeneralConfig::save(const native_config_type& config) {
        foobar::Config::cfg_save(cfg_GeneralConfigVersion, vGeneralConfigVersion);
        foobar::Config::cfg_save(cfg_GeneralConfigSize, nGeneralConfigSize);
        foobar::Config::cfg_save(cfg_GeneralConfig, config);
    }

    //--------------------------------------------------------------------------

    void GeneralConfig::set_default(native_config_type& native) noexcept {
        native = native_config_type{};
        foobar::Config::cfg_default(native, cfg_GeneralConfig);
    }
} // namespace foobar::Config
