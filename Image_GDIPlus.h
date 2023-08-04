#pragma once
#ifndef GUID_FE8C76F6_04AD_4523_AA6E_DAC390E25B86
#define GUID_FE8C76F6_04AD_4523_AA6E_DAC390E25B86
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
#include "Image.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI.h"
#include "Windows/GDIPlus/GDIPlus.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // GDIPlusImage
    //**************************************************************************
    class GDIPlusImage final : public ::Image::Image {
    private:
        using base_type = ::Image::Image;
        using this_type = ::Image::GDIPlusImage;

    public:
        using graphics_pointer  = managed_pointer_type<::Gdiplus::Graphics>;
        using bitmap_pointer    = managed_pointer_type<::Gdiplus::Bitmap>;
        using pointer           = managed_pointer_type<this_type>;

        using native_extent_type = INT;
        using native_coord_type  = INT;

    public:
        static pointer NewImage(_In_ HDC hDC,
                                _In_ native_coord_type width,
                                _In_ native_coord_type height,
                                _In_ const void* data) noexcept;

        static pointer NewImage(_In_ HDC hDC,
                                _In_ const data_type& data) noexcept;

        static decltype(auto) NewImage(_In_ native_coord_type width,
                                       _In_ native_coord_type height,
                                       _In_ const void* data) noexcept {
            return NewImage(::Windows::GDI::GetCurrentDC(), width, height, data);
        }

        static decltype(auto) NewImage(_In_ const data_type& data) noexcept {
            return NewImage(::Windows::GDI::GetCurrentDC(), data);
        }

    private:
        GDIPlusImage(native_extent_type width,
                     native_extent_type height,
                     graphics_pointer&& pGraphics) noexcept;

        GDIPlusImage(const GDIPlusImage&)            = delete; // No Copy
        GDIPlusImage& operator=(const GDIPlusImage&) = delete; // No Copy

    public:
        GDIPlusImage() = default;

        GDIPlusImage(GDIPlusImage&& other) noexcept :
            base_type  { std::forward<GDIPlusImage>(other) },
            m_pBitmap  { std::forward<bitmap_pointer>(other.m_pBitmap) },
            m_pGraphics{ std::forward<graphics_pointer>(other.m_pGraphics) }
        {}

        GDIPlusImage& operator=(GDIPlusImage&& other) noexcept {
            Destroy();
            base_type::operator=(std::forward<base_type>(other));
            m_pBitmap   = std::forward<bitmap_pointer>(other.m_pBitmap);
            m_pGraphics = std::forward<graphics_pointer>(other.m_pGraphics);
            return *this;
        }

    public:
        virtual void Destroy() noexcept override;

        virtual void Draw(_In_ coord_type x0, _In_ coord_type y0,
                          _In_ coord_type x1, _In_ coord_type y1) const noexcept override;

        virtual void Blend(_In_ coord_type x0, _In_ coord_type y0,
                           _In_ coord_type x1, _In_ coord_type y1,
                           _In_ float alpha = 1.f) const noexcept override;

        virtual void Blend(_In_ coord_type x0, _In_ coord_type y0,
                           _In_ coord_type x1, _In_ coord_type y1,
                           _In_ byte_type alpha) const noexcept override {
            Blend(x0, y0, x1, y1, static_cast<float>(alpha) / 255.f);
        }

        virtual bool Valid() const noexcept override {
            return base_type::Valid() && m_pBitmap && m_pGraphics;
        }

    private:
        graphics_pointer m_pGraphics{};
        bitmap_pointer   m_pBitmap  {};
    }; // class GDIPlusImage final

    //**************************************************************************
    // GDIPlusImagePtr
    //**************************************************************************
    using GDIPlusImagePtr = ::Image::GDIPlusImage::pointer;
} // namespace Image

#endif //GUID_FE8C76F6_04AD_4523_AA6E_DAC390E25B86