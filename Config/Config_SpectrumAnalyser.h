#pragma once
#ifndef GUID_19C0A925_5107_445E_8E99_99BA9A4CDA06
#define GUID_19C0A925_5107_445E_8E99_99BA9A4CDA06
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
// SpectrumAnalyserMode
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(SpectrumAnalyserMode,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Linear, 0),
                                             NonLinear1,
                                             NonLinear2,
                                             NonLinear3,
                                             NonLinear4),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Linear",
                                                    L"Non-Linear 1",
                                                    L"Non-Linear 2",
                                                    L"Non-Linear 3",
                                                    L"Non-Linear 4"));

//******************************************************************************
// SpectrumAnalyserType
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(SpectrumAnalyserType,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Mono, 0),
                                             MonoBlock,
                                             Stereo,
                                             StereoBlock),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"Spectrum Analyser (Mono)",
                                                    L"Block Spectrum Analyser (Mono)",
                                                    L"Spectrum Analyser (Stereo)",
                                                    L"Block Spectrum Analyser (Stereo)"));

//==============================================================================

namespace Config {
    //**************************************************************************
    // SpectrumAnalyserConfig
    //**************************************************************************
    struct SpectrumAnalyserConfig final {
    public:
        using config_type  = SpectrumAnalyserConfig;
        using enum_type    = SpectrumAnalyserType;
        using array_type   = std::array<config_type, enum_type::count()>;

        using mode_type    = SpectrumAnalyserMode;
        using version_type = std::uint32_t;
        using size_type    = std::uint32_t;

        //---------------------------------------
        struct BlockConfig final {
            bool      m_bGap  { false };
            size_type m_uCount{ 8 };
        }; // struct BlockConfig final
        //---------------------------------------

    public:
        inline static constexpr const version_type Version{ 1 };

    public:
        SpectrumAnalyserConfig() noexcept = default;
        SpectrumAnalyserConfig(enum_type /*type*/) noexcept {}

    public:
        bool        m_bEnabled    { true };
        mode_type   m_SpectrumMode{ mode_type::NonLinear4 };
        float       m_fPreScale   { 1.f  };
        float       m_fPostScale  { .1f  };
        float       m_fOffset     { 10.f };
        PeakConfig  m_Peak        { };
        ColorConfig m_Color       { };
        BlockConfig m_Block       { };

    public:
        static const config_type& get(enum_type type);
        static const array_type&  get();
    }; // struct SpectrumConfig final
} // namespace Config

#endif // GUID_19C0A925_5107_445E_8E99_99BA9A4CDA06
