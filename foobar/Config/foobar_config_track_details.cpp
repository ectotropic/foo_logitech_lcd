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
#include "foobar/Config/foobar_config_track_details.h"
#include "foobar/Config/foobar_config_common.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Types & Constants
    //**************************************************************************
    using native_config_type = typename foobar::Config::TrackDetailsConfig::native_config_type;
    using native_array_type  = typename foobar::Config::TrackDetailsConfig::native_array_type;
    using native_enum_type   = typename foobar::Config::TrackDetailsConfig::native_enum_type;

    inline static constexpr const auto cTrackDetailsPageCount{ native_enum_type::count()   };
    inline static constexpr const auto vPageConfigVersion    { native_config_type::Version };
    inline static constexpr const auto nPageConfigSize       { sizeof(native_config_type)  };

    using  cfg_id_type = GUID;

    struct cfg_ids;
    using  cfg_page_ids = std::array<cfg_ids, cTrackDetailsPageCount>;

    class  cfg_track_details;
    using  cfg_pages = std::array<cfg_track_details, cTrackDetailsPageCount>;

    //**************************************************************************
    // cfg_ids
    //**************************************************************************
    struct cfg_ids final {
        //---------------------------------------
        struct cfg_text_ids final {
            cfg_id_type m_Vertical{ 0 };
            cfg_id_type m_Horizontal{ 0 };
            cfg_id_type m_bAllowOverlap{ 0 };
            cfg_id_type m_bClearBackground{ 0 };
            cfg_id_type m_bRestrictedLineCount{ 0 };
            cfg_id_type m_Font{ 0 };
            cfg_id_type m_Color{ 0 };
            cfg_id_type m_BackgroundColor{ 0 };
            cfg_id_type m_Format{ 0 };
        }; // struct cfg_text_ids final
        //---------------------------------------

        cfg_id_type  m_bEnabled{ 0 };
        cfg_id_type  m_bSyncProgressBarConfig{ 0 };
        cfg_id_type  m_ProgressBar{ 0 };
        cfg_text_ids m_Text{};
    }; // struct cfg_ids final

    //**************************************************************************
    // cfg_track_details
    //**************************************************************************
    class cfg_track_details final {
    public:
        using native_config    = typename native_config_type;
        using cfg_progress_bar = cfg_struct_t<decltype(native_config::m_ProgressBar)>;

        //---------------------------------------
        class cfg_text final {
        public:
            using cfg_ids        = decltype(cfg_ids::m_Text);
            using native_config  = decltype(native_config::m_Text);

            using cfg_font       = cfg_struct_t<decltype(native_config::m_Font)>;
            using cfg_vertical   = cfg_struct_t<decltype(native_config::m_Vertical)>;
            using cfg_horizontal = cfg_struct_t<decltype(native_config::m_Horizontal)>;

        public:
            cfg_text(const cfg_ids& guids,
                     const native_config& defaults,
                     const char* format);

            void cfg_save   (const native_config& native);
            void cfg_load   (      native_config& native) const;
            void cfg_default(      native_config& native) const;

        private:
            cfg_vertical   m_Vertical;
            cfg_horizontal m_Horizontal;
            cfg_bool       m_bAllowOverlap;
            cfg_bool       m_bClearBackground;
            cfg_bool       m_bRestrictedLineCount;
            cfg_font       m_Font;
            cfg_uint       m_Color;
            cfg_uint       m_BackgroundColor;
            cfg_string     m_Format;
            pfc::string8   m_DefaultFormat{};
        }; // class cfg_text final
        //---------------------------------------

    public:
        cfg_track_details(const cfg_ids& guids,
                          const native_config& defaults,
                          const char* format);

        void cfg_save   (const native_config& native);
        void cfg_load   (      native_config& native) const;
        void cfg_default(      native_config& native) const;

    private:
        cfg_bool         m_bEnabled;
        cfg_progress_bar m_ProgressBar;
        cfg_bool         m_bSyncProgressBarConfig;
        cfg_text         m_Text;
    }; // class cfg_track_details final

    //-----------------------------------------------------
    //-----------------------------------------------------

    cfg_track_details::cfg_track_details(const cfg_ids& guids,
                                         const native_config& defaults,
                                         const char* format) :
        m_bEnabled              { guids.m_bEnabled              , defaults.m_bEnabled },
        m_bSyncProgressBarConfig{ guids.m_bSyncProgressBarConfig, defaults.m_bSyncProgressBarConfig },
        m_ProgressBar           { guids.m_ProgressBar           , defaults.m_ProgressBar },
        m_Text                  { guids.m_Text                  , defaults.m_Text, format } {}


    //-----------------------------------------------------

    void cfg_track_details::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_bEnabled              , native.m_bEnabled);
        foobar::Config::cfg_save(m_bSyncProgressBarConfig, native.m_bSyncProgressBarConfig);
        foobar::Config::cfg_save(m_ProgressBar           , native.m_ProgressBar);
        foobar::Config::cfg_save(m_Text                  , native.m_Text);
    }

    //-----------------------------------------------------

    void cfg_track_details::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_bEnabled              , m_bEnabled);
        foobar::Config::cfg_load(native.m_bSyncProgressBarConfig, m_bSyncProgressBarConfig);
        foobar::Config::cfg_load(native.m_ProgressBar           , m_ProgressBar);
        foobar::Config::cfg_load(native.m_Text                  , m_Text);
    }

    //-----------------------------------------------------

    void cfg_track_details::cfg_default(native_config& native) const {
        native = native_config{};
        foobar::Config::cfg_default(native.m_Text, m_Text);
    }

    //-----------------------------------------------------
    //-----------------------------------------------------

    cfg_track_details::cfg_text::cfg_text(const cfg_ids& guids,
                                          const native_config& defaults,
                                          const char* format) :
        m_Vertical            { guids.m_Vertical            , defaults.m_Vertical },
        m_Horizontal          { guids.m_Horizontal          , defaults.m_Horizontal },
        m_bAllowOverlap       { guids.m_bAllowOverlap       , defaults.m_bAllowOverlap },
        m_bClearBackground    { guids.m_bClearBackground    , defaults.m_bClearBackground },
        m_bRestrictedLineCount{ guids.m_bRestrictedLineCount, defaults.m_bRestrictedLineCount },
        m_Font                { guids.m_Font                , defaults.m_Font },
        m_Color               { guids.m_Color               , defaults.m_Color },
        m_BackgroundColor     { guids.m_BackgroundColor     , defaults.m_BackgroundColor },
        m_Format              { guids.m_Format              , format },
        m_DefaultFormat       { format } {}

    //-----------------------------------------------------

    void cfg_track_details::cfg_text::cfg_save(const native_config& native) {
        foobar::Config::cfg_save(m_Vertical            , native.m_Vertical);
        foobar::Config::cfg_save(m_Horizontal          , native.m_Horizontal);
        foobar::Config::cfg_save(m_bAllowOverlap       , native.m_bAllowOverlap);
        foobar::Config::cfg_save(m_bClearBackground    , native.m_bClearBackground);
        foobar::Config::cfg_save(m_bRestrictedLineCount, native.m_bRestrictedLineCount);
        foobar::Config::cfg_save(m_Font                , native.m_Font);
        foobar::Config::cfg_save(m_Color               , native.m_Color);
        foobar::Config::cfg_save(m_BackgroundColor     , native.m_BackgroundColor);
        foobar::Config::cfg_save(m_Format              , native.m_Format);
    }

    //-----------------------------------------------------

    void cfg_track_details::cfg_text::cfg_load(native_config& native) const {
        foobar::Config::cfg_load(native.m_Vertical            , m_Vertical);
        foobar::Config::cfg_load(native.m_Horizontal          , m_Horizontal);
        foobar::Config::cfg_load(native.m_bAllowOverlap       , m_bAllowOverlap);
        foobar::Config::cfg_load(native.m_bClearBackground    , m_bClearBackground);
        foobar::Config::cfg_load(native.m_bRestrictedLineCount, m_bRestrictedLineCount);
        foobar::Config::cfg_load(native.m_Font                , m_Font);
        foobar::Config::cfg_load(native.m_Color               , m_Color);
        foobar::Config::cfg_load(native.m_BackgroundColor     , m_BackgroundColor);
        foobar::Config::cfg_load(native.m_Format              , m_Format);
    }

    //-----------------------------------------------------

    void cfg_track_details::cfg_text::cfg_default(native_config& native) const {
        native = native_config{};
        foobar::Config::cfg_default(native.m_Format, m_DefaultFormat);
    }
} // namespace <anonymous>

