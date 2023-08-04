#pragma once
#ifndef GUID_AE35ECEF_1A75_4E3D_BE8C_501EB2C09B81
#define GUID_AE35ECEF_1A75_4E3D_BE8C_501EB2C09B81
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

namespace OpenGL::detail {
    //**************************************************************************
    // glScopedBindBufferT
    //**************************************************************************
    template <GLenum TargetT>
    struct glScopedBindBufferT final {
    public:
        inline static constexpr const auto target{ TargetT };

    public:
        glScopedBindBufferT(GLuint buffer) noexcept {
            ::glBindBuffer(TargetT, buffer);
            OpenGLAssertNoError();
        }
        ~glScopedBindBufferT() noexcept {
            ::glBindBuffer(TargetT, 0);
            OpenGLAssertNoError();
        }

        glScopedBindBufferT(const glScopedBindBufferT& ) = delete; // No Copy
        glScopedBindBufferT(      glScopedBindBufferT&&) = delete; // No Move
        glScopedBindBufferT& operator=(const glScopedBindBufferT& ) = delete; // No Copy
        glScopedBindBufferT& operator=(      glScopedBindBufferT&&) = delete; // No Move
    }; // template<...> struct glScopedBindBufferT final

    //**************************************************************************
    // glScopedBindBufferARBT
    //**************************************************************************
    template <GLenum TargetT>
    struct glScopedBindBufferARBT final {
    public:
        inline static constexpr const auto target{ TargetT };

    public:
        glScopedBindBufferARBT(GLuint buffer) noexcept {
            ::glBindBufferARB(TargetT, buffer);
            OpenGLAssertNoError();
        }
        ~glScopedBindBufferARBT() noexcept {
            ::glBindBufferARB(TargetT, 0);
            OpenGLAssertNoError();
        }

        glScopedBindBufferARBT(const glScopedBindBufferARBT& ) = delete; // No Copy
        glScopedBindBufferARBT(      glScopedBindBufferARBT&&) = delete; // No Move
        glScopedBindBufferARBT& operator=(const glScopedBindBufferARBT& ) = delete; // No Copy
        glScopedBindBufferARBT& operator=(      glScopedBindBufferARBT&&) = delete; // No Move
    }; // template<...> struct glScopedBindBufferARBT final

    //**************************************************************************
    // glScopedMapBufferARBT
    //**************************************************************************
    template <GLenum TargetT>
    struct glScopedMapBufferARBT final {
    public:
        inline static constexpr const auto target{ TargetT };

    public:
        template <typename TypeT>
        glScopedMapBufferARBT(TypeT*& pData,
                              GLenum access) noexcept {
            pData = static_cast<TypeT*>(::glMapBufferARB(TargetT, access));
            OpenGLAssertNoError();
        }

        ~glScopedMapBufferARBT() noexcept {
            ::glUnmapBufferARB(TargetT);
            OpenGLAssertNoError();
        }

        glScopedMapBufferARBT(const glScopedMapBufferARBT& ) = delete; // No Copy
        glScopedMapBufferARBT(      glScopedMapBufferARBT&&) = delete; // No Move
        glScopedMapBufferARBT& operator=(const glScopedMapBufferARBT& ) = delete; // No Copy
        glScopedMapBufferARBT& operator=(      glScopedMapBufferARBT&&) = delete; // No Move
    }; // template<...> struct glScopedMapBufferARBT final

    //**************************************************************************
    // glBufferObjectT
    //**************************************************************************
    template <GLenum TargetT>
    class glBufferObjectT final {
    private:
        using this_class = glBufferObjectT<TargetT>;

    public:
        inline static constexpr const auto target{ TargetT };

        using scoped_bind_type = glScopedBindBufferARBT<TargetT>;
        using scoped_map_type  = glScopedMapBufferARBT<TargetT>;

    public:
        static void create(GLsizei n, GLuint* buffers) noexcept {
            ::glGenBuffersARB(n, buffers);
            OpenGLAssertNoError();
        }

