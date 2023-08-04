#pragma once
#ifndef GUID_C51D0453_3689_4824_A5D1_098439332ABA
#define GUID_C51D0453_3689_4824_A5D1_098439332ABA
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
#include "Image_ImageData.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // ImageLoader
    //**************************************************************************
    struct ImageLoader final {
        using size_type = typename ImageData::size_type;

        //==================================================
        // Load
        static ImageData::Compact Load(const void* pData,
                                                      size_type byteSize);
        static ImageData::Compact Load(const char* szFile);
        static ImageData::Compact Load(const wchar_t* szFile);
        //==================================================
    }; // struct ImageLoader final
} // namespace Image

#endif // GUID_C51D0453_3689_4824_A5D1_098439332ABA