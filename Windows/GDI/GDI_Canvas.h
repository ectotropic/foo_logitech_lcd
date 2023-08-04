#pragma once
#ifndef GUID_4C1EFF73_3B80_4C42_8F52_2FC0ABC6CBBB
#define GUID_4C1EFF73_3B80_4C42_8F52_2FC0ABC6CBBB
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
#include "Windows/GDI/GDI_Bitmap.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

namespace Windows::GDI::detail {
    //**************************************************************************
    // CanvasCommon
    //**************************************************************************
    class CanvasCommon {
    protected:
        template <class TypeT>
        static decltype(auto) exchange_with_null(TypeT& val) noexcept {
            return std::exchange(val, (TypeT)NULL);
        }

    private:
        CanvasCommon(const CanvasCommon&)            = delete; // No Copy
        CanvasCommon& operator=(const CanvasCommon&) = delete; // No Copy

    protected:
        constexpr CanvasCommon() noexcept = default;

        constexpr CanvasCommon(_In_ SIZE dims,
                               _In_ BYTE colorBits,
                               _In_ BYTE depthBits = 0,
                               _In_ BYTE stencilBits = 0,
                               _In_ INT pixelFormat = 0,
                               _In_ HDC hDC = NULL) noexcept :
            m_Dimensions { dims.cx,  (dims.cy > 0 ? dims.cy : -dims.cy) },
            m_ColorBits  { colorBits },
            m_DepthBits  { depthBits },
            m_StencilBits{ stencilBits },
            m_PixelFormat{ pixelFormat },
            m_hDC        { hDC } {}
        constexpr CanvasCommon(_In_ LONG width,
                               _In_ LONG height,
                               _In_ BYTE colorBits,
                               _In_ BYTE depthBits = 0,
                               _In_ BYTE stencilBits = 0,
                               _In_ INT pixelFormat = 0,
                               _In_ HDC hDC = NULL) noexcept :
            CanvasCommon{ SIZE{ width, height }, colorBits, depthBits, stencilBits, pixelFormat, hDC } {}

        ~CanvasCommon() = default;

    public:
        CanvasCommon(_In_ CanvasCommon&& other) noexcept :
            m_Dimensions { std::exchange(other.m_Dimensions , SIZE{ 0, 0 }) },
            m_ColorBits  { std::exchange(other.m_ColorBits  , 0) },
            m_DepthBits  { std::exchange(other.m_DepthBits  , 0) },
            m_StencilBits{ std::exchange(other.m_StencilBits, 0) },
            m_PixelFormat{ std::exchange(other.m_PixelFormat, 0) },
            m_hDC        { exchange_with_null(other.m_hDC) } {}

        CanvasCommon& operator=(_In_ CanvasCommon&& other) noexcept {
            m_Dimensions  = std::exchange(other.m_Dimensions , SIZE{ 0, 0 });
            m_ColorBits   = std::exchange(other.m_ColorBits  , 0);
            m_DepthBits   = std::exchange(other.m_DepthBits  , 0);
            m_StencilBits = std::exchange(other.m_StencilBits, 0);
            m_PixelFormat = std::exchange(other.m_PixelFormat, 0);
            m_hDC         = exchange_with_null(other.m_hDC);
            return *this;
        }

        constexpr HDC GetDeviceContext() const noexcept { return m_hDC; }

        constexpr auto GetDimensions () const noexcept { return m_Dimensions; }
        constexpr auto GetWidth      () const noexcept { return m_Dimensions.cx; }
        constexpr auto GetHeight     () const noexcept { return m_Dimensions.cy; }
        constexpr auto GetPixelCount () const noexcept { return m_Dimensions.cx * m_Dimensions.cy; }
        constexpr auto GetPixelFormat() const noexcept { return m_PixelFormat; }

        constexpr auto GetColorBitsPerPixel () const noexcept { return m_ColorBits; }
        constexpr auto GetColorBytesPerPixel() const noexcept { return GetColorBitsPerPixel() / 8; }
        constexpr auto GetColorByteSize     () const noexcept {
            return GetPixelCount() * GetColorBytesPerPixel();
        }

        constexpr auto HasDepthBuffer       () const noexcept { return m_DepthBits != 0; }
        constexpr auto GetDepthBitsPerPixel () const noexcept { return m_DepthBits; }
        constexpr auto GetDepthBytesPerPixel() const noexcept { return GetDepthBitsPerPixel() / 8; }
        constexpr auto GetDepthByteSize     () const noexcept {
            return GetPixelCount() * GetDepthBytesPerPixel();
        }

