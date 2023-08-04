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
#include "CommonHeaders.h"
#include "Image_ImageData.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // ImageData::Compact
    //**************************************************************************
    ImageData::Compact::Compact(const sparse_type& sparse) :
        m_iWidth { sparse.iWidth },
        m_iHeight{ sparse.iHeight } {

        const auto pixelStorageBits { sparse.cBytesPerPixel * 8 };
        const auto pixelStrideBits  { sparse.cPixelStride   * 8 };
        const auto scanByteSize     { sparse.iWidth * sparse.cBytesPerPixel };
        const auto dataByteSize     { scanByteSize * sparse.iHeight };

        const auto compactPixels{
            (pixelStrideBits  == pixelStorageBits) &&
            (pixelStorageBits == sparse.cBitsPerPixel)
        };
        const auto compactRows{ scanByteSize == sparse.cScanStride };

        m_Pixels.resize(dataByteSize);

        if (compactRows && compactPixels) {
            std::memcpy(m_Pixels.data(), sparse.pPixels, dataByteSize);
        } else if (compactPixels) {
                  auto* byteDst{ m_Pixels.data() };
            const auto* byteSrc{ static_cast<const byte_type*>(sparse.pPixels) };
            for (auto line = 0; line < sparse.iHeight; ++line) {
                std::memcpy(byteDst, byteSrc, scanByteSize);
                byteSrc += sparse.cScanStride;
                byteDst += scanByteSize;
            }
        } else {
                  auto* byteDst{ m_Pixels.data() };
            const auto* byteSrc{ static_cast<const byte_type*>(sparse.pPixels) };
            if (pixelStorageBits > (sparse.cBitsPerPixel / 8)) {
                std::memset(byteDst, 0, dataByteSize);
            }
            for (auto line = 0; line < sparse.iHeight; ++line) {
                      auto* dstPixel = byteDst;
                const auto* srcPixel = byteSrc;
                for (auto pixel = 0; pixel < sparse.iWidth; ++pixel) {
                    std::memcpy(dstPixel, srcPixel, sparse.cBytesPerPixel);
                    srcPixel += sparse.cPixelStride;
                    dstPixel += sparse.cBytesPerPixel;
                }
                byteSrc += sparse.cScanStride;
                byteDst += scanByteSize;
            }
        }
    }
} // namespace Image
