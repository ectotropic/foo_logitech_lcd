#pragma once
#ifndef GUID_0E26C781_C56E_41AF_A47F_DD14D907BFCC
#define GUID_0E26C781_C56E_41AF_A47F_DD14D907BFCC
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
#include "GL/glget.h"
//--------------------------------------

namespace OpenGL::detail {
    //**************************************************************************
    // glGetNameT
    //**************************************************************************
    template <GLenum NameT, typename GLTypeT>
    struct glGetNameT {
    public:
        using value_type = GLTypeT;

    public:
        static decltype(auto) get() noexcept {
            return ::OpenGL::glGet<value_type>(NameT);
        };
    }; // template <...> struct glGetNameT

    //**************************************************************************
    // glScopedSetT
    //**************************************************************************
    template <class AccessorT>
    class glScopedSetT final : public AccessorT {
    public:
        using accessor_type = AccessorT;
        using value_type    = typename accessor_type::value_type;

    public:
        template <typename... ArgPackT>
        glScopedSetT(ArgPackT&& ...args) noexcept :
            m_Param{ accessor_type::get() } {
            accessor_type::set(std::forward<ArgPackT>(args)...);
        }

        ~glScopedSetT() noexcept {
            accessor_type::set(m_Param);
        }

        glScopedSetT(const glScopedSetT& ) = delete; // No Copy
        glScopedSetT(      glScopedSetT&&) = delete; // No Move
        glScopedSetT& operator=(const glScopedSetT& ) = delete; // No Copy
        glScopedSetT& operator=(      glScopedSetT&&) = delete; // No Move

    private:
        const value_type m_Param{};
    }; // template <...> struct glScopedSetT final
} // namespace OpenGL::detail

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OpenGL {
    //**************************************************************************
    // glPointSizeAccessor
    //**************************************************************************
    struct glPointSizeAccessor :
        public ::OpenGL::detail::glGetNameT<GL_POINT_SIZE, ::GLfloat> {

        static void set(GLfloat param) noexcept {
            ::glPointSize(param);
        }
    }; // struct glPointSizeAccessor

    using glScopedSetPointSize =
        ::OpenGL::detail::glScopedSetT<::OpenGL::glPointSizeAccessor>;

    //**************************************************************************
    // glLineWidthAccessor
    //**************************************************************************
    struct glLineWidthAccessor :
        public ::OpenGL::detail::glGetNameT<GL_LINE_WIDTH, ::GLfloat> {

        static void set(GLfloat param) noexcept {
            ::glLineWidth(param);
        }
    }; // struct glLineWidthAccessor

    using glScopedSetLineWidth =
        ::OpenGL::detail::glScopedSetT<::OpenGL::glLineWidthAccessor>;

    //**************************************************************************
    // glScopedBegin
    //**************************************************************************
    struct glScopedBegin final {
    public:
        glScopedBegin(GLenum mode) noexcept {
            OpenGLAssertNoError();
            ::glBegin(mode);
            // NOTE:
            //  - It is an error to call `glGetError` after
            //    `glBegin` but before `glEnd`, but the
            //    error this generates will appear _AFTER_
            //    `glEnd`.
        }

        ~glScopedBegin() noexcept {
            ::glEnd();
            OpenGLAssertNoError();
        }

        glScopedBegin(const glScopedBegin& ) = delete; // No Copy
        glScopedBegin(      glScopedBegin&&) = delete; // No Move
        glScopedBegin& operator=(const glScopedBegin& ) = delete; // No Copy
        glScopedBegin& operator=(      glScopedBegin&&) = delete; // No Move
    }; // struct glScopedBegin final

    //**************************************************************************
    // glScopedPushAttrib
    //**************************************************************************
    struct glScopedPushAttrib final {
    public:
        glScopedPushAttrib(::GLbitfield mask = GL_ALL_ATTRIB_BITS) noexcept {
            ::glPushAttrib(mask);
            OpenGLAssertNoError();
        }

        ~glScopedPushAttrib() noexcept {
            ::glPopAttrib();
            OpenGLAssertNoError();
        }

        glScopedPushAttrib(const glScopedPushAttrib& ) = delete; // No Copy
        glScopedPushAttrib(      glScopedPushAttrib&&) = delete; // No Move
        glScopedPushAttrib& operator=(const glScopedPushAttrib& ) = delete; // No Copy
        glScopedPushAttrib& operator=(      glScopedPushAttrib&&) = delete; // No Move
    }; // struct glScopedPushAttrib final
} // namespace OpenGL

#endif // GUID_0E26C781_C56E_41AF_A47F_DD14D907BFCC