        constexpr auto HasStencilBuffer       () const noexcept { return m_DepthBits != 0; }
        constexpr auto GetStencilBitsPerPixel () const noexcept { return m_StencilBits; }
        constexpr auto GetStencilBytesPerPixel() const noexcept { return GetStencilBitsPerPixel() / 8; }
        constexpr auto GetStencilByteSize     () const noexcept {
            return GetPixelCount() * GetDepthBytesPerPixel();
        }

    protected:
        constexpr bool Valid() const noexcept {
            return m_Dimensions.cx && m_Dimensions.cy &&
                m_ColorBits && m_PixelFormat && m_hDC;
        }

    protected:
        SIZE m_Dimensions { 0, 0 };
        BYTE m_ColorBits  { 0 };
        BYTE m_DepthBits  { 0 };
        BYTE m_StencilBits{ 0 };
        INT  m_PixelFormat{ 0 };
        HDC  m_hDC        { NULL };
    };
} // namespace Windows::GDI::detail

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // WindowCanvas
    //**************************************************************************
    class WindowCanvas :
        public detail::CanvasCommon {
    private:
        using baseClass = detail::CanvasCommon;
        using thisClass = WindowCanvas;

    private:
        WindowCanvas(const WindowCanvas&)            = delete; // No Copy
        WindowCanvas& operator=(const WindowCanvas&) = delete; // No Copy

    public:
        WindowCanvas() = default;

        WindowCanvas(_In_ WindowCanvas&& other) noexcept :
            baseClass{ std::forward<WindowCanvas>(other) },
            m_hWnd   { exchange_with_null(other.m_hWnd) } {
        }

        WindowCanvas& operator=(_In_ WindowCanvas&& other) noexcept {
            Destroy();
            baseClass::operator=(std::forward<WindowCanvas>(other));
            m_hWnd = exchange_with_null(other.m_hWnd);
            return *this;
        }

        ~WindowCanvas() noexcept { Destroy(); }

    public:
        bool Destroy() noexcept {
            bool res = true;

            if (m_hDC) {
                const auto resDC = ::ReleaseDC(m_hWnd, exchange_with_null(m_hDC));;
                WinAPIAssert(resDC);
                res = res && resDC;
            }

            if (m_hWnd) {
                const auto resWnd = ::DestroyWindow(exchange_with_null(m_hWnd));
                WinAPIAssert(resWnd);
                res = res && resWnd;
            }
            return res;
        }

        auto ShowWindow(_In_ int nCmdShow) const noexcept {
            return ::ShowWindow(m_hWnd, nCmdShow);
        }

        auto ShowWindow() const noexcept { return ShowWindow(SW_SHOW); }
        auto HideWindow() const noexcept { return ShowWindow(SW_HIDE); }

        constexpr auto GetWindow() const noexcept { return m_hWnd; }

        constexpr operator HWND() const noexcept { return GetWindow(); }
        constexpr operator HDC () const noexcept { return GetDeviceContext();  }

        constexpr bool IsMonochrome() const noexcept { return false; }

        constexpr bool Valid() const noexcept { return baseClass::Valid() && m_hWnd; }

    public:
        explicit constexpr operator bool() const noexcept { return Valid(); }

    public:
        struct CreateParams {
            HINSTANCE hInstance { NULL };
            LPCTSTR   szClass   { NULL };
            LPCTSTR   szName    { NULL };
            POINT     location  { CW_USEDEFAULT, CW_USEDEFAULT };
            SIZE      dimensions{ 0, 0 };
            DWORD     dwStyle   { 0 };
            DWORD     dwStyleEx { 0 };
            HWND      hWndParent{ NULL };
            HMENU     hMenu     { NULL };
            LPVOID    lpParam   { NULL };
        };

        static WindowCanvas Create(_In_ const CreateParams& params,
                                   _In_ BOOL bClientDC,
                                   _In_ DWORD dwPixelFormatFlags,
                                   _In_ BYTE cColorBits,
                                   _In_ BYTE cDepthBits = 0,
                                   _In_ BYTE cStencilBits = 0) noexcept {
            WindowCanvas canvas{};
            canvas.m_hWnd = ::CreateWindowEx(params.dwStyleEx,
                                           params.szClass, params.szName,
                                           params.dwStyle,
                                           params.location.x, params.location.y,
                                           params.dimensions.cx, params.dimensions.cy,
                                           params.hWndParent,
                                           params.hMenu,
                                           params.hInstance,
                                           params.lpParam);
            WinAPIAssert(canvas.m_hWnd);
            if (!canvas.m_hWnd) { return {}; }

            if (bClientDC) {
                canvas.m_hDC = ::GetDC(canvas.m_hWnd);
            } else {
                canvas.m_hDC = ::GetWindowDC(canvas.m_hWnd);
            }
            WinAPIAssert(canvas.m_hDC);
            if (!canvas.m_hDC) { return {}; }

            PIXELFORMATDESCRIPTOR pfd;
            ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
            pfd.nVersion     = 1;
            pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.iPixelType   = PFD_TYPE_RGBA;
            pfd.dwFlags      = dwPixelFormatFlags | PFD_DRAW_TO_WINDOW;
            pfd.cColorBits   = cColorBits;
            pfd.cDepthBits   = cDepthBits;
            pfd.cStencilBits = cStencilBits;

            const auto format = ::ChoosePixelFormat(canvas.m_hDC, &pfd);
            WinAPIAssert(format);
            if (!format) { return {}; }

            const auto width     = params.dimensions.cx;
            const auto height    = params.dimensions.cy;
            const auto absHeight = (height > 0) ? height : -height;

            canvas.m_Dimensions  = { width, absHeight };
            canvas.m_ColorBits   = cColorBits;
            canvas.m_DepthBits   = cDepthBits;
            canvas.m_StencilBits = cStencilBits;
            canvas.m_PixelFormat = format;

            const auto res = ::SetPixelFormat(canvas.m_hDC, format, &pfd);
            WinAPIAssert(res);
            if (!res) { return {}; }

            return canvas;
        }

    private:
        HWND m_hWnd{ NULL };
    };

    //**************************************************************************
    // BitmapCanvas
    //**************************************************************************
    class BitmapCanvas :
        public detail::CanvasCommon {
    private:
        using baseClass = detail::CanvasCommon;
        using thisClass = BitmapCanvas;

    private:
        BitmapCanvas(const BitmapCanvas&)            = delete; // No Copy
        BitmapCanvas& operator=(const BitmapCanvas&) = delete; // No Copy

    public:
        BitmapCanvas() = default;

        BitmapCanvas(_In_ BitmapCanvas&& other) noexcept :
            baseClass          { std::forward<BitmapCanvas>(other) },
            m_bMonochrome      { std::exchange(other.m_bMonochrome, false) },
            m_hOriginalDCBitmap{ exchange_with_null(other.m_hOriginalDCBitmap) },
            m_hBitmap          { exchange_with_null(other.m_hBitmap) },
            m_pBitmapBits      { exchange_with_null(other.m_pBitmapBits) } {}

        BitmapCanvas& operator=(_In_ BitmapCanvas&& other) noexcept {
            Destroy();
            baseClass::operator=(std::forward<BitmapCanvas>(other));
            m_bMonochrome       = std::exchange(other.m_bMonochrome, false);
            m_hOriginalDCBitmap = exchange_with_null(other.m_hOriginalDCBitmap);
            m_hBitmap           = exchange_with_null(other.m_hBitmap);
            m_pBitmapBits       = exchange_with_null(other.m_pBitmapBits);
            return *this;
        }

        ~BitmapCanvas() noexcept { Destroy(); }

    public:
        bool Destroy() noexcept {
            bool res = true;

            if (m_hBitmap) {
                if (m_hOriginalDCBitmap) {
                    ::SelectObject(m_hDC, exchange_with_null(m_hOriginalDCBitmap));
                }
                const auto resBM = ::DeleteObject(exchange_with_null(m_hBitmap));
                WinAPIAssert(resBM);
                res = res && resBM;
            }

            if (m_hDC) {
                const auto resDC = ::DeleteDC(exchange_with_null(m_hDC));
                WinAPIAssert(resDC);
                res = res && resDC;
            }
            return res;
        }

        constexpr const HBITMAP GetBitmap    () const noexcept { return m_hBitmap; }
        constexpr       HBITMAP GetBitmap    ()       noexcept { return m_hBitmap; }
        constexpr const void*   GetBitmapBits() const noexcept { return m_pBitmapBits; }
        constexpr       void*   GetBitmapBits()       noexcept { return m_pBitmapBits; }

        constexpr operator HDC    () const noexcept { return GetDeviceContext(); }
        constexpr operator HBITMAP() const noexcept { return GetBitmap(); }

        constexpr bool IsMonochrome() const noexcept { return m_bMonochrome; }

        constexpr bool Valid() const noexcept { return baseClass::Valid() && m_hBitmap; }

    public:
        constexpr explicit operator bool() const noexcept { return Valid(); }

    public:
        static BitmapCanvas Create(_In_opt_ HDC hDC,
                                   _In_ const BITMAPINFO* pbmi,
                                   _In_ DWORD dwPixelFormatFlags,
                                   _In_ BYTE cDepthBits = 0,
                                   _In_ BYTE cStencilBits = 0) noexcept {
            assert(pbmi); if (!pbmi) { return {}; }

            BitmapCanvas canvas;
            canvas.m_hDC = ::CreateCompatibleDC(hDC);
            WinAPIAssert(canvas.m_hDC);
            if (!canvas.m_hDC) { return {}; }

            canvas.m_hBitmap = ::CreateDIBSection(canvas.m_hDC,
                                                  pbmi, DIB_RGB_COLORS,
                                                  &canvas.m_pBitmapBits,
                                                  NULL, 0);
            WinAPIAssert(canvas.m_hBitmap && canvas.m_pBitmapBits);
            if (!(canvas.m_hBitmap && canvas.m_pBitmapBits)) { return {}; }

            const auto cColorBits = static_cast<BYTE>(pbmi->bmiHeader.biBitCount);

            canvas.m_hOriginalDCBitmap = (HBITMAP)::SelectObject(canvas.m_hDC,
                                                                 canvas.m_hBitmap);
            PIXELFORMATDESCRIPTOR pfd;
            ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
            pfd.nVersion     = 1; pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.iPixelType   = PFD_TYPE_RGBA;
            pfd.dwFlags      = dwPixelFormatFlags | PFD_DRAW_TO_BITMAP;
            pfd.cColorBits   = cColorBits;
            pfd.cDepthBits   = cDepthBits;
            pfd.cStencilBits = cStencilBits;

            const auto format = ::ChoosePixelFormat(canvas.m_hDC, &pfd);
            WinAPIAssert(format);
            if (!format) { return {}; }

            const auto res = ::SetPixelFormat(canvas.m_hDC, format, &pfd);
            WinAPIAssert(res);
            if (!res) { return {}; }

            const auto width     = pbmi->bmiHeader.biWidth;
            const auto height    = pbmi->bmiHeader.biHeight;
            const auto absHeight = (height > 0) ? height : -height;

            canvas.m_Dimensions  = { width, absHeight };
            canvas.m_ColorBits   = cColorBits;
            canvas.m_DepthBits   = cDepthBits;
            canvas.m_StencilBits = cStencilBits;
            canvas.m_PixelFormat = format;
            return canvas;
        }

        static decltype(auto) Create(_In_opt_ HDC hDC,
                                     _In_ const BITMAPINFO& bmi,
                                     _In_ DWORD dwPixelFormatFlags,
                                     _In_ BYTE cDepthBits = 0,
                                     _In_ BYTE cStencilBits = 0) noexcept {
            return Create(hDC, &bmi,
                          dwPixelFormatFlags,
                          cDepthBits, cStencilBits);
        }

        static decltype(auto) Create(_In_opt_ HDC hDC,
                                     _In_ LONG width,
                                     _In_ LONG height,
                                     _In_ DWORD dwPixelFormatFlags,
                                     _In_ BYTE cColorBits,
                                     _In_ BYTE cDepthBits = 0,
                                     _In_ BYTE cStencilBits = 0) noexcept {
            BitmapInfo256 bmi;
            bmi.bmiHeader.biWidth       = width;
            bmi.bmiHeader.biHeight      = height;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biBitCount    = cColorBits;
            return Create(hDC, bmi,
                          dwPixelFormatFlags,
                          cDepthBits, cStencilBits);
        }

        static auto CreateMonochrome(_In_opt_ HDC hDC,
                                     _In_ LONG width,
                                     _In_ LONG height,
                                     _In_ DWORD dwPixelFormatFlags,
                                     _In_ BYTE cDepthBits = 0,
                                     _In_ BYTE cStencilBits = 0) noexcept {
            const auto absHeight = height > 0 ? height : -height; //< "height" may be negative
            BitmapInfo256 bmi;
            bmi.bmiHeader.biWidth       = width;
            bmi.bmiHeader.biHeight      = height;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biBitCount    = 8; //< Need 1 byte minimum
            bmi.bmiHeader.biSizeImage   = width * absHeight; //< 1 byte per pixel
            bmi.bmiHeader.biClrUsed     = BitmapInfo256::ColorCount;

            // Values from 0 - 127 are already zero
            for (auto i = (BitmapInfo256::ColorCount / 2); i < BitmapInfo256::ColorCount; ++i) {
                bmi.bmiColors[i] = RGBQUAD{ 255, 255, 255, 0 };
            }

            auto canvas = Create(hDC, bmi, dwPixelFormatFlags, cDepthBits, cStencilBits);
            if (canvas) { canvas.m_bMonochrome = true; }
            return canvas;
        }

    private:
        bool    m_bMonochrome      { false };
        HBITMAP m_hOriginalDCBitmap{ NULL };
        HBITMAP m_hBitmap          { NULL };
        void*   m_pBitmapBits      { nullptr };
    };
}

#endif // GUID_4C1EFF73_3B80_4C42_8F52_2FC0ABC6CBBB