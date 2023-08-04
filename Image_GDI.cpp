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
#include "Image_GDI.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // GDIImage <static>
    //**************************************************************************
    GDIImage::pointer GDIImage::NewImage(_In_ HDC hDC,
                                         _In_ native_coord_type width,
                                         _In_ native_coord_type height,
                                         _In_ const void* data) noexcept {
        try {
            const auto hBitmapDC = ::CreateCompatibleDC(hDC);
            WinAPIAssert(hBitmapDC); if (!hBitmapDC) { return {}; }

            pointer pImage{ new GDIImage{width, height, hBitmapDC} };
            WinAPIAssert(pImage && pImage->m_DC);

            const auto byteSize{ width * height * 4 };
            Windows::GDI::BitmapInfo bmi{};
            bmi.bmiHeader.biWidth       = width;
            bmi.bmiHeader.biHeight      = -height; //< Inverted orientation
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biBitCount    = 32;
            bmi.bmiHeader.biSizeImage   = byteSize;
            void* pBitmapBits{ nullptr };
            pImage->m_Bitmap = ::CreateDIBSection(pImage->m_DC,
                                                  bmi, DIB_RGB_COLORS,
                                                  &pBitmapBits,
                                                  NULL, 0);
            WinAPIAssert(pImage->m_Bitmap && pBitmapBits);
            if (pImage->m_Bitmap && pBitmapBits) {
                pImage->m_OriginalDCBitmap = (HBITMAP)::SelectObject(pImage->m_DC,
                                                                     pImage->m_Bitmap);
                pImage->m_iWidth  = width;
                pImage->m_iHeight = height;
                std::memcpy(pBitmapBits, data, byteSize);
                return pImage;
            }
        } catch (...) {
            assert(false);
        }
        return {};
    }

    //--------------------------------------------------------------------------

    GDIImage::pointer GDIImage::NewImage(_In_ HDC hDC,
                                         _In_ const data_type& data) noexcept {
        return NewImage(hDC,
                        data.GetWidth(),
                        data.GetHeight(),
                        data.GetPixels());
    }

    //**************************************************************************
    // GDIImage
    //**************************************************************************
    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void GDIImage::Destroy() noexcept {
        if (m_OriginalDCBitmap) {
            m_DC.SelectBitmap(m_OriginalDCBitmap.Detach());
        }
        if (m_Bitmap) { m_Bitmap.DeleteObject(); }
        if (m_DC)     { m_DC.DeleteDC(); }
        base_type::Destroy();
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void GDIImage::Draw(coord_type x0, coord_type y0,
                        coord_type x1, coord_type y1) const noexcept {
        const auto hDC{ m_TargetDC ? m_TargetDC : Windows::GDI::GetCurrentDC() };
        const auto dstWidth  = x1 - x0;
        const auto dstHeight = y1 - y0;
        WinAPIVerify(::StretchBlt(/*hdcDest*/ hDC      ,
                                  /*xDest*/   x0       , /*yDest*/ y0,
                                  /*wDest*/   dstWidth , /*hDest*/ dstHeight,
                                  /*hdcSrc*/  m_DC     ,
                                  /*xSrc*/    0        , /*ySrc*/  0,
                                  /*wSrc*/    m_iWidth , /*hSrc*/  m_iHeight,
                                  /*rop*/     SRCCOPY  ));
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void GDIImage::Blend(coord_type x0, coord_type y0,
                         coord_type x1, coord_type y1,
                         byte_type alpha) const noexcept {
        const auto hDC{ m_TargetDC ? m_TargetDC : Windows::GDI::GetCurrentDC() };

        BLENDFUNCTION blendFunc;
        ZeroMemory(&blendFunc, sizeof(BLENDFUNCTION));
        blendFunc.SourceConstantAlpha = alpha;
        if (alpha == 255) {
            blendFunc.BlendOp     = AC_SRC_OVER;
            blendFunc.AlphaFormat = AC_SRC_ALPHA;
        }

        const auto dstWidth  = x1 - x0;
        const auto dstHeight = y1 - y0;
        WinAPIVerify(::GdiAlphaBlend(/*hdcDest*/ hDC       ,
                                     /*xDest*/   x0        , /*yDest*/ y0,
                                     /*wDest*/   dstWidth  , /*hDest*/ dstHeight,
                                     /*hdcSrc*/  m_DC      ,
                                     /*xSrc*/    0         , /*ySrc*/  0,
                                     /*wSrc*/    m_iWidth  , /*hSrc*/  m_iHeight,
                                     /*ftn*/     blendFunc ));
    }
} // namespace Image
