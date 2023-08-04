#pragma once
#ifndef GUID_058E5EF2_734E_4DEB_916B_4666E32A58BA
#define GUID_058E5EF2_734E_4DEB_916B_4666E32A58BA
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
#include "Config/Config_Common.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI_Font.h"
#include "Windows/GDI/GDI.h"
#include "GDI_Scroller.h" // Windows::GDI::ScrollMode
#include "GDI_TextLine.h" // Windows::GDI::VerticalAlignWindows::GDI::Text::Align::Horizontal
//--------------------------------------

//******************************************************************************
// TrackDetailsType
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(TrackDetailsType,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Page1, 0),
                                             Page2,
                                             Page3,
                                             Page4),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Track Details Page 1",
                                                    L"Track Details Page 2",
                                                    L"Track Details Page 3",
                                                    L"Track Details Page 4"));

//******************************************************************************
// TextFlags
//******************************************************************************
FLAG_ENUM(TextFlags,
          FLAG_ENUM_VALUES(None                = 0,
                           AllowOverlap        = 1 << 0,
                           ClearBackground     = 1 << 1,
                           RestrictedLineCount = 1 << 2));

//******************************************************************************
// TrackTime
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(TrackTime,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Off, 0),
                                             TotalTime,
                                             ElapsedTime,
                                             RemainingTime,
                                             SystemTime,
                                             SystemDate,
                                             ElapsedPercent,
                                             RemainingPercent),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"None",
                                                    L"Total Time",
                                                    L"Elapsed Time",
                                                    L"Remaining Time",
                                                    L"System Time",
                                                    L"System Date",
                                                    L"Elapsed Percent",
                                                    L"Remaining Percent"));

//==============================================================================

namespace Config {
    //**************************************************************************
    // TrackDetailsConfig
    //
    // NOTE: Text Config text format is specific to the program driving the
    //       visualisations, so is the only default that is NOT specified here,
    //       but is specified with the program config interface.
    //**************************************************************************
    struct TrackDetailsConfig final {
    public:
        using config_type  = TrackDetailsConfig;
        using enum_type    = TrackDetailsType;
        using array_type   = std::array<config_type, enum_type::count()>;

        using version_type = std::uint32_t;
        using string_type  = std::wstring;

        //---------------------------------------
        struct TextConfig final {
        public:
            using font_type             = ::LOGFONT;
            using color_type            = ::COLORREF;
            using vertical_align_type   = Windows::GDI::Text::Align::Vertical;
            using horizontal_align_type = Windows::GDI::Text::Align::Horizontal;

            //-------------------------
            template <typename AlignT>
            struct AlignScrollConfigT final {
            public:
                using align_type = AlignT;

                //---------------------
                struct ScrollConfig final {
                    using mode_type = Windows::GDI::ScrollMode;

                    mode_type m_Mode  { mode_type::None };
                    float     m_fSpeed{ 1.f };
                    float     m_fDelay{ 5.f };
                    float     m_fGap  { 2.f };
                }; // struct ScrollConfig final
                //---------------------

            public:
                align_type   m_Align { align_type::Centre };
                ScrollConfig m_Scroll{ };
            }; // struct AlignScrollConfigT final
            //-------------------------

            using vertical_config_type   = AlignScrollConfigT<vertical_align_type>;
            using horizontal_config_type = AlignScrollConfigT<horizontal_align_type>;

        public:
            TextConfig() noexcept;

        public:
            // m_Format contents is application specific;
            // default is defined with application config
            string_type            m_Format              { };

            font_type              m_Font                { 0 };
            bool                   m_bAllowOverlap       { false };

            color_type             m_Color               { 0x00FFFFFF };
            color_type             m_BackgroundColor     { 0x00000000 };
            bool                   m_bClearBackground    { false };

            vertical_config_type   m_Vertical            {};
            horizontal_config_type m_Horizontal          {};

            bool                   m_bRestrictedLineCount{ true };
        }; // struct TextConfig final
        //---------------------------------------

        //---------------------------------------
        struct ProgressBarConfig final {
        public:
            using color_type = ::COLORREF;

            //-------------------------
            struct TimeConfig final {
                TrackTime m_Left { TrackTime::ElapsedTime };
                TrackTime m_Right{ TrackTime::TotalTime   };
            }; // struct TimeConfig final
            //-------------------------

        public:
            bool        m_bEnabled       { true };
            float       m_fHeight        { 10.f };

            TimeConfig  m_Time           {};

            color_type  m_Color          { 0x00FFFFFF };
            bool        m_bFilled        { true };
            color_type  m_BackgroundColor{ 0x00000000 };
        }; // struct ProgressBarConfig final
        //---------------------------------------

    public:
        inline static constexpr const version_type Version{ 1 };

    public:
        TrackDetailsConfig() noexcept = default;
        TrackDetailsConfig(enum_type type) noexcept
            : m_bEnabled{ type == enum_type::Page1 } {}

        bool              m_bEnabled              { false };
        TextConfig        m_Text                  { };
        ProgressBarConfig m_ProgressBar           { };
        bool              m_bSyncProgressBarConfig{ false }; // Use m_Text values to set m_ProgressBar values

    public:
        static const config_type& get(enum_type type);
        static const array_type&  get();
    }; // struct TrackDetailsConfig final
} // namespace Config

#endif // GUID_058E5EF2_734E_4DEB_916B_4666E32A58BA