//==============================================================================

namespace {
    //**************************************************************************
    // Page Config IDs
    //**************************************************************************
    static constexpr const cfg_page_ids id_TrackDetails{
        //--------------------------------------------------
        // native_enum_type::Page1
        cfg_ids{
            cfg_id_type{ 0x3af52a15, 0x3fc4, 0x4fb5, { 0xa7, 0xcf, 0x7b, 0x3b, 0xda, 0x83, 0x9b, 0xf7 } },
            cfg_id_type{ 0x4d2b5e17, 0x1585, 0x44f4, { 0x94, 0x1e, 0x00, 0x67, 0x1b, 0x4f, 0x6a, 0x3f } },
            cfg_id_type{ 0x456d3286, 0x6b60, 0x49e6, { 0x89, 0x96, 0x80, 0x15, 0xcd, 0x01, 0xb4, 0x46 } },
            {
                cfg_id_type{ 0x74fd14e5, 0xea71, 0x4db6, { 0x80, 0xe9, 0x42, 0xb8, 0x06, 0x7a, 0x94, 0xd2 } },
                cfg_id_type{ 0x6d0799c0, 0x1491, 0x4875, { 0x82, 0x88, 0xde, 0xf2, 0x00, 0xed, 0x53, 0xbd } },
                cfg_id_type{ 0x764e6474, 0x8735, 0x498d, { 0x9a, 0x98, 0x3e, 0x03, 0x2e, 0x5a, 0x09, 0x1a } },
                cfg_id_type{ 0xfcd8792a, 0x7db7, 0x4841, { 0xab, 0xb7, 0xda, 0xa9, 0xc9, 0x91, 0xbe, 0x3e } },
                cfg_id_type{ 0x48f1c00d, 0xf911, 0x4a85, { 0xb3, 0x42, 0x82, 0x9c, 0x1f, 0x19, 0x9f, 0xba } },
                cfg_id_type{ 0x7cb9da1b, 0xb438, 0x4bc4, { 0xa0, 0x37, 0xf5, 0x70, 0x91, 0xaf, 0xd0, 0x6e } },
                cfg_id_type{ 0x35ebbe1b, 0x3fd3, 0x40fb, { 0xb8, 0x30, 0x06, 0x42, 0x51, 0xc8, 0x9e, 0xca } },
                cfg_id_type{ 0x92b99df9, 0xaa8b, 0x440f, { 0xb4, 0x2d, 0x18, 0x55, 0x16, 0x00, 0x5a, 0x2c } },
                cfg_id_type{ 0x21d1f91e, 0x304a, 0x42d0, { 0x9a, 0x82, 0x5c, 0x22, 0xa0, 0xff, 0x85, 0xfa } },
            },
        },

        //--------------------------------------------------
        // native_enum_type::Page2
        cfg_ids{
            cfg_id_type{ 0x23122e67, 0x9e92, 0x4dee, { 0x84, 0xd4, 0x2e, 0x2c, 0xf4, 0xdb, 0x08, 0xaf } },
            cfg_id_type{ 0x5f29a33b, 0xbf30, 0x44bb, { 0x9b, 0xc8, 0x24, 0x7d, 0x4a, 0x24, 0x75, 0x85 } },
            cfg_id_type{ 0xa15443be, 0xcd9b, 0x47ac, { 0x88, 0x2a, 0x81, 0x93, 0xcd, 0xdc, 0x9b, 0x08 } },
            {
                cfg_id_type{ 0x213c3ef1, 0x230e, 0x4ea4, { 0xa6, 0x02, 0x55, 0x6a, 0xdb, 0x09, 0x94, 0x63 } },
                cfg_id_type{ 0x9f60dc86, 0x60e9, 0x4e3b, { 0xbb, 0xfc, 0x70, 0x9f, 0xc0, 0xc7, 0x3a, 0x13 } },
                cfg_id_type{ 0x4538b6cb, 0xb9a6, 0x42b6, { 0xb9, 0xb3, 0xd7, 0x78, 0x5d, 0x13, 0x6a, 0x13 } },
                cfg_id_type{ 0x2735c174, 0x0938, 0x4bf0, { 0x9a, 0x6d, 0x55, 0x1f, 0x07, 0x90, 0x07, 0x0d } },
                cfg_id_type{ 0x8505b3b1, 0xb9a8, 0x4098, { 0x9c, 0x42, 0x1d, 0xa1, 0x36, 0xd1, 0x42, 0x4a } },
                cfg_id_type{ 0xeb10b71f, 0x9583, 0x4e99, { 0x8b, 0x86, 0xa0, 0x9f, 0xda, 0x12, 0xbe, 0xac } },
                cfg_id_type{ 0xcae41c93, 0x7d8b, 0x4326, { 0x80, 0x03, 0xc1, 0x79, 0xb4, 0x86, 0x8a, 0x8c } },
                cfg_id_type{ 0x1c50d62c, 0x2871, 0x4552, { 0x9c, 0xf3, 0xc0, 0x5d, 0x73, 0x79, 0xc2, 0xd1 } },
                cfg_id_type{ 0x5352d1c5, 0xb66c, 0x40a4, { 0xad, 0xb7, 0xfa, 0x81, 0x0c, 0xeb, 0xa4, 0x72 } },
            },
        },

        //--------------------------------------------------
        // native_enum_type::Page3
        cfg_ids{
            cfg_id_type{ 0x2aaf5b5a, 0x476a, 0x44f6, { 0xa1, 0x0b, 0xc1, 0x27, 0x4e, 0x89, 0x83, 0x45 } },
            cfg_id_type{ 0xf089c5fe, 0x5228, 0x4687, { 0xaa, 0x33, 0x48, 0x4d, 0xee, 0x67, 0xb5, 0xec } },
            cfg_id_type{ 0x3fcb9c56, 0xd169, 0x4f05, { 0xae, 0x10, 0x35, 0xe0, 0x9f, 0x92, 0x04, 0x43 } },
            {
                cfg_id_type{ 0xd5da4d72, 0x3731, 0x497a, { 0xa3, 0x7c, 0x8f, 0x8f, 0x66, 0x92, 0xfa, 0xa1 } },
                cfg_id_type{ 0x5fcad63c, 0x43ec, 0x4304, { 0x8d, 0x68, 0x82, 0xc0, 0x58, 0xb4, 0x3a, 0xf1 } },
                cfg_id_type{ 0x553a9ada, 0x7f82, 0x4a8b, { 0x81, 0xcc, 0x99, 0x84, 0xa9, 0xc9, 0x85, 0x18 } },
                cfg_id_type{ 0xacefa2aa, 0x1296, 0x4376, { 0xb4, 0x53, 0x51, 0x6a, 0x36, 0x91, 0xb6, 0xa9 } },
                cfg_id_type{ 0x191cbc80, 0x7c21, 0x48ab, { 0x87, 0x67, 0xb7, 0xb6, 0x04, 0x87, 0x2e, 0xec } },
                cfg_id_type{ 0x45ea0e30, 0x04b9, 0x48ad, { 0xbc, 0x6f, 0x88, 0x88, 0xb3, 0xe3, 0x06, 0xf9 } },
                cfg_id_type{ 0xb90d8240, 0x3259, 0x4a94, { 0x88, 0x34, 0xcc, 0x1b, 0x92, 0xce, 0x95, 0xf7 } },
                cfg_id_type{ 0xfc1895b1, 0x3323, 0x498d, { 0xa2, 0x61, 0xa6, 0x91, 0xb6, 0x20, 0x7e, 0x26 } },
                cfg_id_type{ 0x86037d92, 0x6a82, 0x44a7, { 0x88, 0x40, 0xe2, 0x12, 0x92, 0xac, 0x12, 0x0d } },
            },
        },

        //--------------------------------------------------
        // native_enum_type::Page4
        cfg_ids{
            cfg_id_type{ 0xf73f84cb, 0x82c5, 0x4a27, { 0x97, 0x8c, 0x3f, 0xfe, 0x9c, 0xd9, 0xdc, 0xf2 } },
            cfg_id_type{ 0x46a49894, 0x181e, 0x44b5, { 0x9a, 0x52, 0x86, 0x21, 0x6e, 0x5f, 0x4f, 0x30 } },
            cfg_id_type{ 0xb8eae928, 0x77c2, 0x4013, { 0x9e, 0xbb, 0x96, 0xd0, 0xad, 0x5f, 0xdc, 0xb7 } },
            {
                cfg_id_type{ 0x85f47fb8, 0xe713, 0x47a6, { 0x84, 0x12, 0x13, 0x6c, 0x2f, 0xd2, 0xa1, 0x57 } },
                cfg_id_type{ 0xfa9a8ef4, 0x58da, 0x4d5d, { 0xb3, 0x65, 0x27, 0xc1, 0x1d, 0xfd, 0x3f, 0xe4 } },
                cfg_id_type{ 0x73288933, 0x23c3, 0x44b9, { 0xa9, 0xe5, 0x33, 0x3a, 0xeb, 0xd9, 0xfc, 0xb1 } },
                cfg_id_type{ 0x31cc4e13, 0x9c20, 0x4f1f, { 0xa5, 0x3b, 0xc3, 0x17, 0xfa, 0x57, 0x7b, 0x0d } },
                cfg_id_type{ 0x271ae2a5, 0x541c, 0x4bf7, { 0x9e, 0xd1, 0x6b, 0xbf, 0xae, 0x23, 0x52, 0x62 } },
                cfg_id_type{ 0xad9bcfbb, 0xb33d, 0x4996, { 0xa4, 0x95, 0x39, 0x38, 0xd9, 0x98, 0xe0, 0x36 } },
                cfg_id_type{ 0x0375db67, 0xd0d0, 0x456c, { 0xb4, 0x82, 0x67, 0x10, 0x39, 0x35, 0xf6, 0x64 } },
                cfg_id_type{ 0xe52224e8, 0x08a1, 0x495a, { 0x85, 0xcb, 0x92, 0x57, 0x26, 0xbf, 0xca, 0x5f } },
                cfg_id_type{ 0x3f18f098, 0x6a92, 0x4507, { 0xaf, 0x0f, 0x87, 0x54, 0xf3, 0x3f, 0x2b, 0xe1 } },
            },
        },
    }; // cfg_page_ids id_TrackDetails

