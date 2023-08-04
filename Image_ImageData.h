#pragma once
#ifndef GUID_203B757D_9828_464A_8927_C48B322CF418
#define GUID_203B757D_9828_464A_8927_C48B322CF418
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
#include <cassert>
#include <cstdint>
#include <vector>
#include <utility>
//--------------------------------------

namespace Image {
    //**************************************************************************
    // ImageData
    //**************************************************************************
    struct ImageData final {
    public:
        template <typename PixelT>
        using storage_type = std::vector<PixelT>;

        using extent_type = ::std::int32_t;
        using byte_type   = ::std::uint8_t;
        using size_type   = typename storage_type<byte_type>::size_type;

    public:
        //======================================================================
        // Sparse:
        //  <**NOT** Memory Owning>
        //
        //  Image data where the data block may contain "gaps" between the data.
        //  This data is often received from APIs (where the data is aligned
        //  for best performance and so may have padding).
        //
        //  Data type is unspecified.
        //
        //  Data is not owned by this structure.
        //======================================================================
        struct Sparse final {
        public:
            using extent_type = typename ::Image::ImageData::extent_type;
            using size_type   = typename ::Image::ImageData::size_type;

        private:
            static constexpr auto BytesFromBits(size_type bits) noexcept {
                auto bytes{ bits / 8 };
                if (bits % 8) { ++bytes; }
                if (bytes != 0) {
                    bytes--;
                    bytes |= bytes >> 1;
                    bytes |= bytes >> 2;
                    bytes |= bytes >> 4;
                    bytes |= bytes >> 8;
                    bytes |= bytes >> 16;
                    bytes++;
                }
                return bytes;
            }

        public:
            Sparse() = default;

            Sparse(extent_type width,
                   extent_type height,
                   size_type bitsPerPixel,
                   size_type pixelStride,
                   size_type scanStride,
                   void* pixels) noexcept :
                iWidth        { width },
                iHeight       { height },
                cBitsPerPixel { bitsPerPixel },
                cBytesPerPixel{ BytesFromBits(bitsPerPixel) },
                cPixelStride  { pixelStride },
                cScanStride   { scanStride },
                pPixels       { pixels } {}

            Sparse(extent_type width,
                   extent_type height,
                   size_type bitsPerPixel,
                   size_type scanStride,
                   void* pixels) noexcept :
                Sparse{
                    width,
                    height,
                    bitsPerPixel,
                    bitsPerPixel / 8, // default pixelStride == 1 pixel
                    scanStride,
                    pixels
                } {}

            Sparse(extent_type width,
                   extent_type height,
                   size_type bitsPerPixel,
                   void* pixels) noexcept :
                Sparse{
                    width,
                    height,
                    bitsPerPixel,
                    width * bitsPerPixel / 8, // default scanStride == width pixels
                    pixels
                } {}

            Sparse(const Sparse&) = default;

            Sparse(Sparse&& other) noexcept :
                iWidth       { std::exchange(other.iWidth       , 0) },
                iHeight      { std::exchange(other.iHeight      , 0) },
                cBitsPerPixel{ std::exchange(other.cBitsPerPixel, 0) },
                cPixelStride { std::exchange(other.cPixelStride , 0) },
                cScanStride  { std::exchange(other.cScanStride  , 0) },
                pPixels      { std::exchange(other.pPixels      , nullptr) } {}

            Sparse& operator=(const Sparse&) noexcept = default;

            Sparse& operator=(Sparse&& other) noexcept {
                iWidth        = std::exchange(other.iWidth       , 0);
                iHeight       = std::exchange(other.iHeight      , 0);
                cBitsPerPixel = std::exchange(other.cBitsPerPixel, 0);
                cPixelStride  = std::exchange(other.cPixelStride , 0);
                cScanStride   = std::exchange(other.cScanStride  , 0);
                pPixels       = std::exchange(other.pPixels      , nullptr);
                return *this;
            }

        public:
            constexpr explicit operator bool() const noexcept {
                assert(pPixels == nullptr ||
                       ((iWidth != 0) && (iHeight != 0) &&
                        (cBitsPerPixel != 0) &&
                        (cPixelStride != 0) &&
                        (cScanStride != 0)));
                return pPixels != nullptr;
            }

        public:
            extent_type iWidth        { 0 };
            extent_type iHeight       { 0 };
            size_type   cBitsPerPixel { 0 };
            size_type   cBytesPerPixel{ 0 };
            size_type   cPixelStride  { 0 };
            size_type   cScanStride   { 0 };
            void*       pPixels       { nullptr };
        }; // struct Sparse final

        //======================================================================
        // Compact:
        //  <Memory Owning>
        //
        //  Image data where the data has no gaps (except where required due
        //  to type sizes; e.g. 24bit data will be stored as 32bit).
        //
        //  This is how data is normally required to be passed to APIs.
        //
        //  Data type does **not** have to match PixelStorageTypeT, but the
        //  size of the data type **must** be less or equal to the size of
        //  PixelStorageTypeT.
        //======================================================================
    public:
        class Compact final {
        public:
            using sparse_type = ::Image::ImageData::Sparse;
            using buffer_type = ::Image::ImageData::storage_type<byte_type>;
            using extent_type = typename ::Image::ImageData::extent_type;

        public:
            Compact() = default;

            Compact(extent_type width,
                    extent_type height,
                    const buffer_type& pixels) :
                m_iWidth { width },
                m_iHeight{ height },
                m_Pixels { pixels } {}

            Compact(extent_type width,
                    extent_type height,
                    buffer_type&& pixels) noexcept :
                m_iWidth { width },
                m_iHeight{ height },
                m_Pixels { std::forward<buffer_type>(pixels) } {}

            Compact(const sparse_type& sparse);

            Compact(const Compact&) = default;

            Compact(Compact&& other) noexcept :
                m_iWidth { std::exchange(other.m_iWidth , 0) },
                m_iHeight{ std::exchange(other.m_iHeight, 0) },
                m_Pixels { std::move(other.m_Pixels) } {}

            Compact& operator=(const Compact&) = default;

            Compact& operator=(Compact&& other) noexcept {
                m_iWidth  = std::exchange(other.m_iWidth , 0);
                m_iHeight = std::exchange(other.m_iHeight, 0);
                m_Pixels  = std::move(other.m_Pixels);
                return *this;
            }

            auto GetWidth () const noexcept { return m_iWidth; }
            auto GetHeight() const noexcept { return m_iHeight; }

            auto GetPixelCount() const noexcept { return m_iWidth * m_iHeight; }
            auto GetByteSize  () const noexcept { return m_Pixels.size(); }

            decltype(auto) GetPixels() const noexcept { return m_Pixels.data(); }
            decltype(auto) GetPixels()       noexcept { return m_Pixels.data(); }

            explicit operator bool() const noexcept {
                assert(m_Pixels.empty() || ((m_iWidth > 0) && (m_iHeight > 0)));
                return !m_Pixels.empty();
            }

        private:
            extent_type m_iWidth { 0 };
            extent_type m_iHeight{ 0 };
            buffer_type m_Pixels {};
        }; // class Compact final
    }; // struct ImageData final
} // namespace Image

#endif //GUID_203B757D_9828_464A_8927_C48B322CF418