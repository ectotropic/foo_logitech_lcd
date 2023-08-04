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

//==============================================================================
// Canvas
// ------
//
// Canvas abstracts away the target for drawing and deals with setting up
// OpenGL, GDI in the various supported configurations.
//
// In all cases *both* OpenGL and GDI drawing are used, with each frame split
// into two passes (one for each OpenGL and GDI). GDI is used primarily for
// text support (as OpenGL has no real native text support), but also serves
// as a fall back for some operations if OpenGL does not support them.
//
// Anything that wishes to draw to the canvas can use either OpenGL, GDI, or
// even both, so long as they are drawn in the correct "pass".
//
// No assumption should be made about the order of passes.
//
// Depending on configuration and platform support there will be either one
// or two canvases used for drawing. Two canvases are used by preference and
// should perform better than a single canvas, but for small canvas sizes the
// difference is likely negligible.
//
// When two canvases are used, at the end of the first pass the data is copied
// from one canvas to the other. This may involve significant computation
// depending on configuration and platform support.
//
// There should be no limitations on functionality regardless of platform
// support, however the quality of output will suffer if OpenGL images can not
// be used due to GDI filtering being significantly less capable.
//
// NOTE: The use of two canvases for best performance is partially a historical
//       artefact and may be superfluous with current systems, especially if
//       support for monochrome is not required. The main issue is access to
//       the pixel data, which for modern OpenGL is relatively easy and
//       relatively fast.
//==============================================================================

//--------------------------------------
//
#include "CommonHeaders.h"
#include "Canvas.hpp"
#include "CanvasDebug.hpp"
//--------------------------------------

//--------------------------------------
//
#include "Windows/UI/UI.h"
#include "Windows/UI/UI_WindowClass.h"
//--------------------------------------

//--------------------------------------
//
#include "Config/Config_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorPacker.h"
#include "ColorBlend.h"
//--------------------------------------

namespace {
    static constexpr const auto* const s_szWindowName{ TEXT("foo_logitech_lcd canvas") };
    static constexpr const auto* const s_szClassName { TEXT("foo_logitech_lcd canvas class") };

    //--------------------------------------------------------------------------

    Windows::UI::WindowClass g_WindowClass{};
} // namespace <anonymous>

//==============================================================================

namespace Windows {
    //**************************************************************************
    // Canvas
    //**************************************************************************
    Canvas::Canvas(singleton_constructor_tag /*tag*/) noexcept {
        if (!g_WindowClass) {
            WNDCLASSEX cls{};
            ZeroMemory(&cls, sizeof(WNDCLASSEX));
            cls.cbSize        = sizeof(WNDCLASSEX);
            cls.hInstance     = Windows::GetInstanceHandle();
            cls.lpszClassName = s_szClassName;
            cls.lpfnWndProc   = Windows::UI::WindowProcHelperT<Canvas>::GetWindowProc();
            cls.style         = CS_OWNDC | CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW;
            // Note: Stock Objects do **not** need deleted/released
            cls.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
            WinAPIAssert(cls.hbrBackground);

            g_WindowClass = Windows::UI::WindowClass{ cls };
            WinAPIAssert(g_WindowClass);
        }

        InitialiseDebug();
    }

    //--------------------------------------------------------------------------
    // Initialise
    //--------------------------------------------------------------------------

    void Canvas::Initialise(_In_ coord_type iWidth,
                            _In_ coord_type iHeight,
                            _In_ BYTE cColorBits,
                            _In_ bool bTryUseGLWindow) {
        Uninitialise();

        InitialiseDebugCanvas({ iWidth , iHeight });

        if (bTryUseGLWindow) {
            InitialiseWindowCanvas(iWidth, iHeight, 32); //< Always 32-bit for the window canvas
        }

        InitialiseBitmapCanvas(iWidth, iHeight, cColorBits);

        InitialiseOpenGL();
    }

    //--------------------------------------------------------------------------