    //------------------------------------------------------

    static constexpr const cfg_id_type id_PageConfigSize   { 0x45505427, 0x8894, 0x4e40, { 0xa8, 0x74, 0x2c, 0xfd, 0xae, 0x7f, 0x2f, 0xfb } };
    static constexpr const cfg_id_type id_PageConfigVersion{ 0xb070b721, 0x2dd9, 0x44e5, { 0x97, 0x48, 0x18, 0xa8, 0xe6, 0xdd, 0x32, 0xbd } };

    //**************************************************************************
    // Storage
    //
    // NOTE: Track Details text format is specific to the program driving the
    //       visualisations, so is the only default that is specified here and
    //       not with the native native.
    //**************************************************************************
    static cfg_pages cfg_TrackDetails{
        //--------------------------------------------------
        // native_enum_type::Page1
        cfg_track_details{
            id_TrackDetails[native_enum_type::Page1],
            native_config_type{ native_enum_type::Page1 },
            "[%track artist% - ]%title%$char(10)\r\n%album artist%$char(10)\r\n%album%$char(10)\r\n%date%"
        },

        //--------------------------------------------------
        // native_enum_type::Page2
        cfg_track_details{
            id_TrackDetails[native_enum_type::Page2],
            native_config_type{ native_enum_type::Page2 },
            "[%track artist% - ]%title%$char(10)\r\n%album artist%$char(10)\r\n%album% ['('%date%')']"
        },

        //--------------------------------------------------
        // native_enum_type::Page3
        cfg_track_details{
            id_TrackDetails[native_enum_type::Page3],
            native_config_type{ native_enum_type::Page3 },
            "%album artist%$char(10)\r\n[%track artist% - ]%title%$char(10)\r\n%album%$char(10)\r\n%date%"
        },

        //--------------------------------------------------
        // native_enum_type::Page4
        cfg_track_details{
            id_TrackDetails[native_enum_type::Page4],
            native_config_type{ native_enum_type::Page4 },
            "%album artist%$char(10)\r\n[%track artist% - ]%title%$char(10)\r\n%album% ['('%date%')']"
        },
    }; // cfg_pages cfg_TrackDetails

