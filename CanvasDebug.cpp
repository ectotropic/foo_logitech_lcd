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
#include "CanvasDebug.hpp"
//--------------------------------------

#if defined(DEBUG) || defined(_DEBUG) || defined(foo_logitech_lcd_ENABLE_DEBUG_CANVAS)


//--------------------------------------
//
#include "Windows/UI/UI.h"
#include "Windows/UI/UI_WindowClass.h"
#include "Windows/Windows_Error.h"
#include "Windows/Windows_StopWatch.h"
#include "Windows/GDI/GDI_Font.h"
#include "Windows/GDI/GDI_Bitmap.h"
//--------------------------------------

//--------------------------------------
//
#include <gl/GLew.h>
#include <cassert>
//--------------------------------------

namespace {
    //**************************************************************************
    // Constants
    //**************************************************************************
    static constexpr const auto* const s_szClassName     { TEXT("foo_logitech_lcd debug window class") };
    static constexpr const auto* const s_szWindowName    { TEXT("foo_logitech_lcd debug window") };
    static constexpr const auto* const s_szInfoWindowName{ TEXT("foo_logitech_lcd debug info window") };

    //**************************************************************************
    // Globals
    //**************************************************************************
    Windows::UI::WindowClass g_WindowClass{};

    //**************************************************************************
    // Helpers
    //**************************************************************************
    inline void GetFrameStats(float& fFrame, float& fAverage) noexcept {
        static constexpr const int c_nSmoothedCount{ 30 };
        static Windows::StopWatch s_Timer{};
        static float s_fFrameTimeAcc[c_nSmoothedCount]{ 0 };
        static int s_nFrameID{ 0 };

        s_fFrameTimeAcc[s_nFrameID] = fFrame = s_Timer.GetElapsedMilliseconds();
        s_nFrameID = ++s_nFrameID % c_nSmoothedCount;

        fAverage = 0.0f;
        for (auto fTime : s_fFrameTimeAcc) {
            fAverage += fTime;
        }
        fAverage /= static_cast<float>(c_nSmoothedCount);

        s_Timer.Start();
    }

    //------------------------------------------------------

    inline auto String_GetLastError_OpenGL() noexcept {
        switch (glGetError()) {
            case GL_NO_ERROR:                      return TEXT("GL_NO_ERROR");
            case GL_INVALID_ENUM:                  return TEXT("GL_INVALID_ENUM");
            case GL_INVALID_VALUE:                 return TEXT("GL_INVALID_VALUE");
            case GL_INVALID_OPERATION:             return TEXT("GL_INVALID_OPERATION");
            case GL_STACK_OVERFLOW:                return TEXT("GL_STACK_OVERFLOW");
            case GL_STACK_UNDERFLOW:               return TEXT("GL_STACK_UNDERFLOW");
            case GL_OUT_OF_MEMORY:                 return TEXT("GL_OUT_OF_MEMORY");
            case GL_INVALID_FRAMEBUFFER_OPERATION: return TEXT("GL_INVALID_FRAMEBUFFER_OPERATION");
            case GL_CONTEXT_LOST:                  return TEXT("GL_CONTEXT_LOST");
            case GL_TABLE_TOO_LARGE:               return TEXT("GL_TABLE_TOO_LARGE");
            default:                               return TEXT("<UNKNOWN ERROR ID>");
        }
    }

    //**************************************************************************
    // DBGWindow
    //**************************************************************************
    class DBGWindow final {
    public:
        ~DBGWindow() noexcept { Destroy(); }

        DBGWindow()                  = default;
        DBGWindow(const DBGWindow& ) = default;
        DBGWindow(      DBGWindow&&) = default;

        DBGWindow& operator=(const DBGWindow&) = default;
        DBGWindow& operator=(      DBGWindow&&) = default;

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void Destroy() noexcept {
            m_hDC.Detach();
            if (m_InfoFrame) { m_InfoFrame.DestroyWindow(); }
            if (m_InfoFont) { m_InfoFont.DeleteObject(); }
            if (m_Window) { m_Window.DestroyWindow(); }
        }

