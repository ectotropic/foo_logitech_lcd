#pragma once
#ifndef GUID_2034F456_E0E6_4B78_A419_D395E3DA7C55
#define GUID_2034F456_E0E6_4B78_A419_D395E3DA7C55
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
#include "Windows/GDI/GDI_Bitmap.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // GDIImage
    //**************************************************************************
    class GDIImage final : public ::Image::Image {
    private:
        using base_type = ::Image::Image;
        using this_type = ::Image::GDIImage;

    public:
        using pointer            = managed_pointer_type<this_type>;
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
        GDIImage(native_extent_type width,
                 native_extent_type height,
                 HDC hDC) noexcept :
            base_type{ width, height },
            m_DC{ hDC } {}

        GDIImage(const GDIImage&)            = delete; // No Copy
        GDIImage& operator=(const GDIImage&) = delete; // No Copy

    public:
        GDIImage() = default;

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        GDIImage(GDIImage&& other) noexcept :
            base_type         { std::forward<GDIImage>(other) },
            m_DC              { other.m_DC.Detach() },
            m_Bitmap          { other.m_Bitmap.Detach() },
            m_OriginalDCBitmap{ other.m_OriginalDCBitmap.Detach() }
        {}

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        GDIImage& operator=(GDIImage&& other) noexcept {
            Destroy();
            base_type::operator=(std::forward<base_type>(other));
            m_DC               = other.m_DC.Detach();
            m_Bitmap           = other.m_Bitmap.Detach();
            m_OriginalDCBitmap = other.m_OriginalDCBitmap.Detach();
            return *this;
        }

    public: // Overrides
        virtual void Destroy() noexcept override;

        virtual void Draw(coord_type x0, coord_type y0,
                          coord_type x1, coord_type y1) const noexcept override;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           byte_type alpha) const noexcept override;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           float fAlpha = 1.f) const noexcept override {
            byte_type alpha{ 0 };
            if (fAlpha > 1.f) {
                alpha = 255;
            } else if (fAlpha > 0.f) {
                gsl_suppress(26467) // C26467: Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value.
                alpha = static_cast<byte_type>(255.f * fAlpha);
            }
            Blend(x0, y0, x1, y1, alpha);
        }

        virtual bool Valid() const noexcept override {
            return base_type::Valid() && m_Bitmap.m_hBitmap;
        }

    public: // Helpers
        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void SetTargetDC(HDC hDC) noexcept {
            m_TargetDC = hDC;
        }

    private:
        CDCHandle     m_TargetDC        { NULL };
        CDC           m_DC              { NULL };
        CBitmap       m_Bitmap          { NULL };
        CBitmapHandle m_OriginalDCBitmap{ NULL };
    }; // class GDIImage final

    //**************************************************************************
    // GDIImagePtr
    //**************************************************************************
    using GDIImagePtr = ::Image::GDIImage::pointer;
} // namespace Image

#endif //GUID_2034F456_E0E6_4B78_A419_D395E3DA7C55