    void Canvas::InitialiseWindowCanvas(_In_ coord_type iWidth,
                                        _In_ coord_type iHeight,
                                        _In_ BYTE cColorBits) noexcept {
        window_canvas::CreateParams params{};
        params.hInstance  = ::Windows::GetInstanceHandle();
        params.szClass    = g_WindowClass;
        params.szName     = s_szWindowName;
        params.dwStyle    = WS_POPUP;
        params.dwStyleEx  = WS_EX_NOACTIVATE;
        params.dimensions = { iWidth, iHeight };

        auto canvas = window_canvas::Create(params, TRUE,
                                            PFD_SUPPORT_OPENGL |
                                            PFD_DEPTH_DONTCARE |
                                            PFD_DOUBLEBUFFER_DONTCARE,
                                            cColorBits);
        WinAPIAssert(canvas);
        if (canvas) {
            canvas.HideWindow();
            InitialiseRenderContext(canvas);
            if (m_RenderContext) {
                m_WindowCanvas = std::move(canvas);

                gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
                ::SetWindowLongPtr(m_WindowCanvas,
                                   GWLP_USERDATA,
                                   (LONG_PTR)(this));

                Windows::UI::SetMainWindow(m_WindowCanvas);
            }
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::InitialiseRenderContext(_In_ HDC dc) noexcept {
        WinAPIAssert(dc);
        if (dc) {
            render_context context{ dc };
            WinAPIAssert(context);
            if (context) {
                const auto res = context.MakeCurrent(dc);
                WinAPIAssert(res);
                if (res) {
                    m_RenderContext = std::move(context);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    /*
     * The Bitmap Canvas is a Device Independent Bitmap (i.e. a util Bitmap)
     * that is needed in order to retrieve the pixels that have drawn. If
     * there is no Window Canvas, all drawing happens directly to the Bitmap
     * Canvas. When a Window Canvas is present, any OpenGL rendering is made
     * to _that_ canvas, then copied to the Bitmap Canvas where all GDI
     * rendering happens.
     *
     * This approach is used to allow GDI to be used for text rendering as
     * it is easier than using OpenGL, while OpenGL can be used for other
     * rendering as it is easier than GDI and tends to be far faster. Doing
     * things this way also allows Windows to do conversions to monochrome
     * instead of needing to render in that format or doing those conversions
     * manually.
     *
     * In contrast to the Window Canvas, the color depth is set appropriate
     * for the output, so monochrome output uses single bytes for pixels.
     * When copying from the Window Canvas, Windows will do an automatic
     * conversion of the pixels.
     *
     * NOTE:
     *  - The GDI co-ordinate system and color channel order are different
     *    to OpenGL norms. (The co-ordinate system uses an inverted Y axis
     *    and color channels are BGRA rather than RGBA.) The former is
     *    addressed by using negative heights in various locations, the
     *    latter requires addressing when copying pixels from OpenGL to
     *    GDI or when sending those pixels to an external device. Both of
     *    these continually cause issues when changing code!
     *
     *  - If GDI text rendering is not needed then the use of the Bitmap Canvas
     *    should probably be replaced with something else (e.g. using
     *    glReadPixels to grab the data), this would be easier and faster.
     *
     *  - Device Independent Bitmap traditionally are not hardware accelerated
     *    (although this may no longer be strictly true). The OpenGL
     *    implementation used is likely to be the Windows default version that
     *    supports only OpenGL 1.1 with no extensions. The Window Canvas has
     *    access to other hardware and drivers and is capable of using any
     *    version of OpenGL those support.
     */
    void Canvas::InitialiseBitmapCanvas(_In_ coord_type iWidth,
                                        _In_ coord_type iHeight,
                                        _In_ BYTE cColorBits) {
        const auto srcDC = m_WindowCanvas
            ? m_WindowCanvas.GetDeviceContext()
            : NULL;
        const auto bmWidth  = iWidth;
        const auto bmHeight = -iHeight; //< Minus inverts the bitmap horizontal orientation

        constexpr const auto pfdFlags = PFD_SUPPORT_OPENGL |
                                        PFD_SUPPORT_GDI |
                                        PFD_DEPTH_DONTCARE |
                                        PFD_DOUBLEBUFFER_DONTCARE;
        auto canvas = (cColorBits == 1)
            ? bitmap_canvas::CreateMonochrome(srcDC, bmWidth,
                                              bmHeight, pfdFlags)
            : bitmap_canvas::Create(srcDC, bmWidth, bmHeight,
                                    pfdFlags, cColorBits);
        WinAPIAssert(canvas);
        if (canvas) {
            CDCHandle dc{ canvas.GetDeviceContext() };
            if (dc) {
                if (!m_RenderContext) {
                    InitialiseRenderContext(canvas);
                }

                if (m_RenderContext) {
                    // Note: Stock Objects do **not** need deleted/released
                    dc.SelectBrush((HBRUSH)::GetStockObject(DC_BRUSH));
                    dc.SelectPen((HPEN)::GetStockObject(DC_PEN));
                    dc.SelectFont((HFONT)::GetStockObject(DEVICE_DEFAULT_FONT));

                    dc.SetBkColor(m_BGColor);
                    dc.SetTextColor(m_FGColor);
                    dc.SetDCPenColor(m_FGColor);
                    dc.SetDCBrushColor(m_FGColor);

                    dc.SelectClipRgn(NULL);

                    Windows::GDI::SetCurrentDC(dc);
                    m_BitmapCanvas = std::move(canvas);

                    std::vector<color_type> data;
                    data.resize(m_BitmapCanvas.GetWidth() * m_BitmapCanvas.GetHeight(), m_BGColor);
                    m_pGDIClearImage = gdi_image::NewImage(m_BitmapCanvas,
                                                           m_BitmapCanvas.GetWidth(),
                                                           m_BitmapCanvas.GetHeight(),
                                                           data.data());
                    m_pGDIClearImage->SetTargetDC(m_BitmapCanvas);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    /*
     * InitialiseOpenGL
     * ----------------
     *
     * Initialise and configure OpenGL. Much of the configuration is not really
     * required, but harmless to set and ensures everything is in a known state.
     */
    void Canvas::InitialiseOpenGL() {
        // HACK:
        //  - glew provides no way to "un-initialise" which is a problem since
        //    changing between hardware and software means changing OpenGL
        //    capabilities. For now just manually set the queried values
        //    to false before initialisation.
        __GLEW_ARB_texture_non_power_of_two = GL_FALSE;
        __GLEW_ARB_texture_rectangle        = GL_FALSE;

        const auto err = ::glewInit();
        if (err != GLEW_OK) {
            SPDLOG_ERROR("Failed to initialise glew: \"{}\"",
                         reinterpret_cast<const char*>(::glewGetErrorString(err)));
            return;
        }

        SPDLOG_DEBUG("OpenGL Version: {}" , ::OpenGL::glGetVersionString());
        SPDLOG_DEBUG("OpenGL Vendor: {}"  , ::OpenGL::glGetVendorString());
        SPDLOG_DEBUG("OpenGL Renderer: {}", ::OpenGL::glGetRendererString());

        if (!m_OpenGLPixels.empty()) {
            m_OpenGLPixels.clear();
        }

        if (m_WindowCanvas) {
            // Very likely no hardware does **not** support
            // PBOs anymore, but check anyway
            if (GLEW_ARB_pixel_buffer_object) {
                m_PixelBufferObject.create(m_WindowCanvas.GetColorByteSize(),
                                           GL_STREAM_READ_ARB);
            } else {
                m_PixelBufferObject.destroy();
            }

            // If a PBO couldn't be created, glReadPixels
            // can still be used, but is considerably
            // slower. For a small canvas this is likely not
            // a problem, but it does require a caller
            // supplied buffer to store the data (to replace
            // the PBO that would otherwise be used).
            if (!m_PixelBufferObject) {
                SPDLOG_INFO("Warning: OpenGL Pixel Buffer Object could not be created 'Hardware Accelerated Mode' may be slow!");
                m_OpenGLPixels.resize(m_WindowCanvas.GetColorByteSize());
            }

            ::glHint(GL_FOG_HINT                   , GL_NICEST);
            ::glHint(GL_LINE_SMOOTH_HINT           , GL_NICEST);
            ::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            ::glHint(GL_POINT_SMOOTH_HINT          , GL_NICEST);
            ::glHint(GL_POLYGON_SMOOTH_HINT        , GL_NICEST);
        } else {
            ::glHint(GL_FOG_HINT                   , GL_FASTEST);
            ::glHint(GL_LINE_SMOOTH_HINT           , GL_FASTEST);
            ::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
            ::glHint(GL_POINT_SMOOTH_HINT          , GL_FASTEST);
            ::glHint(GL_POLYGON_SMOOTH_HINT        , GL_FASTEST);
        }

        ::glShadeModel(GL_SMOOTH);

        ::glDisable(GL_CULL_FACE);
        ::glDisable(GL_DEPTH_TEST);
        ::glDisable(GL_LIGHTING);

         // The following will be enabled if/when required
        ::glDisable(GL_TEXTURE_2D);
        ::glDisable(GL_BLEND);
        ::glBlendFunc(GL_ONE, GL_ZERO);

        //No need for mipmaps so we use simpler filtering (and generating is slow)
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP);

        ::glMatrixMode(GL_PROJECTION);
        ::glLoadIdentity();
        ::glOrtho(0, m_BitmapCanvas.GetWidth(),
                  m_BitmapCanvas.GetHeight(), 0,
                  -1, 1);

        ::glMatrixMode(GL_MODELVIEW);
        ::glLoadIdentity();

        if (IsMonochrome()) {
            ::glColor4f(1.f, 1.f, 1.f, 1.f);
            ::glClearColor(0.f, 0.f, 0.f, 0.f);
        } else {
            ::glColor4f(m_glFGColor.r(), m_glFGColor.g(),
                        m_glFGColor.b(), 1.f);// m_glFGColor.a());
            ::glClearColor(m_glBGColor.r(), m_glBGColor.g(),
                           m_glBGColor.b(), 0.f); // m_glBGColor.a());
        }

        OpenGLAssertNoError();

        return;
    }

    //--------------------------------------------------------------------------
    // Uninitialise
    //--------------------------------------------------------------------------

    void Canvas::Uninitialise() noexcept {
        RemoveWallpaper();

        UninitialiseOpenGL();

        UninitialiseBitmapCanvas();
        UninitialiseWindowCanvas();

        UninitialiseDebugCanvas();
    }

    //--------------------------------------------------------------------------

    void Canvas::UninitialiseOpenGL() noexcept {
        if (m_RenderContext) {
            ::glFinish();
        }

        if (m_PixelBufferObject) {
            m_PixelBufferObject.destroy();
        }

        if (!m_OpenGLPixels.empty()) {
            m_OpenGLPixels.clear();
        }

        UninitialiseRenderContext();
    };

    //--------------------------------------------------------------------------

    void Canvas::UninitialiseRenderContext() noexcept {
        if (m_RenderContext) {
            m_RenderContext.Destroy();
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::UninitialiseWindowCanvas() noexcept {
        Windows::UI::SetMainWindow(NULL);
        if (m_WindowCanvas) {
            m_WindowCanvas.Destroy();
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::UninitialiseBitmapCanvas() noexcept {
        Windows::GDI::SetCurrentDC(NULL);
        if (m_pGDIClearImage) {
            m_pGDIClearImage.reset();
        }
        if (m_BitmapCanvas) {
            m_BitmapCanvas.Destroy();
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::ClearWindowCanvas() noexcept {
        const auto canvasWidth  = m_BitmapCanvas.GetWidth();
        const auto canvasHeight = m_BitmapCanvas.GetHeight();

        const auto glFGColor{ m_glFGColor };
        auto glBGColor{ m_glBGColor };
        if (m_bTransparentClears) {
            ::glEnable(GL_BLEND);
            ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // BUG: The alpha needs adjusted depending on current frame rate
            //      (although this is a really shit way of doing this effect
            //       anyway, so probably worth just doing it "properly")
            glBGColor.a(.1f);
        } else {
            ::glBlendFunc(GL_ONE, GL_ZERO);
            ::glDisable(GL_BLEND);
            glBGColor.a(1.f);
        }

        if (m_ImageMode == ImageMode::OpenGL) {
            if (m_iWallpaperX0 != 0) {
                ::glColor4f(glBGColor.r(), glBGColor.g(),
                            glBGColor.b(), glBGColor.a());
                ::glRecti(0, 0, m_iWallpaperX0, canvasHeight);
                ::glRecti(m_iWallpaperX1, 0, canvasWidth, canvasHeight);
                ::glColor4f(glFGColor.r(), glFGColor.g(),
                            glFGColor.b(), 1.f);
            } else if (m_iWallpaperY0 != 0) {
                ::glColor4f(glBGColor.r(), glBGColor.g(),
                            glBGColor.b(), glBGColor.a());
                ::glRecti(0, 0, canvasWidth, m_iWallpaperY0);
                ::glRecti(0, m_iWallpaperY1, canvasWidth, canvasHeight);
                ::glColor4f(glFGColor.r(), glFGColor.g(),
                            glFGColor.b(), 1.f);
            }

            m_pImage->DrawBlendAuto(m_iWallpaperX0, m_iWallpaperY0,
                                    m_iWallpaperX1, m_iWallpaperY1,
                                    glBGColor.a());
        } else {
            if (m_bTransparentClears && m_ImageMode == ImageMode::None) {
                ::glColor4f(glBGColor.r(), glBGColor.g(),
                            glBGColor.b(), glBGColor.a());
                ::glRecti(0, 0, canvasWidth, canvasHeight);
                ::glColor4f(glFGColor.r(), glFGColor.g(),
                            glFGColor.b(), 1.f);
            } else {
                // If m_ImageMode == ImageMode::GDI then the OpenGL buffer
                // is cleared as the contents are not needed even if there
                // are transparent clears
                ::glClear(GL_COLOR_BUFFER_BIT);
            }
        }

        if (m_bTransparentClears) {
            ::glBlendFunc(GL_ONE, GL_ZERO);
            ::glDisable(GL_BLEND);
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::ClearBitmapCanvas() noexcept {
        const auto canvasWidth  = m_BitmapCanvas.GetWidth();
        const auto canvasHeight = m_BitmapCanvas.GetHeight();
        // BUG: The alpha needs adjusted depending on current frame rate
        //      (although this is a really shit way of doing this effect
        //       anyway, so probably worth just doing it "properly")
        const std::uint8_t alpha = m_bTransparentClears ? 25 : 255; //< 25 ~~ 10%
        if (m_ImageMode == ImageMode::GDI) {
            if (m_pGDIClearImage && m_iWallpaperX0 != 0) {
                m_pGDIClearImage->DrawBlendAuto(0, 0,
                                                m_iWallpaperX0, canvasHeight,
                                                alpha);
                m_pGDIClearImage->DrawBlendAuto(m_iWallpaperX1, 0,
                                                canvasWidth, canvasHeight,
                                                alpha);
            } else if (m_pGDIClearImage && m_iWallpaperY0 != 0) {
                m_pGDIClearImage->DrawBlendAuto(0, 0,
                                                canvasWidth, m_iWallpaperY0,
                                                alpha);
                m_pGDIClearImage->DrawBlendAuto(0, m_iWallpaperY1,
                                                canvasWidth, canvasHeight,
                                                alpha);
            }
            m_pImage->DrawBlendAuto(m_iWallpaperX0, m_iWallpaperY0,
                                    m_iWallpaperX1, m_iWallpaperY1,
                                    alpha);
        } else if (!m_WindowCanvas && m_bTransparentClears) {
            m_pGDIClearImage->DrawBlendAuto(0, 0,
                                            canvasWidth, canvasHeight,
                                            alpha);
        } else {
            const RECT rect{ 0, 0, canvasWidth, canvasHeight };
            ::SetDCPenColor(m_BitmapCanvas, m_BGColor);
            ::SetDCBrushColor(m_BitmapCanvas, m_BGColor);
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            WinAPIVerify(::FillRect(m_BitmapCanvas, &rect, m_BGBrush));
            ::SetDCPenColor(m_BitmapCanvas, m_FGColor);
            ::SetDCBrushColor(m_BitmapCanvas, m_FGColor);
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::StartFrame() noexcept {
        // Clear
        // - Only clear using OpenGL if there are two canvases,
        //   the GDI clear will do the work if there is only
        //   one. (Note that some care has to be taken when
        //   partial clears are used (for a trail effect),
        //   so as not to clear multiple times)
        //
        // BUG: If there is no Window Canvas text will be
        //      subject to the trail effect, otherwise it
        //      is not (which is a bit nicer). Easy to fix
        //      but needs yet another canvas and is unlikely
        //      to ever be needed.
        if (m_WindowCanvas) {
            ClearWindowCanvas();
        }

        ClearBitmapCanvas();

        // Set GDI & OpenGL Modes
        // - these values should already be correct, setting them once a
        //   frame has little cost and ensures the frame always starts
        //   with the same settings.

        const auto canvasWidth = m_BitmapCanvas.GetWidth();
        const auto canvasHeight = m_BitmapCanvas.GetHeight();

        { // GDI
            WinAPIVerify(::SetViewportOrgEx(m_BitmapCanvas, 0, 0, NULL));
            WinAPIVerify(::SetViewportExtEx(m_BitmapCanvas,
                                            canvasWidth, canvasHeight,
                                            NULL));
            ::SetBkMode(m_BitmapCanvas, TRANSPARENT); // [DOC]: "Background remains untouched."
            ::SelectClipRgn(m_BitmapCanvas, NULL);
            ::SetTextColor(m_BitmapCanvas, m_FGColor);
            ::SetDCPenColor(m_BitmapCanvas, m_FGColor);
            ::SetDCBrushColor(m_BitmapCanvas, m_FGColor);
        }

        { // OpenGL
            ::glMatrixMode(GL_PROJECTION);
            ::glLoadIdentity();
            ::glOrtho(0, canvasWidth,
                      canvasHeight, 0,
                      -1, 1);

            ::glMatrixMode(GL_MODELVIEW);
            ::glLoadIdentity();

            if (IsMonochrome()) {
                ::glColor4f(1.f, 1.f, 1.f, 1.f);
                ::glClearColor(0.f, 0.f, 0.f, 0.f);
            } else {
                ::glColor4f(m_glFGColor.r(), m_glFGColor.g(), m_glFGColor.b(), 1.f);
                //::glRecti(0, 0, canvasWidth, canvasHeight);
                ::glClearColor(m_glBGColor.r(), m_glBGColor.g(),
                               m_glBGColor.b(), 0.f);
            }
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::StartPass(RenderPass /*pass*/) noexcept {}

    //--------------------------------------------------------------------------

    void Canvas::EndPass(RenderPass pass) noexcept {
        switch (pass) {
            case RenderPass::OpenGL: {
                ::glFlush();
                if (m_WindowCanvas) {
                    constexpr const GLenum glPixelFormat = GL_BGRA;
                    constexpr const GLenum glPixelType = GL_UNSIGNED_BYTE;
                    const auto width = m_WindowCanvas.GetWidth();
                    const auto height = m_WindowCanvas.GetHeight();
                    if (m_PixelBufferObject) {
                        m_PixelBufferObject.bind();
                        ::glReadPixels(0, 0,
                                       width, height,
                                       glPixelFormat,
                                       glPixelType,
                                       0);
                        OpenGLAssertNoError();
                        GLvoid* pBits{ nullptr };
                        m_PixelBufferObject.map(pBits, GL_READ_ONLY_ARB);
                        if (pBits) {
                            WindowBitsToBitmap(pBits);
                            m_PixelBufferObject.unmap();
                        }
                        m_PixelBufferObject.unbind();
                    } else {
                        WinAPIAssert(!m_OpenGLPixels.empty());
                        ::glReadPixels(0, 0,
                                       width, height,
                                       glPixelFormat,
                                       glPixelType,
                                       m_OpenGLPixels.data());
                        OpenGLAssertNoError();
                        WindowBitsToBitmap(m_OpenGLPixels.data());
                    }
                }
                ::glFinish();
                OpenGLAssertNoError();
                break;
            }

            case RenderPass::GDI: {
                ::GdiFlush();
                break;
            }

            HintNoDefault();
        } // switch (pass)
    }

    //--------------------------------------------------------------------------

    void* Canvas::EndFrame() noexcept {
        if (!m_BitmapCanvas) {
            return NULL;
        }

        // If the bit map was  created correctly, the bitmap bits are editable as is
        // and no copy is required to use them. It would be safer to copy them though.
        auto* pBits = m_BitmapCanvas.GetBitmapBits();
        // std::memcpy(..., m_BitmapCanvas.GetBitmapBits(), m_BitmapCanvas.GetColorBytesPerPixel());

        /* HACK:
            When this was originally written it was enough to simple return the data
            to the caller which would contain all the image data as rendered. In the
            years since, something has caused this to stop working as it was previously
            and now when displayed GDI drawn data is corrupt or missing.

            The root of this issue seems to be related to alpha values of pixels
            which GDI sets to zero (GDI has no support for alpha), however, setting
            these to any other value seems to be out-with the scope of GDI.

            The simplest solution is to manually ensure the alpha of each pixel is
            set correctly.

            This **MAY** only be an issue with the Logitech debug tools.

            If migrating to OpenGL for all rendering (GDI is currently used only
            for text) then this is not necessary and should be removed.
        */
        if (!m_BitmapCanvas.IsMonochrome()) {
            DWORD* pSrc = static_cast<DWORD*>(pBits);
            for (auto i = 0; i < m_BitmapCanvas.GetPixelCount(); ++i) {
                (*pSrc++) |= 0xFF000000;
            }
        }

        UpdateDebugCanvas(m_BitmapCanvas.GetDimensions(),
                          m_BitmapCanvas.GetColorBitsPerPixel(),
                          pBits);
        return pBits;
    }

    //--------------------------------------------------------------------------
    // Accessors
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    void Canvas::RemoveWallpaper() noexcept {
        m_ImageMode = ImageMode::None;
        if (m_pImage) {
            m_pImage.reset();
        }
    }

    //--------------------------------------------------------------------------

    void Canvas::SetWallpaper(const image_data& image,
                              bool bStretchToFit) noexcept {
        RemoveWallpaper();

        if (!image) { return; }

        if (m_WindowCanvas) {
            m_pImage = ::Image::OpenGLImage::NewImage(image);
            if (m_pImage) {
                m_ImageMode = ImageMode::OpenGL;
            }
        }

        if (!m_pImage) {
            m_pImage = ::Image::GDIPlusImage::NewImage(m_BitmapCanvas, image);
            if (!m_pImage) {
                m_pImage = ::Image::GDIImage::NewImage(m_BitmapCanvas, image);
            }
            if (m_pImage) {
                m_ImageMode = ImageMode::GDI;
            }
        }

        if (m_ImageMode == ImageMode::None) {
            return;
        }

        const auto canvasWidth  = m_BitmapCanvas.GetWidth();
        const auto canvasHeight = m_BitmapCanvas.GetHeight();
        const auto imageWidth   = image.GetWidth();
        const auto imageHeight  = image.GetHeight();

        m_iWallpaperX0 = 0;
        m_iWallpaperY0 = 0;
        m_iWallpaperX1 = canvasWidth;
        m_iWallpaperY1 = canvasHeight;

        if (bStretchToFit ||
            ((canvasWidth  == imageWidth) &&
             (canvasHeight == imageHeight))) {
            return;
        }

        const auto fImageWidth = static_cast<float>(imageWidth);
        const auto fImageHeight = static_cast<float>(imageHeight);
        const auto fCanvasWidth = static_cast<float>(canvasWidth);
        const auto fCanvasHeight = static_cast<float>(canvasHeight);

        const auto fImageAspect = fImageWidth / fImageHeight;
        const auto fCanvasAspect = fCanvasWidth / fCanvasHeight;

        if (std::fabs(fImageAspect - fCanvasAspect) <= 0.01f) {
            // Aspect almost identical so, just stretch
            return;
        }

        auto fDrawWidth  = fCanvasWidth;
        auto fDrawHeight = fCanvasHeight;

        if (fImageAspect > fCanvasAspect) {
            fDrawHeight = fDrawWidth / fImageAspect;
        } else {
            fDrawWidth  = fDrawHeight * fImageAspect;
        }

        const auto fOffsetX = std::max((fCanvasWidth  - fDrawWidth)  * .5f, 0.f);
        const auto fOffsetY = std::max((fCanvasHeight - fDrawHeight) * .5f, 0.f);

        const auto offsetX = static_cast<coord_type>(std::round(fOffsetX));
        const auto offsetY = static_cast<coord_type>(std::round(fOffsetY));

        const auto drawWidth  = static_cast<coord_type>(std::round(fDrawWidth));
        const auto drawHeight = static_cast<coord_type>(std::round(fDrawHeight));

        m_iWallpaperX0 = offsetX;
        m_iWallpaperY0 = offsetY;
        m_iWallpaperX1 = offsetX + drawWidth;
        m_iWallpaperY1 = offsetY + drawHeight;
    }

    //--------------------------------------------------------------------------

    void Canvas::SetFGColor(color_type color) noexcept {
        if (m_BitmapCanvas.IsMonochrome() || m_FGColor == color) {
            return;
        }

        m_FGColor = color;
        ::Color::PackedColor32ui::ABGR::Unpack(m_FGColor,
                                               m_glFGColor);
        m_glFGColor.a(1.f);

        if (m_BitmapCanvas) {
            CDCHandle dc = m_BitmapCanvas.GetDeviceContext();
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            {
                dc.SetTextColor   (m_FGColor);
                dc.SetDCPenColor  (m_FGColor);
                dc.SetDCBrushColor(m_FGColor);
            }

            try {
                std::vector<COLORREF> imageData;
                imageData.resize(m_BitmapCanvas.GetWidth() * m_BitmapCanvas.GetHeight(),
                                 m_BGColor);
                m_pGDIClearImage = gdi_image::NewImage(m_BitmapCanvas,
                                                       m_BitmapCanvas.GetWidth(),
                                                       m_BitmapCanvas.GetHeight(),
                                                       imageData.data());
                m_pGDIClearImage->SetTargetDC(m_BitmapCanvas);
            } catch (...) {
                assert(false);
            }
        }

        if (m_RenderContext) {
            ::glColor4f(m_glFGColor.r(), m_glFGColor.g(),
                        m_glFGColor.b(), m_glFGColor.a());
        }
    }

    //--------------------------------------------------------------------------

    gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
    void Canvas::SetBGColor(color_type color) noexcept {
        if (m_BitmapCanvas.IsMonochrome() || m_BGColor == color) {
            return;
        }

        m_BGColor = color;
        ::Color::PackedColor32ui::ABGR::Unpack(m_BGColor,
                                               m_glBGColor);
        m_glBGColor.a(0.f);

        m_BGBrush = ::CreateSolidBrush(m_BGColor);
        if (m_BitmapCanvas) {
            CDCHandle dc = m_BitmapCanvas.GetDeviceContext();
            dc.SetBkColor(m_BGColor);
        }

        if (m_RenderContext) {
            ::glClearColor(m_glBGColor.r(), m_glBGColor.g(),
                           m_glBGColor.b(), m_glBGColor.a());
        }
    }

    //--------------------------------------------------------------------------

    bool Canvas::IsValid() noexcept {
        return (m_BitmapCanvas || m_WindowCanvas) && m_RenderContext;
    }

    //--------------------------------------------------------------------------
    // HELPERS
    //--------------------------------------------------------------------------

    void Canvas::WindowBitsToBitmap(void* pBits) noexcept {
        if (m_BitmapCanvas.IsMonochrome()) {
            auto* pSrc{ static_cast<const std::uint32_t*>(pBits) };
            auto* pDst{ static_cast<std::uint8_t*>(m_BitmapCanvas.GetBitmapBits()) };
            for (auto pixel = 0; pixel < m_BitmapCanvas.GetPixelCount(); ++pixel) {
                // if top bit is set in _any_ component, pixel is
                // considered "set" otherwise "unset"
                *pDst++ = (*pSrc++ & 0x00808080) ? 0xFF : 0x00;
            }
        } else if (m_ImageMode == ImageMode::GDI) {
            if (m_bTransparentClears) {
                // FIXME: This is not really the best way to deal with this:
                //        software alpha blending will be pretty slow for larger
                //        screens. However, this code is not likely to be used
                //        (requires a hardware OpenGL that does not support
                //         suitable image formats _and_ using transparent
                //         clears).
                //
                // NOTE: There is a GDI blending function that
                //       might be useful here.
                auto* pSrc{ static_cast<const std::uint32_t*>(pBits) };
                auto* pDst{ static_cast<std::uint32_t*>(m_BitmapCanvas.GetBitmapBits()) };
                for (auto pixel = 0; pixel < m_BitmapCanvas.GetPixelCount(); ++pixel) {
                    const auto src4ub{ Color::PackedColor32ui::ABGR::Unpack<Color::Color4ub>(*pSrc) };
                    if (src4ub.a() == Color::Color4ub::ChannelMax) {
                        *pDst = *pSrc;
                    } else if (src4ub.a() > Color::Color4ub::ChannelMin) {
                        const auto src{ Color::color_cast<Color::Color4f>(src4ub) };
                        const auto dst{ Color::PackedColor32ui::ABGR::Unpack<Color::Color4f>(*pDst) };
                        const auto blend{ Color::ColorAlphaBlend(src, dst) };
                        *pDst = Color::PackedColor32ui::ABGR::Pack(blend);
                    }
                    ++pDst;
                    ++pSrc;
                }
            } else {
                auto* pSrc = static_cast<const std::uint32_t*>(pBits);
                auto* pDst = static_cast<std::uint32_t*>(m_BitmapCanvas.GetBitmapBits());
                for (auto pixel = 0; pixel < m_BitmapCanvas.GetPixelCount(); ++pixel) {
                    if (*pSrc & 0xFF000000) { *pDst = *pSrc; }
                    ++pDst;
                    ++pSrc;
                }
            }
        } else {
            std::memcpy(m_BitmapCanvas.GetBitmapBits(),
                        pBits, m_BitmapCanvas.GetColorByteSize());
        }
    }

    //--------------------------------------------------------------------------

    LRESULT Canvas::WindowProc(_In_ HWND hWnd,
                               _In_ UINT uMsg,
                               _In_ WPARAM wParam,
                               _In_ LPARAM lParam) noexcept {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
} // namespace Windows
