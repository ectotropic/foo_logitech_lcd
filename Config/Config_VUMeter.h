#pragma once
#ifndef GUID_8C58BC34_DB88_4EDB_A6BE_3602571BA956
#define GUID_8C58BC34_DB88_4EDB_A6BE_3602571BA956
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
// VUMeterType
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(VUMeterType,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(Meter1, 0),
                                             Meter2,
                                             Image),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"VU Meter 1",
                                                    L"VU Meter 2",
                                                    L"VU Meter Image"));

//==============================================================================

namespace Config {
    //**************************************************************************
    // VUMeterConfig
    //**************************************************************************
    struct VUMeterConfig final {
    public:
        using config_type  = VUMeterConfig;
        using enum_type    = VUMeterType;
        using array_type   = std::array<config_type, enum_type::count()>;

        using version_type = std::uint32_t;

        //---------------------------------------
        struct RangeConfig final {
            float m_fMin{ -40.f };
            float m_fMax{   3.f };
        }; // struct RangeConfig final
        //---------------------------------------

        //---------------------------------------
        enum RangeChannel {
            RangeChannel0,
            RangeChannel1,
            RangeChannelCount
        }; // enum RangeChannel
        //---------------------------------------

    public:
        inline static constexpr const version_type Version{ 1 };

    public:
        constexpr VUMeterConfig() noexcept = default;
        constexpr VUMeterConfig(enum_type /*type*/) noexcept {}

    public:
        bool        m_bEnabled{ true };
        ColorConfig m_Color   { };
        PeakConfig  m_Peak    { };

        RangeConfig m_Range[RangeChannelCount]{ };

    public:
        static const config_type& get(enum_type type);
        static const array_type&  get();
    }; // struct VUMeterConfig final
} // namespace Config

#endif // GUID_8C58BC34_DB88_4EDB_A6BE_3602571BA956
