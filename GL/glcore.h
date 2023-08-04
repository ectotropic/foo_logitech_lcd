#pragma once
#ifndef GUID_EAE93F16_B513_49FB_840A_F3F6094B3478
#define GUID_EAE93F16_B513_49FB_840A_F3F6094B3478
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
#ifndef GLEW_STATIC
#   define GLEW_STATIC
#endif
#include <gl/GLew.h>
//--------------------------------------

namespace OpenGL::detail {
    //**************************************************************************
    // glGetString
    //**************************************************************************
    template <GLenum NameT>
    inline decltype(auto) glGetString() noexcept {
        return reinterpret_cast<const char* const>(::glGetString(NameT));
    }

    //----------------------------------

    template <GLenum NameT>
    inline decltype(auto) glGetString(GLuint index) noexcept {
        return reinterpret_cast<const char* const>(::glGetStringi(NameT, index));
    }
} // namespace OpenGL::detail

//==============================================================================

namespace OpenGL {
    //**************************************************************************
    // glGet...String
    //**************************************************************************
    inline decltype(auto) glGetVersionString() noexcept {
        return ::OpenGL::detail::glGetString<GL_VERSION>();
    }

    //----------------------------------

    inline decltype(auto) glGetVendorString() noexcept {
        return ::OpenGL::detail::glGetString<GL_VENDOR>();
    }

    //----------------------------------

    inline decltype(auto) glGetRendererString() noexcept {
        return ::OpenGL::detail::glGetString<GL_RENDERER>();
    }

    //----------------------------------

    inline decltype(auto) glGetExtensionString(GLuint index) noexcept {
        return ::OpenGL::detail::glGetString<GL_EXTENSIONS>(index);
    }

    //----------------------------------

#ifdef GL_SHADING_LANGUAGE_VERSION
    inline decltype(auto) glGetGLSLVersion() noexcept {
        return ::OpenGL::detail::glGetString<GL_SHADING_LANGUAGE_VERSION>();
    }
#endif
} // namespace OpenGL

#endif // GUID_EAE93F16_B513_49FB_840A_F3F6094B3478
