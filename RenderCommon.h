#pragma once
#ifndef GUID_15688453_593B_4C1C_B03C_FF6454AD961A
#define GUID_15688453_593B_4C1C_B03C_FF6454AD961A
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
 *******************************************************************************/

//--------------------------------------
//
#include "Util/SequentialEnum.h"
//--------------------------------------

//--------------------------------------
//
#include "Color.h"
//--------------------------------------

//******************************************************************************
// RenderPass
//******************************************************************************
SEQUENTIAL_NAMED_ENUM(RenderPass,
                      SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(OpenGL, 0),
                                             GDI),
                      SEQUENTIAL_NAMED_ENUM_STRINGS(L"OpenGL",
                                                    L"GDI"));

#endif //GUID_15688453_593B_4C1C_B03C_FF6454AD961A