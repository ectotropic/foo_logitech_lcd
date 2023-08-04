#pragma once
#ifndef GUID_3B563720_202A_4ABC_A1F1_8BD320265328
#define GUID_3B563720_202A_4ABC_A1F1_8BD320265328
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
#include "Windows/Windows_Core.h"
#include "Windows/GDI/GDI.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

//--------------------------------------
//
#ifndef WGLAssert
#   define WGLAssert                    WinAPIAssert
#endif
#ifndef WGLVerify
#   define WGLVerify                    WinAPIVerify
#endif
#ifndef WinGLAssertErrorSuccess
#   define WGLAssertErrorSuccess        WinAPIAssertErrorSuccess
#endif
//--------------------------------------

namespace OpenGL::WGL {
    //**************************************************************************
    // RenderContext
    //**************************************************************************
    class RenderContext final {
    private:
        template <typename TypeT>
        static [[nodiscard]]
        auto exchange_null(TypeT& val) noexcept
            -> decltype(std::exchange(val, static_cast<TypeT>(NULL))) {
            return std::exchange(val, static_cast<TypeT>(NULL));
        }

    public:
        static [[nodiscard]]
        auto Create(_In_ HDC dc) noexcept
            -> decltype(::wglCreateContext(dc)) {
            const auto rc{ ::wglCreateContext(dc) };
            WGLAssert(rc);
            return rc;
        }

        static [[nodiscard]]
        auto Create(_In_ HDC dc,
                    _In_ int layer) noexcept
            -> decltype(::wglCreateLayerContext(dc, layer)) {
            const auto rc{ ::wglCreateLayerContext(dc, layer) };
            WGLAssert(rc);
            return rc;
        }

        static auto CopyState(_In_ HGLRC srcRC,
                              _In_ HGLRC dstRC,
                              _In_ UINT bits = GL_ALL_ATTRIB_BITS) noexcept {
            const auto res{ ::wglCopyContext(srcRC, dstRC, bits) };
            WGLAssert(res);
            return res != FALSE;
        }

        static auto Delete(_In_ HGLRC rc) noexcept {
            const auto res{ ::wglDeleteContext(rc) };
            WGLAssert(res);
            return res != FALSE;
        }

        static auto MakeCurrent(_In_opt_ HDC dc,
                                _In_opt_ HGLRC rc) noexcept {
            const auto res{ ::wglMakeCurrent(dc, rc) };
            WGLAssert(res);
            return res != FALSE;
        }

        static auto Current() noexcept
            -> decltype(::wglGetCurrentContext()) {
            return ::wglGetCurrentContext();
        }

        static auto CurrentDC() noexcept
            -> decltype(::wglGetCurrentDC()) {
            return ::wglGetCurrentDC();
        }

    private:
        RenderContext(const RenderContext&)            = delete; // No Copy
        RenderContext& operator=(const RenderContext&) = delete; // No Copy

    public:
        RenderContext() = default;

        explicit RenderContext(_In_ HGLRC hGLRC) noexcept :
            m_hGLRC{ hGLRC } {}

        explicit RenderContext(_In_ HDC hDC) noexcept :
            m_hGLRC{ Create(hDC) } {}

        RenderContext(_In_ HDC hDC,
                      _In_ int layer) noexcept :
            m_hGLRC{ Create(hDC, layer) } {}

        RenderContext(RenderContext&& other) noexcept :
            m_hGLRC{ exchange_null(other.m_hGLRC) } {}


        RenderContext& operator=(RenderContext&& other) noexcept {
            Destroy();
            m_hGLRC = exchange_null(other.m_hGLRC);
            return *this;
        }

        ~RenderContext() noexcept { Destroy(); }

    public:
        auto MakeCurrent(HDC dc) const noexcept {
            return m_hGLRC && MakeCurrent(dc, m_hGLRC);
        }

        auto MakeNotCurrent(HDC dc) const noexcept {
            return m_hGLRC && MakeCurrent(dc, NULL);
        }

        bool Delete() noexcept {
            if (!m_hGLRC) { return false; }
            return Delete(exchange_null(m_hGLRC));
        }

        bool Destroy(HDC dc) noexcept {
            if (!m_hGLRC) { return false; }
            if (IsCurrent()) {
                assert(dc);
                const auto res = MakeNotCurrent(dc);
                return Delete() && res;
            } else {
                return Delete();
            }
        }

        bool Destroy() noexcept { return Destroy(CurrentDC()); }

        bool IsCurrent() const noexcept { return m_hGLRC && (m_hGLRC == Current()); }

        constexpr HGLRC GetRenderContext() const { return m_hGLRC; }

    public:
        constexpr operator HGLRC() const noexcept { return m_hGLRC; }

        explicit constexpr operator bool() const noexcept { return m_hGLRC; }

    private:
        HGLRC m_hGLRC{ NULL };
    }; // class RenderContext final
} // namespace OpenGL::WGL

#endif // GUID_3B563720_202A_4ABC_A1F1_8BD320265328