#pragma once
#ifndef GUID_B863D567_E7D3_4A8B_9D6D_006A15378179
#define GUID_B863D567_E7D3_4A8B_9D6D_006A15378179
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
#include "RenderCommon.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/FlagEnum.h"
#include "Util/Singleton.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDI/GDI.h"
#include "Windows/GDI/GDI_Canvas.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/wglcore.h"
#include "GL/glbuffer.h"
//--------------------------------------

//--------------------------------------
//
#include "Image_GDI.h"
#include "Image_GDIPlus.h"
#include "Image_OpenGL.h"
//--------------------------------------

namespace Windows {
    //**************************************************************************
    // CanvasFlags
    //**************************************************************************
    FLAG_ENUM(CanvasFlags,
              FLAG_ENUM_VALUES(DualPass = 1 << 0,
                               BlendedCombine = 1 << 1,
                               UseOpenGLWindow = 1 << 2));

    //**************************************************************************
    // Canvas
    //**************************************************************************
    class Canvas final :
        public ::util::singleton<Canvas> {
    public:
        using window_canvas  = ::Windows::GDI::WindowCanvas;
        using bitmap_canvas  = ::Windows::GDI::BitmapCanvas;
        using render_context = ::OpenGL::WGL::RenderContext;
        using image_data     = ::Image::ImageData::Compact;
        using gdi_image      = ::Image::GDIImage;
        using gdi_image_ptr  = ::Image::GDIImagePtr;
        using image_ptr      = ::Image::ImagePtr;

        using string_type = std::wstring;
        using color_type  = COLORREF;
        using coord_type  = INT;

        using gl_color_type   = ::Color::Color4f;
        using gl_pixel_buffer = ::OpenGL::glPixelPackBuffer;
        using gl_pixel_data   = std::vector<std::uint8_t>;

    public:
        Canvas() = delete;

        Canvas(singleton_constructor_tag /*tag*/) noexcept;

        Canvas(const Canvas& ) = default;
        Canvas(      Canvas&&) = default;

        ~Canvas() noexcept {
            Uninitialise();
        }

        Canvas& operator=(const Canvas& ) = default;
        Canvas& operator=(      Canvas&&) = default;

    public: // Windows Message Pump
        LRESULT WindowProc(_In_ HWND hWnd,
                           _In_ UINT Msg,
                           _In_ WPARAM wParam,
                           _In_ LPARAM lParam) noexcept;

    public:
        void Initialise(_In_ coord_type iWidth,
                        _In_ coord_type iHeight,
                        _In_ BYTE cColorBits,
                        _In_ bool bTryUseGLWindow);
        void Uninitialise() noexcept;

        bool IsValid() noexcept;

        void  StartFrame() noexcept;
        void  StartPass (RenderPass pass) noexcept;
        void  EndPass   (RenderPass pass) noexcept;
        void* EndFrame  () noexcept;


        void SetWallpaper(const image_data& image,
                          bool bStretchToFit) noexcept;
        void RemoveWallpaper() noexcept;

        void SetFGColor(color_type color) noexcept;
        void SetBGColor(color_type color) noexcept;

        void SetTransparentClears(bool bEnable) noexcept { m_bTransparentClears = bEnable; }

        decltype(auto) GetDC() const noexcept { return m_BitmapCanvas.GetDeviceContext(); }

        constexpr decltype(auto) GetDimensions() const noexcept { return m_BitmapCanvas.GetDimensions(); }
        constexpr decltype(auto) GetWidth     () const noexcept { return m_BitmapCanvas.GetWidth(); }
        constexpr decltype(auto) GetHeight    () const noexcept { return m_BitmapCanvas.GetHeight(); }
        constexpr decltype(auto) IsMonochrome () const noexcept { return m_BitmapCanvas.IsMonochrome(); }
        constexpr decltype(auto) IsColor      () const noexcept { return !m_BitmapCanvas.IsMonochrome(); }

        constexpr auto HasWallpaper() const noexcept { return m_ImageMode != ImageMode::None; }

        constexpr auto GetFGColor() const noexcept { return m_FGColor; }
        constexpr auto GetBGColor() const noexcept { return m_BGColor; }

    protected:
        void InitialiseWindowCanvas (_In_ coord_type iWidth,
                                     _In_ coord_type iHeight,
                                     _In_ BYTE cColorBits) noexcept;

        void InitialiseBitmapCanvas (_In_ coord_type iWidth,
                                     _In_ coord_type iHeight,
                                     _In_ BYTE cColorBits);
        void InitialiseOpenGL       ();
        void InitialiseRenderContext(_In_ HDC dc) noexcept;

        void UninitialiseOpenGL       () noexcept;
        void UninitialiseRenderContext() noexcept;
        void UninitialiseBitmapCanvas () noexcept;
        void UninitialiseWindowCanvas () noexcept;

        void ClearWindowCanvas() noexcept;
        void ClearBitmapCanvas() noexcept;

        void WindowBitsToBitmap(void* pBits) noexcept;

    private:
        bool m_bTransparentClears{ false };

    private:
        window_canvas   m_WindowCanvas     {};
        bitmap_canvas   m_BitmapCanvas     {};
        render_context  m_RenderContext    {};
        gl_pixel_buffer m_PixelBufferObject{};
        gl_pixel_data   m_OpenGLPixels     {};

        color_type m_FGColor{ RGB(255, 255, 255) };
        color_type m_BGColor{ RGB(0, 0, 0) };
        CBrush     m_BGBrush{ ::CreateSolidBrush(RGB(0, 0, 0)) };

        gl_color_type m_glFGColor{ 1.f, 1.f, 1.f, 1.f };
        gl_color_type m_glBGColor{ 0.f, 0.f, 0.f, 0.f };

    private:
        enum class ImageMode {
            None = 0,
            GDI,
            OpenGL,
        };

        ImageMode     m_ImageMode{ ImageMode::None };
        image_ptr     m_pImage{};
        gdi_image_ptr m_pGDIClearImage{};

        coord_type m_iWallpaperX0{ 0 };
        coord_type m_iWallpaperY0{ 0 };
        coord_type m_iWallpaperX1{ 0 };
        coord_type m_iWallpaperY1{ 0 };
    }; // class Canvas final
} // namespace Windows

#endif // GUID_B863D567_E7D3_4A8B_9D6D_006A15378179