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
#include "Image_GDIPlus.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDIPlus/GDIPlus_Bitmap.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // GDIPlusImage <static>
    //**************************************************************************
    GDIPlusImage::pointer GDIPlusImage::NewImage(_In_ HDC hDC,
                                                 _In_ native_coord_type width,
                                                 _In_ native_coord_type height,
                                                 _In_ const void* data) noexcept {
        // BUG: This should be done globally; as a static it may cause problems
        //      on destruction (as this may happen when the required info no
        //      longer exists). No errors have occurred so far however!
        static const ::Windows::GDIPlus::ScopedStartup s_ScopedGDIPlusStartup{};

        try {
            graphics_pointer pGraphics{ ::Gdiplus::Graphics::FromHDC(hDC) };
            assert(pGraphics);
            WinGDIPlusAssertStatus(pGraphics);
            if (!pGraphics) { return {}; }
            if (pGraphics->GetLastStatus() != ::Gdiplus::Ok) { return {}; }

            pGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            pointer pImage {};
            {
                pImage.reset(new GDIPlusImage{ width, height, std::move(pGraphics) });
                pImage->m_pBitmap.reset(new ::Gdiplus::Bitmap{ width, height });
            }

            if (pImage->m_pBitmap->GetLastStatus() != ::Gdiplus::Ok) { return {}; }

            ::Windows::GDIPlus::ScopedBitmapLockBits lock_{
                *pImage->m_pBitmap,
                Gdiplus::ImageLockModeWrite,
                PixelFormat32bppARGB
            };
            if (lock_) {
                const auto bitmapLock{ lock_.GetData() };
                std::memcpy(bitmapLock.Scan0, data,
                            width * height * 4);
                return pImage;
            }
        } catch (...) {
            assert(false);
        }
        return {};
    }

    //--------------------------------------------------------------------------

    GDIPlusImage::pointer GDIPlusImage::NewImage(_In_ HDC hDC,
                                                 _In_ const data_type& data) noexcept {
        if (!data) { return {}; }
        return NewImage(hDC, data.GetWidth(),
                        data.GetHeight(), data.GetPixels());
    }

    //**************************************************************************
    // GDIPlusImage
    //**************************************************************************
    GDIPlusImage::GDIPlusImage(native_extent_type width,
                               native_extent_type height,
                               graphics_pointer&& pGraphics) noexcept :
        base_type{ static_cast<extent_type>(width), static_cast<extent_type>(height) },
        m_pGraphics{ std::forward<graphics_pointer>(pGraphics) } {
        WinGDIPlusAssertStatus(m_pGraphics);
    }

    //--------------------------------------------------------------------------

    void GDIPlusImage::Destroy() noexcept {
        if (m_pBitmap)   { m_pBitmap.reset(); }
        if (m_pGraphics) { m_pGraphics.reset(); }
        base_type::Destroy();
    }

    //--------------------------------------------------------------------------

    void GDIPlusImage::Draw(_In_ coord_type x0, _In_ coord_type y0,
                            _In_ coord_type x1, _In_ coord_type y1) const noexcept {
        const auto  dstWidth = x1 - x0;
        const auto dstHeight = y1 - y0;
        try {
            m_pGraphics->DrawImage(m_pBitmap.get(), x0, y0, dstWidth, dstHeight);
        } catch (...) {
            assert(false);
        }
    }

    //--------------------------------------------------------------------------

    void GDIPlusImage::Blend(_In_ coord_type x0, _In_ coord_type y0,
                             _In_ coord_type x1, _In_ coord_type y1,
                             _In_ float alpha) const noexcept {
        try {
            // https://www.codeproject.com/Articles/9778/Alpha-Blending-using-GDI
            // Diagonal values are R, G, B, A, w
            const auto gdiAlpha{ static_cast<Gdiplus::REAL>(alpha) };
            const Gdiplus::ColorMatrix matrix{
                1.f,    0.f,    0.f,     0.f,        0.f,
                0.f,    1.f,    0.f,     0.f,        0.f,
                0.f,    0.f,    1.f,     0.f,        0.f,
                0.f,    0.f,    0.f,     gdiAlpha,   0.f,
                0.f,    0.f,    0.f,     0.f,        1.f
            };

            Gdiplus::ImageAttributes ImgAttr{};

            ImgAttr.SetColorMatrix(&matrix,
                                   Gdiplus::ColorMatrixFlagsDefault,
                                   Gdiplus::ColorAdjustTypeBitmap);
            const auto  dstWidth = x1 - x0;
            const auto dstHeight = y1 - y0;
            m_pGraphics->DrawImage(m_pBitmap.get(),
                                   Gdiplus::Rect{ x0, y0, dstWidth, dstHeight },
                                   0,        0,
                                   m_iWidth, m_iHeight,
                                   Gdiplus::UnitPixel,
                                   &ImgAttr);
        } catch (...) {
            assert(false);
        }
    }
} // namespace Image
