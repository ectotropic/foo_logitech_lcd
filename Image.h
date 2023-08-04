#pragma once
#ifndef GUID_81D35099_FC41_4D4A_9F16_194E4ECBB75A
#define GUID_81D35099_FC41_4D4A_9F16_194E4ECBB75A
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

//--------------------------------------
//
#include <cstdint>

#include <utility>
#include <memory>
#include <limits>
//--------------------------------------

namespace Image {
    //**************************************************************************
    // Image
    //**************************************************************************
    class Image {
    public:
        template <class TypeT>
        using managed_pointer_type = std::unique_ptr<TypeT>;

        using data_type   = ::Image::ImageData::Compact;
        using byte_type   = typename ::Image::ImageData::byte_type;
        using size_type   = typename ::Image::ImageData::size_type;
        using extent_type = typename ::Image::ImageData::extent_type;
        using coord_type  = typename extent_type;
        using pointer     = managed_pointer_type<::Image::Image>;

    private:
        Image(const Image&)            = delete; // No Copy
        Image& operator=(const Image&) = delete; // No Copy

    protected:
        Image() = default;

        Image(extent_type width, extent_type height) noexcept :
            m_iWidth { width },
            m_iHeight{ height } {}

        Image(Image&& other) noexcept :
            m_iWidth { std::exchange(other.m_iWidth,  0) },
            m_iHeight{ std::exchange(other.m_iHeight, 0) } {}

        Image& operator=(Image&& other) noexcept {
            Destroy();
            using std::swap;
            swap(m_iWidth,  other.m_iWidth);
            swap(m_iHeight, other.m_iHeight);
            return *this;
        }

    public: // Interface
        virtual ~Image() noexcept { Destroy(); }

        virtual void Destroy() noexcept = 0 {
            m_iWidth = m_iHeight = 0;
        };

        virtual void Draw(coord_type x0, coord_type y0,
                          coord_type x1, coord_type y1) const noexcept = 0;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           float alpha = 1.f) const noexcept = 0;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           byte_type alpha) const noexcept = 0;

        virtual bool Valid() const noexcept = 0 {
            return m_iWidth > 0 && m_iHeight > 0;
        }

    public: // Helpers
        void DrawBlendAuto(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           byte_type alpha) const noexcept {
            if (alpha == std::numeric_limits<byte_type>::max()) {
                Draw(x0, y0, x1, y1);
            } else if (alpha > std::numeric_limits<byte_type>::min()) {
                Blend(x0, y0, x1, y1, alpha);
            }
        }

        void DrawBlendAuto(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           float alpha) const noexcept {
            if (alpha >= 1.f) {
                Draw(x0, y0, x1, y1);
            } else if (alpha > 0.f) {
                Blend(x0, y0, x1, y1, alpha);
            }
        }

    public: // Accessors
        constexpr auto GetWidth () const noexcept { return m_iWidth; }
        constexpr auto GetHeight() const noexcept { return m_iHeight; }

        explicit operator bool() const noexcept { return Valid(); }

    protected:
        extent_type m_iWidth { 0 };
        extent_type m_iHeight{ 0 };
    }; // class Image

    //**************************************************************************
    // ImagePtr
    //**************************************************************************
    using ImagePtr = typename ::Image::Image::pointer;
} // namespace Image

#endif //GUID_81D35099_FC41_4D4A_9F16_194E4ECBB75A