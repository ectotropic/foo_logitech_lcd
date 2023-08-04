#pragma once
#ifndef GUID_9780CA03_CA34_4B57_8CB7_D5ED3BC20DDD
#define GUID_9780CA03_CA34_4B57_8CB7_D5ED3BC20DDD
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
#include "Util/SequentialEnum.h"
//--------------------------------------

//--------------------------------------
//
#include <cstdint>
//--------------------------------------

//******************************************************************************
// WallpaperMode
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(WallpaperMode,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(None, 0),
                                             AlbumArt,
                                             File),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"None",
                                                    L"AlbumArt",
                                                    L"File"));

//******************************************************************************
// AlbumArtType
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(AlbumArtType,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(FrontCover, 0),
                                             BackCover,
                                             Disc,
                                             Artist,
                                             Icon,
                                             Any),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Front Cover",
                                                    L"Back Cover",
                                                    L"Disc",
                                                    L"Artist",
                                                    L"Icon",
                                                    L"Any"));


//******************************************************************************
// VisualisationMode
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(VisualisationMode,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Mode1, 0),
                                             Mode2,
                                             Mode3,
                                             Mode4,
                                             ModeError        = 0xffffffff,
                                             SpectrumAnalyser = Mode1,
                                             Oscilloscope     = Mode2,
                                             VUMeter          = Mode3,
                                             TrackDetails     = Mode4),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Spectrum Analyser",
                                                    L"Oscilloscope",
                                                    L"VU Meter",
                                                    L"Track Information"));

//******************************************************************************
// VisualisationDisplayMode
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(VisualisationDisplayMode,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Normal, 0),
                                             PermanentProgressBar,
                                             PermanentTrackDetails,
                                             SplitScreen),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Normal",
                                                    L"Permanent Progress Bar",
                                                    L"Permanent Track Details",
                                                    L"Split Screen"));

//==============================================================================

namespace Config {
    //**************************************************************************
    // GeneralConfig
    //**************************************************************************
    struct GeneralConfig final {
    public:
        using config_type  = GeneralConfig;

        using version_type = std::uint32_t;
        using string_type  = std::wstring;
        using index_type   = std::uint32_t;

        //---------------------------------------
        struct DisplayConfig final {
            bool m_bVSync             { false };
            bool m_bForceToForeground { false };
            bool m_bBackgroundMode    { false };
            bool m_bBackgroundOnPause { true  };
            bool m_bPopUpOnTrackChange{ false };
        }; // struct DisplayConfig final
        //---------------------------------------

        //---------------------------------------
        struct CanvasConfig final {
            //--------------------------
            struct WallpaperConfig final {
                WallpaperMode m_Mode         { WallpaperMode::None };
                AlbumArtType  m_AlbumArtType { AlbumArtType::Any };
                string_type   m_File         { };
                bool          m_bStretchToFit{ false };
            }; // struct WallpaperConfig final
            //--------------------------

            bool            m_bPreferHardwareCanvas{ true  };
            bool            m_bUseTrailEffect      { false };
            WallpaperConfig m_Wallpaper            { };
        }; // struct CanvasConfig final
        //---------------------------------------

        //---------------------------------------
        struct VisualisationConfig final {
            //--------------------------
            struct StartConfig final {
                bool              m_bRememberLast{ false };
                VisualisationMode m_LastMode     { VisualisationMode::TrackDetails };
                index_type        m_LastIndex    { 0 };
            }; // struct StartConfig final
            //--------------------------

            //--------------------------
            struct AutoChangeConfig final {
                bool    m_bEnable       { false };
                float   m_fChangeSeconds{ 30.f  };
            }; // struct AutoChangeConfig final
            //--------------------------

            //--------------------------
            struct OnTrackChangeConfig final {
                bool  m_bShowTrackDetails{ true };
                float m_fShowSeconds     { 5.f  };
            }; // struct OnTrackChangeConfig final
            //--------------------------

            VisualisationDisplayMode m_DisplayMode  { VisualisationDisplayMode::Normal };
            StartConfig              m_Start        { };
            AutoChangeConfig         m_AutoChange   { };
            OnTrackChangeConfig      m_OnTrackChange{ };
        }; // struct VisualisationConfig final
        //---------------------------------------

    public:
        inline static constexpr const version_type Version{ 1 };

    public:
        constexpr GeneralConfig() noexcept = default;

    public:
        bool                m_bExpertMode  { false };
        DisplayConfig       m_Display      { };
        CanvasConfig        m_Canvas       { };
        VisualisationConfig m_Visualisation{ };

    public:
        static const config_type& get();
    }; // struct GeneralConfig final
} // namespace Config

#endif // GUID_9780CA03_CA34_4B57_8CB7_D5ED3BC20DDD
