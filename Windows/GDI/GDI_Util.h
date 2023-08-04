#pragma once
#ifndef GUID_D5E18572_46BB_448B_AF0D_F029D5A038D7
#define GUID_D5E18572_46BB_448B_AF0D_F029D5A038D7
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
#include "Windows/GDI/GDI.h"
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // DrawLine
    //**************************************************************************
    inline BOOL DrawLine(_In_ HDC hDC,
                         _In_ INT x1, _In_ INT y1,
                         _In_ INT x2, _In_ INT y2) noexcept {
        assert(hDC);
        POINT oldPosition{ 0, 0 };
        {
            const auto success_ = ::MoveToEx(hDC, x1, y1, &oldPosition);
            WinAPIAssert(success_);
            if (!success_) { return FALSE; }
        }
        WinAPIAssertErrorSuccess();
        WinAPIAssertErrorSuccess("BUGGER!");
        {
            const auto success_ = ::LineTo(hDC, x2, y2);
            WinAPIAssert(success_);
            if (!success_) { return FALSE; }
        }
        {
            const auto success_ = ::MoveToEx(hDC, oldPosition.x, oldPosition.y, NULL);
            WinAPIAssert(success_);
            if (!success_) { return FALSE; }
        }
        return TRUE;
    }

    //------------------------------------------------------

    inline decltype(auto) DrawLine(_In_ HDC hDC,
                                   _In_ POINT start,
                                   _In_ POINT end) noexcept {
        return DrawLine(hDC, start.x, start.y, end.x, end.y);
    }

    //**************************************************************************
    // CopyDIBitsToDevice
    //**************************************************************************
    inline auto CopyDIBitsToDevice(_In_ HDC hDC,
                                   _In_ INT iWidth, _In_ INT iHeight,
                                   _In_ BOOL bInverted,
                                   _In_ INT cColorBits,
                                   _In_ CONST VOID* lpvBits) noexcept {
        BITMAPINFO info;
        ZeroMemory(&info, sizeof(BITMAPINFO));
        info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biPlanes      = 1; //< Docs: "This value must be set to 1."
        info.bmiHeader.biWidth       = iWidth;
        info.bmiHeader.biHeight      = bInverted ? -iHeight : iHeight;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biBitCount    = static_cast<WORD>(cColorBits);
        info.bmiHeader.biSizeImage   = (iWidth * iHeight * cColorBits) / 8;
        const auto res               = ::SetDIBitsToDevice(hDC,
                                             0, 0,
                                             iWidth, iHeight,
                                             0, 0,
                                             0, iHeight,
                                             lpvBits,
                                             &info,
                                             DIB_RGB_COLORS);
        WinAPIAssert(res);
        return res;
    }
} // namespace Windows::GDI

#endif // GUID_D5E18572_46BB_448B_AF0D_F029D5A038D7