    //------------------------------------------------------

    cfg_uint cfg_PageConfigVersion{ id_PageConfigVersion, vPageConfigVersion };
    cfg_uint cfg_PageConfigSize   { id_PageConfigSize,    nPageConfigSize    };
} // namespace <anonymous>

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // TrackDetailsConfig
    //**************************************************************************
    TrackDetailsConfig::native_config_type TrackDetailsConfig::load(native_enum_type type) {
        native_config_type native{};
        if (cfg_PageConfigVersion == vPageConfigVersion &&
            cfg_PageConfigSize    == nPageConfigSize) {
            foobar::Config::cfg_load(native, cfg_TrackDetails[type]);
        } else {
            // Convert config to newer version when possible
            // ...
            // Otherwise:
            SPDLOG_ERROR("Error loading Track Details native, version/size mismatch: expected v{}/{}b, got v{}/{}b. Config has been reset.",
                         vPageConfigVersion,
                         nPageConfigSize,
                         cfg_PageConfigVersion.get_value(),
                         cfg_PageConfigSize.get_value());
            set_default(type, native);
            foobar::Config::cfg_save(cfg_TrackDetails[type], native);
        }
        return native;
    }

    //--------------------------------------------------------------------------

    TrackDetailsConfig::native_array_type TrackDetailsConfig::load() {
        TrackDetailsConfig::native_array_type native{};
        for (const auto i : native_enum_type{}) { native[i] = load(i); }
        return native;
    }

    //--------------------------------------------------------------------------

    void TrackDetailsConfig::save(typename native_enum_type type,
                                  const native_config_type& native) {
        foobar::Config::cfg_save(cfg_PageConfigVersion,  vPageConfigVersion);
        foobar::Config::cfg_save(cfg_PageConfigSize,     nPageConfigSize);
        foobar::Config::cfg_save(cfg_TrackDetails[type], native);
    }

    //--------------------------------------------------------------------------

    void TrackDetailsConfig::save(const native_array_type& native) {
        for (const auto i : native_enum_type{}) { save(i, native[i]); }
    }

    //--------------------------------------------------------------------------

    void TrackDetailsConfig::set_default(typename native_enum_type type,
                                         native_config_type& native) {
        native = native_config_type{};
        foobar::Config::cfg_default(native, cfg_TrackDetails[type]);
    }

    //--------------------------------------------------------------------------

    void TrackDetailsConfig::set_default(native_array_type& config) {
        for (const auto i : native_enum_type{}) { set_default(i, config[i]); }
    }
} // namespace foobar::Config
