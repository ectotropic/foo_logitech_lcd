#pragma once
#ifndef GUID_0DB9801E_1FE0_4A54_ADEA_142148EC3501
#define GUID_0DB9801E_1FE0_4A54_ADEA_142148EC3501
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
#include "GL/glcommon.h"
//--------------------------------------

namespace OpenGL {
    //**************************************************************************
    // glGet: helpers for getting currently set values
    //**************************************************************************
    inline void glGet(GLenum pname,
                      GLboolean* params) noexcept {
        ::glGetBooleanv(pname, params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLboolean& params) noexcept {
        ::glGetBooleanv(pname, &params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLdouble* params) noexcept {
        ::glGetDoublev(pname, params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLdouble& params) noexcept {
        ::glGetDoublev(pname, &params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLfloat* params) noexcept {
        ::glGetFloatv(pname, params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLfloat& params) noexcept {
        ::glGetFloatv(pname, &params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLint* params) noexcept {
        ::glGetIntegerv(pname, params);
    }

    //----------------------------------

    inline void glGet(GLenum pname,
                      GLint& params) noexcept {
        ::glGetIntegerv(pname, &params);
    }

    //----------------------------------

    template <typename GLTypeT>
    inline auto glGet(GLenum pname) noexcept {
        GLTypeT value{ 0 };
        ::OpenGL::glGet(pname, value);
        return value;
    }
} // namespace OpenGL

#endif // GUID_0DB9801E_1FE0_4A54_ADEA_142148EC3501