        static void create(GLuint& buffer) noexcept {
            this_class::create(1, &buffer);
        }

        static void destroy(GLsizei n, const GLuint* buffers) noexcept {
            ::glDeleteBuffersARB(n, buffers);
            OpenGLAssertNoError();
        }

        static void destroy(GLuint buffer) noexcept {
            this_class::destroy(1, &buffer);
        }

        static void bind(GLuint buffer) noexcept {
            ::glBindBufferARB(target, buffer);
            OpenGLAssertNoError();
        }

        static void unbind() noexcept {
            ::glBindBufferARB(target, 0);
            OpenGLAssertNoError();
        }

        template <typename TypeT>
        static void data(GLsizeiptrARB size,
                         const TypeT* data,
                         GLenum usage) noexcept {
            ::glBufferDataARB(target, size,
                              data, usage);
            OpenGLAssertNoError();
        }

        static void* map(GLenum access) noexcept {
            auto* data = ::glMapBufferARB(target, access);
            OpenGLAssertNoError();
            return data;
        }

        template <typename TypeT>
        static void map(TypeT*& data, GLenum access) noexcept {
            data = this_class::map(access);
        }

        static void unmap() noexcept {
            ::glUnmapBufferARB(target);
            OpenGLAssertNoError();
        }

    public:
        glBufferObjectT() noexcept = default;

        glBufferObjectT(const glBufferObjectT&) = delete; // No Copy

        glBufferObjectT(glBufferObjectT&& other) noexcept :
            m_Buffer{ std::exchange(other.m_Buffer, 0) } {}

        glBufferObjectT& operator=(const glBufferObjectT&)  = delete; // No Copy

        glBufferObjectT& operator=(glBufferObjectT&& other) noexcept {
            destroy();
            using std::swap;
            swap(m_Buffer, other.m_Buffer);
            return *this;
        }

        ~glBufferObjectT() noexcept { destroy(); }

    public:
        void create() noexcept {
            destroy();
            create(m_Buffer);
            OpenGLAssert(m_Buffer);
        }

        template <typename TypeT>
        void create(GLsizeiptrARB size,
                    const TypeT* contents,
                    GLenum usage) noexcept {
            create();
            if (m_Buffer) {
                bind();
                data(size, contents, usage);
                unbind();
            }
        }

        void create(GLsizeiptrARB size,
                    GLenum usage) noexcept {
            create<void>(size, nullptr, usage);
        }

        void destroy() noexcept {
            if (m_Buffer) {
                destroy(std::exchange(m_Buffer, 0));
            }
        }

        void bind() const noexcept { bind(m_Buffer); }

        auto scoped_bind() const noexcept { return scoped_bind_type{ m_Buffer }; }
        auto scoped_map () const noexcept { return scoped_map_type { m_Buffer }; }

        explicit constexpr operator bool() const noexcept { return m_Buffer != 0; }

        constexpr operator const GLuint () const noexcept { return m_Buffer; }
        constexpr operator const GLuint*() const noexcept { return &m_Buffer; }

    private:
        GLuint m_Buffer{ 0 };
    }; // template<...> struct glBufferObjectT final
} // namespace OpenGL::detail

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OpenGL {
    using glPixelPackBuffer =
        ::OpenGL::detail::glBufferObjectT<GL_PIXEL_PACK_BUFFER_ARB>;

    //----------------------------------

    using glScopedBindPixelPackBuffer =
        ::OpenGL::detail::glScopedBindBufferT<GL_PIXEL_PACK_BUFFER_ARB>;

    //----------------------------------

    using glScopedBindPixelPackBufferARB =
        ::OpenGL::detail::glScopedBindBufferARBT<GL_PIXEL_PACK_BUFFER_ARB>;

    //----------------------------------

    using glScopedMapPixelPackBuffer =
        ::OpenGL::detail::glScopedMapBufferARBT<GL_PIXEL_PACK_BUFFER_ARB>;
} // namespace OpenGL

#endif // GUID_AE35ECEF_1A75_4E3D_BE8C_501EB2C09B81
