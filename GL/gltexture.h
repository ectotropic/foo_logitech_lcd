#pragma once
#ifndef GUID_E3505989_337A_44D9_8CFC_F121D6584368
#define GUID_E3505989_337A_44D9_8CFC_F121D6584368
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
#include "GL/glscopedutil.h"
//--------------------------------------

namespace OpenGL {
    //**************************************************************************
    // glScopedBindTexture
    //**************************************************************************
    class glScopedBindTexture final {
    public:
        glScopedBindTexture(::GLenum target,
                            ::GLuint name) noexcept :
            m_Target{ target } {
            ::glBindTexture(m_Target, name);
            OpenGLAssertNoError();
        }

        ~glScopedBindTexture() noexcept {
            ::glBindTexture(m_Target, 0);
            OpenGLAssertNoError();
        }

        glScopedBindTexture(const glScopedBindTexture& ) = delete; // No Copy
        glScopedBindTexture(      glScopedBindTexture&&) = delete; // No Move
        glScopedBindTexture& operator=(const glScopedBindTexture& ) = delete; // No Copy
        glScopedBindTexture& operator=(      glScopedBindTexture&&) = delete; // No Move

    private:
        const GLenum m_Target{ 0 };
    }; // class glScopedBindTexture final

    //**************************************************************************
    // glTexture
    //**************************************************************************
    class glTexture final {
    public:
        using handle_type = ::GLuint;
        using target_type = ::GLenum;
        using extent_type = ::GLsizei;
        using size_type   = ::GLsizei;

    public:
        static void create(::GLsizei n,
                           ::GLuint* names) noexcept {
            ::glGenTextures(n, names);
            OpenGLAssertNoError();
        }

        static void create(::GLuint& name) noexcept {
            glTexture::create(1, &name);
        }

        static void destroy(::GLsizei n,
                            const ::GLuint* names) noexcept {
            ::glDeleteTextures(n, names);
            OpenGLAssertNoError();
        }

        static void destroy(::GLuint name) noexcept {
            glTexture::destroy(1, &name);
        }

        static void bind(::GLenum target,
                         ::GLuint buffer) noexcept {
            ::glBindTexture(target, buffer);
            OpenGLAssertNoError();
        }

        static void unbind(::GLenum target) noexcept {
            glTexture::bind(target, 0);
            OpenGLAssertNoError();
        }

        static void image(::GLenum  target,
                          ::GLint   level,
                          ::GLint   internalFormat,
                          ::GLsizei width,
                          ::GLint   border,
                          ::GLint   dataFormat,
                          ::GLenum  dataType,
                          const ::GLvoid* pixels) noexcept {
            ::glTexImage1D(target, level, internalFormat,
                           width, border,
                           dataFormat, dataType, pixels);
            OpenGLAssertNoError();
        }

        static void image(::GLenum  target,
                          ::GLint   level,
                          ::GLint   internalFormat,
                          ::GLsizei width,
                          ::GLsizei height,
                          ::GLint   border,
                          ::GLint   dataFormat,
                          ::GLenum  dataType,
                          const ::GLvoid* pixels) noexcept {
            ::glTexImage2D(target, level, internalFormat,
                           width, height, border,
                           dataFormat, dataType, pixels);
            OpenGLAssertNoError();
        }

        static void image(::GLenum  target,
                          ::GLint   level,
                          ::GLint   internalFormat,
                          ::GLsizei width,
                          ::GLsizei height,
                          ::GLsizei depth,
                          ::GLint   border,
                          ::GLint   dataFormat,
                          ::GLenum  dataType,
                          const ::GLvoid* pixels) noexcept {
            ::glTexImage3D(target, level, internalFormat,
                           width, height, depth, border,
                           dataFormat, dataType, pixels);
            OpenGLAssertNoError();
        }

        static void parameter(::GLenum target,
                              ::GLenum pname,
                              ::GLfloat value) noexcept {
            ::glTexParameterf(target, pname, value);
            OpenGLAssertNoError();
        }

        static void parameter(::GLenum target,
                              ::GLenum pname,
                              const ::GLfloat* values) noexcept {
            ::glTexParameterfv(target, pname, values);
            OpenGLAssertNoError();
        }

