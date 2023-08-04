#pragma once
#ifndef GUID_9262DD09_B3E0_408E_A2D9_81B203440D62
#define GUID_9262DD09_B3E0_408E_A2D9_81B203440D62
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
//  Include GDI Core
#include "Windows/GDI/GDI.h"
//--------------------------------------

//#define foo_logitech_lcd_ENABLE_DEBUG_CANVAS

#if defined(DEBUG) || defined(_DEBUG) || defined(foo_logitech_lcd_ENABLE_DEBUG_CANVAS)
    void InitialiseDebug        ()                          noexcept;
    void InitialiseDebugCanvas  (_In_ SIZE size)            noexcept;
    void UninitialiseDebugCanvas()                          noexcept;
    void UpdateDebugCanvas      (_In_ SIZE dim,
                                 _In_ BYTE cColorBits,
                                 _In_ LPVOID pBits)         noexcept;
#   else
    inline constexpr
    void InitialiseDebug        ()                          noexcept {};
    inline constexpr
    void InitialiseDebugCanvas  (_In_ SIZE /*size*/)        noexcept {};
    inline constexpr
    void UninitialiseDebugCanvas()                          noexcept {};
    inline constexpr
    void UpdateDebugCanvas      (_In_ SIZE /*dim*/,
                                 _In_ BYTE /*cColorBits*/,
                                 _In_ LPVOID /*pBits*/)     noexcept {};
#endif // #if defined(DEBUG) || defined(_DEBUG) || defined(foo_logitech_lcd_ENABLE_DEBUG_CANVAS)

#endif // GUID_9262DD09_B3E0_408E_A2D9_81B203440D62