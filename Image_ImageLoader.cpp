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
#include "Image_ImageLoader.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDIPlus/GDIPlus.h"
#include "Windows/GDIPlus/GDIPlus_Bitmap.h"
//--------------------------------------

//--------------------------------------
//
#include <atlcomcli.h>
#include <Shlwapi.h>
//--------------------------------------

namespace {
    //**************************************************************************
    // Helper Types
    //**************************************************************************
    using Compact4ub_t       = ::Image::ImageData::Compact;
    using GDIPlusStartup_t   = ::Windows::GDIPlus::ScopedStartup;
    using Bitmap_t           = ::Gdiplus::Bitmap;
    using ScopedBitmapLock_t = ::Windows::GDIPlus::ScopedBitmapLockBits;
    using Sparse_t           = typename ::Image::ImageData::Sparse;
    using SparseExtent_t     = typename Sparse_t::extent_type;
    using SparseSize_t       = typename Sparse_t::size_type;
    using IStream_t          = ::IStream;
    using IStreamPtr_t       = ::CComPtr<IStream_t>;

    //**************************************************************************
    // ImageLoader Helpers
    //**************************************************************************
    inline Compact4ub_t GDIPlusLoadImage(Bitmap_t& bitmap) {
        // BUG: This should be done globally; as a static it may cause problems
        //      on destruction (as this may happen when the required info no
        //      longer exists). No errors have occurred so far however!
        static const GDIPlusStartup_t s_ScopedGDIPlusStartup{};

        WinGDIPlusAssertStatus(bitmap);
        if (bitmap.GetLastStatus() == ::Gdiplus::Ok) {
            ScopedBitmapLock_t lock_{
                bitmap,
                Gdiplus::ImageLockModeRead,
                PixelFormat32bppARGB
            };

            if (lock_) {
                const auto bitmapLock = lock_.GetData();
                const auto sparse = Sparse_t{
                    /*iWidth*/        static_cast<SparseExtent_t>(bitmapLock.Width),
                    /*iHeight*/       static_cast<SparseExtent_t>(bitmapLock.Height),
                    /*cBitsPerPixel*/ 32,
                    /*scanStride*/    static_cast<SparseSize_t>(bitmapLock.Stride),
                    /*pixels*/        bitmapLock.Scan0
                };

                return ::Image::ImageData::Compact{ sparse };
            }
        }
        return {};
    }

    //--------------------------------------------------------------------------

    inline IStreamPtr_t OpenFileStream(const char* szFile) noexcept {
        IStream_t* pStream{ nullptr };
        // "SHCreateStreamOnFileA" is depreciated, but still the easiest
        // way to get an IStream for a narrow string filename
        const auto res = ::SHCreateStreamOnFileA(/*pszFile*/ szFile,
                                                 /*grfMode*/ STGM_READ | STGM_SHARE_DENY_NONE,
                                                 /*ppstm*/   &pStream );
        WinAPIAssertHResult(res);
        IStreamPtr_t stream;
        if (res == S_OK) { stream.Attach(pStream); }
        return stream;
    }

    //--------------------------------------------------------------------------

    inline IStreamPtr_t OpenFileStream(const wchar_t* szFile) noexcept {
        IStream_t* pStream{ nullptr };
        const auto res = ::SHCreateStreamOnFileEx(/*pszFile*/      szFile ,
                                                  /*grfMode*/      STGM_READ | STGM_SHARE_DENY_NONE,
                                                  /*dwAttributes*/ FILE_SHARE_READ,
                                                  /*fCreate*/      FALSE    ,
                                                  /*pstmTemplate*/ nullptr  ,
                                                  /*ppstm*/        &pStream );
        WinAPIAssertHResult(res);
        IStreamPtr_t stream;
        if (res == S_OK) { stream.Attach(pStream); }
        return stream;
    }
} // namespace <anonymous>

//==============================================================================

namespace Image {
    //**************************************************************************
    // ImageLoader
    //**************************************************************************
    ImageData::Compact ImageLoader::Load(const void* pData,
                                                        size_type byteSize) {
        auto stream = IStreamPtr_t{ ::SHCreateMemStream(static_cast<const BYTE*>(pData), byteSize) };
        auto bitmap = Bitmap_t{ stream };
        WinGDIPlusAssertStatus(bitmap);
        return ::GDIPlusLoadImage(bitmap);
    }

    //--------------------------------------------------------------------------

    ImageData::Compact ImageLoader::Load(const char* szFile) {
        IStreamPtr_t stream{ OpenFileStream(szFile) };
        if (stream) {
            auto bitmap = Bitmap_t{ stream };
            WinGDIPlusAssertStatus(bitmap);
            return ::GDIPlusLoadImage(bitmap);
        }
        return {};
    }

    //--------------------------------------------------------------------------

    ImageData::Compact ImageLoader::Load(const wchar_t* szFile) {
        auto bitmap = Bitmap_t{ szFile };
        WinGDIPlusAssertStatus(bitmap);
        return ::GDIPlusLoadImage(bitmap);
    }
} // namespace Image