        LRESULT WindowProc(_In_ HWND hWnd,
                           _In_ UINT Msg,
                           _In_ WPARAM wParam,
                           _In_ LPARAM lParam) noexcept {
            if (Msg == WM_USER_TERMINATE) {
                Destroy();
            }
            return DefWindowProc(hWnd, Msg, wParam, lParam);
        }

        void CreateInfoFrame(_In_ HINSTANCE hInstance) noexcept {
            assert(hInstance);
            auto hWnd = ::CreateWindowEx(WS_EX_NOACTIVATE,
                                         TEXT("Static"),
                                         s_szInfoWindowName,
                                         WS_VISIBLE | WS_CHILDWINDOW | WS_DISABLED,
                                         0, m_CanvasSize.cy + 30,
                                         m_rectClient.right,
                                         m_rectClient.bottom,
                                         m_Window, NULL,
                                         hInstance,
                                         NULL);
            ATLASSERT(hWnd);
            m_InfoFrame.Attach(hWnd);
            ATLASSERT(m_InfoFrame.IsWindow());
            ATLVERIFY(m_InfoFrame.SetWindowText(TEXT("")));

            LOGFONT logFont{};

            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            if (Windows::GDI::GetSystemFont(Windows::GDI::SYS_FONT_MESSAGE,
                                            &logFont)) {
                (*(LPTSTR*)&logFont.lfFaceName) = TEXT("Courier New");
                logFont.lfWeight = FW_BLACK;
                m_InfoFont = ::CreateFontIndirect(&logFont);
                if (m_InfoFont) { m_InfoFrame.SetFont(m_InfoFont, FALSE); }
            }
        }

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        void Create(_In_ SIZE size) noexcept {
            assert(g_WindowClass);

            Destroy();

            m_CanvasSize = size;

            constexpr const DWORD dwStyle = WS_DLGFRAME | WS_VISIBLE;
            constexpr const DWORD dwStyleEx = WS_EX_APPWINDOW | WS_EX_NOACTIVATE;
            RECT rectInitial{ 0, 0, size.cx + 40, size.cy + 250 };
            ::AdjustWindowRectEx(&rectInitial, dwStyle, FALSE, dwStyleEx);

            auto hWnd = ::CreateWindowEx(dwStyleEx,
                                         g_WindowClass.GetClass(),
                                         s_szWindowName,
                                         dwStyle,
                                         CW_USEDEFAULT, CW_USEDEFAULT,
                                         rectInitial.right - rectInitial.left,
                                         rectInitial.bottom - rectInitial.top,
                                         NULL, NULL,
                                         g_WindowClass.GetInstance(),
                                         NULL);
            ATLASSERT(hWnd);
            if (hWnd) {
                ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(this));
            }

            m_Window.Attach(hWnd);
            ATLASSERT(m_Window.IsWindow());
            m_hDC = m_Window.GetDC();
            ATLASSERT(m_hDC);
            if (m_Window && m_hDC) {
                ATLVERIFY(m_Window.GetWindowRect(&m_rectWindow));
                ATLVERIFY(m_Window.GetClientRect(&m_rectClient));
                CreateInfoFrame(g_WindowClass.GetInstance());
            } else {
                Destroy();
            }
        }

        void UpdateInfoFrame() noexcept {
            if (! m_InfoFrame) { return; }
            try {
                CString inf{ TEXT("Frame\r\n") };
                CString formatter{};
                {
                    float fFrame = 0.f, fAverage = 0.f;
                    GetFrameStats(fFrame, fAverage);
                    formatter.Format(TEXT("  Time: %06.2fms (%06.2fms)\r\n"), fFrame, fAverage);
                    inf.Append(formatter);
                    formatter.Format(TEXT("  Rate: %05.2f (%05.2f)\r\n"), 1000.f / fFrame, 1000.f / fAverage);
                    inf.Append(formatter);
                }
                inf.Append(TEXT("Errors\r\n"));
                {
                    if (auto strGetLastError = Windows::FormatMessageFromError()) {
                        inf.Append(TEXT("  Windows: "));
                        inf.Append(strGetLastError);
                    }
                    {
                        inf.Append(TEXT("  errno: "));
                        inf.Append(Windows::GetErrnoString());
                        inf.Append(TEXT("\r\n"));
                    }
                    {
                        inf.Append(TEXT("  OpenGL: "));
                        inf.Append(String_GetLastError_OpenGL());
                        inf.Append(TEXT("\r\n"));
                    }
                }
                ATLVERIFY(m_InfoFrame.SetWindowText(inf));
            } catch (...) {
                ::DebugBreak();
            }
        }

