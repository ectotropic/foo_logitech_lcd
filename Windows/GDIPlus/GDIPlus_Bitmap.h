#pragma once
#ifndef GUID_DE9D1B05_8152_40E5_B826_1C895A62063D
#define GUID_DE9D1B05_8152_40E5_B826_1C895A62063D
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
#include "Windows/GDIPlus/GDIPlus.h"
//--------------------------------------

namespace Windows::GDIPlus {
    //**************************************************************************
    // ScopedBitmapLockBits
    //**************************************************************************
    class ScopedBitmapLockBits final {
    private:
        using this_type = ScopedBitmapLockBits;

    public:
        using bitmap      = ::Gdiplus::Bitmap;
        using lock_mode   = ::Gdiplus::ImageLockMode;
        using lock_format = ::Gdiplus::PixelFormat;
        using lock_rect   = ::Gdiplus::Rect;
        using lock        = ::Gdiplus::BitmapData;

    private:
        static auto GetRect(bitmap& bitmap) {
            using int_type = decltype(lock_rect::Width);
            const auto width { static_cast<int_type>(bitmap.GetWidth())  };
            const auto height{ static_cast<int_type>(bitmap.GetHeight()) };
            return lock_rect{ 0, 0, width, height };
        }

        static auto LockBits(bitmap& bitmap,
                             lock& lock,
                             lock_rect rect,
                             lock_mode mode,
                             lock_format format) {
            const auto res = bitmap.LockBits(&rect,
                                             mode, format,
                                             &lock);
            WinGDIPlusAssertStatus(res);
            return res == ::Gdiplus::Ok;
        }

        static decltype(auto) LockBits(bitmap& bitmap,
                                       lock& lock,
                                       lock_mode mode,
                                       lock_format format) {
            const auto rect{ GetRect(bitmap) };
            return LockBits(bitmap, lock, rect,
                            mode, format);
        }

        static decltype(auto) LockBits(bitmap& bitmap,
                                       lock& lock,
                                       lock_mode mode) {
            return LockBits(bitmap, lock, mode,
                            bitmap.GetPixelFormat());
        }

    private:
        ScopedBitmapLockBits(const ScopedBitmapLockBits& )            = delete;
        ScopedBitmapLockBits(      ScopedBitmapLockBits&&)            = delete;
        ScopedBitmapLockBits& operator=(const ScopedBitmapLockBits& ) = delete;
        ScopedBitmapLockBits& operator=(      ScopedBitmapLockBits&&) = delete;

    public:
        ScopedBitmapLockBits(bitmap& bitmap,
                             lock_rect rect,
                             lock_mode mode,
                             lock_format format) :
            m_Bitmap{ bitmap } {
            m_Locked = LockBits(bitmap, m_Lock,
                                rect, mode,
                                format);
        }

        ScopedBitmapLockBits(bitmap& bitmap,
                             lock_mode mode,
                             lock_format format) :
            m_Bitmap{ bitmap } {
            m_Locked = LockBits(bitmap, m_Lock,
                                mode, format);
        }

        ScopedBitmapLockBits(bitmap& bitmap,
                             lock_mode mode) :
            m_Bitmap{ bitmap } {
            m_Locked = LockBits(bitmap, m_Lock, mode);
        }

        ~ScopedBitmapLockBits() noexcept {
            try {
                m_Bitmap.UnlockBits(&m_Lock);
            } catch (...) {
                WinAPIAssert(false);
            }
        }

    public:
        const auto& GetData() const noexcept { return m_Lock; }
              auto& GetData()       noexcept { return m_Lock; }

        constexpr explicit operator bool() const noexcept { return m_Locked; }

    private:
        bool    m_Locked{ false };
        lock    m_Lock  { };
        bitmap& m_Bitmap;
    }; // class ScopedBitmapLockBits final
} // namespace Windows::GDIPlus

#endif // GUID_DE9D1B05_8152_40E5_B826_1C895A62063D