        static void parameter(::GLenum target,
                              ::GLenum pname,
                              ::GLint value) noexcept {
            ::glTexParameteri(target, pname, value);
            OpenGLAssertNoError();
        }

        static void parameter(::GLenum target,
                              ::GLenum pname,
                              const ::GLint* values) noexcept {
            ::glTexParameteriv(target, pname, values);
            OpenGLAssertNoError();
        }

    private:
        glTexture(const glTexture&)            = delete; // No Copy
        glTexture& operator=(const glTexture&) = delete; // No Copy

    public:
        glTexture() = default;

        constexpr glTexture(::GLuint texture) noexcept :
            m_Texture{ texture } {}

        glTexture(glTexture&& other) noexcept :
            m_Texture{ std::exchange(other.m_Texture, 0) } {}

        glTexture& operator=(glTexture&& other) noexcept {
            destroy();
            using std::swap;
            swap(m_Texture, other.m_Texture);
            return *this;
        }

        ~glTexture() noexcept { destroy(); }

    public:
        void bind(GLenum target) const noexcept {
            glTexture::bind(target, m_Texture);
        }

        [[nodiscard]]
        auto scoped_bind(GLenum target) const noexcept {
            return glScopedBindTexture{ target, m_Texture };
        }

        void create() noexcept {
            destroy();
            glTexture::create(m_Texture);
            OpenGLAssert(m_Texture);
        }

        void create(::GLenum  target,
                    ::GLint   internalFormat,
                    ::GLsizei width,
                    ::GLint   border,
                    ::GLint   dataFormat,
                    ::GLenum  dataType,
                    const ::GLvoid* pixels) noexcept {
            create();
            auto attrib_{ ::OpenGL::glScopedPushAttrib{GL_TEXTURE_BIT} };
            auto bind_{ scoped_bind(target) };
            glTexture::parameter(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
            glTexture::image(target, 0, internalFormat,
                             width, border,
                             dataFormat, dataType, pixels);
        }

        void create(::GLenum  target,
                    ::GLint   internalFormat,
                    ::GLsizei width,
                    ::GLsizei height,
                    ::GLint   border,
                    ::GLint   dataFormat,
                    ::GLenum  dataType,
                    const ::GLvoid* pixels) noexcept {
            create();
            auto attrib_{ ::OpenGL::glScopedPushAttrib{GL_TEXTURE_BIT} };
            auto bind_{ scoped_bind(target) };
            glTexture::parameter(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
            glTexture::parameter(target, GL_TEXTURE_WRAP_T,     GL_CLAMP);
            glTexture::image(target, 0, internalFormat,
                             width, height, border,
                             dataFormat, dataType, pixels);
        }

        void create(::GLenum  target,
                    ::GLint   internalFormat,
                    ::GLsizei width,
                    ::GLsizei height,
                    ::GLsizei depth,
                    ::GLint   border,
                    ::GLint   dataFormat,
                    ::GLenum  dataType,
                    const ::GLvoid* pixels) noexcept {
            create();
            auto bind_{ scoped_bind(target) };
            glTexture::parameter(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexture::parameter(target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
            glTexture::parameter(target, GL_TEXTURE_WRAP_T,     GL_CLAMP);
            glTexture::parameter(target, GL_TEXTURE_WRAP_R,     GL_CLAMP);
            glTexture::image(target, 0, internalFormat,
                             width, height, depth, border,
                             dataFormat, dataType, pixels);
        }

        void destroy() noexcept {
            if (m_Texture) {
                glTexture::destroy(std::exchange(m_Texture, 0));
            }
        }

        constexpr auto name() const noexcept {
            return m_Texture;
        }

    public:
        constexpr operator const GLuint () const noexcept { return m_Texture; }
        constexpr operator const GLuint*() const noexcept { return &m_Texture; }

        explicit constexpr operator bool() const noexcept { return m_Texture != 0; }

    private:
        ::GLuint m_Texture{ 0 };
    }; // class glTexture final
} // namespace OpenGL

#endif // GUID_E3505989_337A_44D9_8CFC_F121D6584368
