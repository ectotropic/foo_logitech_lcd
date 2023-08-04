#pragma once
#ifndef GUID_4B20F4A9_4993_4465_85B3_839AC6B33AFA
#define GUID_4B20F4A9_4993_4465_85B3_839AC6B33AFA
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
#include "Util/SequentialEnum.h"
#include "Util/FlagEnum.h"
#include "RenderCommon.h"
#include "Color.h"
//--------------------------------------

//--------------------------------------
//
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
//--------------------------------------

namespace Config {
    //**************************************************************************
    // PeakConfig
    //**************************************************************************
    struct PeakConfig final {
        bool  m_bEnable   { false };
        float m_fDecayRate{ 0.01f };
    }; // struct PeakConfig final

    //**************************************************************************
    // ColorConfig
    //**************************************************************************
    struct ColorConfig final {
        Color::ColorPalette m_Palette         { 0x00FFFFFF, 0x00FFFFFF, 0x00000000 };
        bool                m_bAltGradientMode{ false };
    }; // struct ColorConfig final
} // namespace Config

#endif // GUID_4B20F4A9_4993_4465_85B3_839AC6B33AFA