        void Update(_In_ SIZE dim,
                    _In_ BYTE cColorBits,
                    _In_ LPVOID pBits) noexcept {
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            if (m_hDC.IsNull()) { return; }

            using Windows::GDI::BitmapInfo256;

            BitmapInfo256 bmi;
            bmi.bmiHeader.biWidth = dim.cx;
            bmi.bmiHeader.biHeight = -dim.cy;
            bmi.bmiHeader.biCompression = BI_RGB;

            if (cColorBits == 8) {
                bmi.bmiHeader.biBitCount = 8; //< Need 1 byte minimum
                const auto imageSize = dim.cx * dim.cy; //< 1 byte per pixel
                bmi.bmiHeader.biSizeImage = imageSize > 0 ? imageSize : imageSize; //< "height" may be negative
                bmi.bmiHeader.biClrUsed = BitmapInfo256::ColorCount;

                // Values from 0 - 127 are already zero
                for (auto i = (BitmapInfo256::ColorCount / 2); i < BitmapInfo256::ColorCount; ++i) {
                    bmi.bmiColors[i] = RGBQUAD{ 255, 255, 255, 0 };
                }
            } else {
                bmi.bmiHeader.biBitCount = cColorBits;
            }
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            ATLVERIFY(::StretchDIBits(m_hDC,
                                      10, 10, 10 + dim.cx, 10 + dim.cy,
                                      0, 0, dim.cx, dim.cy,
                                      pBits, bmi,
                                      DIB_RGB_COLORS,
                                      SRCCOPY));

            UpdateInfoFrame();
        }

        constexpr explicit operator bool() const noexcept { return m_Window.m_hWnd && m_hDC.m_hDC; }

        CWindow   m_Window    {};
        CDCHandle m_hDC       {};
        CWindow   m_InfoFrame {};
        CFont     m_InfoFont  {};
        SIZE      m_CanvasSize{ 0,0 };
        RECT      m_rectWindow{ 0, 0, 0, 0 };
        RECT      m_rectClient{ 0, 0, 0, 0 };
    } g_DBG; // class DBGWindow final
}

//******************************************************************************
// External Functions
//******************************************************************************
void InitialiseDebug() noexcept {
    if (!g_WindowClass) {
        WNDCLASSEX cls;
        ZeroMemory(&cls, sizeof(WNDCLASSEX));
        cls.cbSize        = sizeof(WNDCLASSEX);
        cls.hInstance     = Windows::GetInstanceHandle();
        cls.lpszClassName = s_szClassName;
        cls.lpfnWndProc   = &Windows::UI::WindowProcHelperT<DBGWindow>::WindowProc;
        cls.style         = CS_OWNDC | CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW;
        cls.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH); //< Note: Stock Objects do **not** need deleted/released
        ATLASSERT(cls.hbrBackground);
        g_WindowClass = Windows::UI::WindowClass{ cls };
        ATLASSERT(g_WindowClass);
    }
}

//----------------------------------------------------------

void InitialiseDebugCanvas(_In_ SIZE size) noexcept {
    g_DBG.Create(size);
}

//----------------------------------------------------------

void UninitialiseDebugCanvas() noexcept {
    g_DBG.Destroy();
}

//----------------------------------------------------------

void UpdateDebugCanvas(_In_ SIZE dim,
                       _In_ BYTE cColorBits,
                       _In_ LPVOID pBits) noexcept {
    if (g_DBG) {
        g_DBG.Update(dim, cColorBits, pBits);
    }
}

#endif // #if defined(DEBUG) || defined(_DEBUG) || defined(foo_logitech_lcd_ENABLE_DEBUG_CANVAS)
