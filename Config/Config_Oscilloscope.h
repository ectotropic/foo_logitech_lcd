#pragma once
#ifndef GUID_F89463DE_564B_4D35_A86D_ACA06AD1F03F
#define GUID_F89463DE_564B_4D35_A86D_ACA06AD1F03F
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
#include <array>
//--------------------------------------

//******************************************************************************
// OscilloscopeType
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(OscilloscopeType,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(DotMono, 0),
                                             DotStereo,
                                             LineMono,
                                             LineStereo,
                                             CircleMono,
                                             StarBurstMono),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Dot Oscilloscope (Mono)",
                                                    L"Dot Oscilloscope (Stereo)",
                                                    L"Line Oscilloscope (Mono)",
                                                    L"Line Oscilloscope (Stereo)",
                                                    L"Circle Oscilloscope (Mono)",
                                                    L"StarBurst (Mono)"));

//==============================================================================

namespace Config {
    //**************************************************************************
    // OscilloscopeConfig
    //**************************************************************************
    struct OscilloscopeConfig final {
    public:
        using config_type  = OscilloscopeConfig;
        using enum_type    = OscilloscopeType;
        using array_type   = std::array<config_type, enum_type::count()>;

        using version_type = std::uint32_t;

    public:
        inline static constexpr const version_type Version{ 1 };

    public:
        constexpr OscilloscopeConfig() noexcept = default;
        constexpr OscilloscopeConfig(enum_type /*type*/) noexcept {}

    public:
        bool        m_bEnabled  { true };
        float       m_fScale    { 4.f };
        float       m_fLineWidth{ 2.f };
        float       m_fPointSize{ 2.f };
        ColorConfig m_Color     { };

    public:
        static const config_type& get(enum_type type);
        static const array_type&  get();
    }; // struct OscilloscopeConfig final
} // namespace Config

#endif // GUID_F89463DE_564B_4D35_A86D_ACA06AD1F03F
