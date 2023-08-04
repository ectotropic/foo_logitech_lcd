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
#include "Image_OpenGL.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glscopedutil.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // Helper Types
    //**************************************************************************
    using texture_t = ::OpenGL::glTexture;

    //**************************************************************************
    // Helpers
    //**************************************************************************
    inline constexpr bool IsPowerOfTwo(GLsizei val) noexcept {
        return val != 0 && (val & (val - 1)) == 0;
    }

    //--------------------------------------------------------------------------
    inline void DrawOpenGLImage(GLenum target,
                                const texture_t& texture,
                                GLint x0, GLint y0,
                                GLint x1, GLint y1,
                                GLfloat s, GLfloat t) noexcept {
        texture.parameter(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texture.parameter(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        texture.parameter(target, GL_TEXTURE_WRAP_S,     GL_CLAMP);
        texture.parameter(target, GL_TEXTURE_WRAP_T,     GL_CLAMP);
        {
            const OpenGL::glScopedBindTexture bind_ { texture.scoped_bind(target) };
            const OpenGL::glScopedBegin       begin_{ GL_QUADS };
            ::glTexCoord2f(0, s); ::glVertex2i(x0, y0);
            ::glTexCoord2f(t, s); ::glVertex2i(x1, y0);
            ::glTexCoord2f(t, 0); ::glVertex2i(x1, y1);
            ::glTexCoord2f(0, 0); ::glVertex2i(x0, y1);
        }
    }
} // namespace <anonymous>

//==============================================================================

namespace Image {
    //**************************************************************************
    // OpenGLImage <static>
    //**************************************************************************
    OpenGLImage::pointer OpenGLImage::NewImage(::GLsizei width,
                                               ::GLsizei height,
                                               ::GLenum dataFormat,
                                               const ::GLvoid* data) noexcept {
        const auto bIsPowerOfTwo{ ::IsPowerOfTwo(width) && ::IsPowerOfTwo(height) };

        GLenum target = 0;
        if (GLEW_ARB_texture_non_power_of_two || bIsPowerOfTwo) {
            target = GL_TEXTURE_2D;
        } else if (GLEW_ARB_texture_rectangle) {
            target = GL_TEXTURE_RECTANGLE;
        }

        assert(target != 0); if (target == 0) { return {}; }

        const ::OpenGL::glScopedPushAttrib glAttrib{
            GL_ENABLE_BIT | GL_TEXTURE_BIT
        };

        ::glEnable(target);
        texture_type texture{};
        texture.create(/*target*/         target,
                       /*internalFormat*/ GL_RGBA,
                       /*width*/          width,
                       /*height*/         height,
                       /*border*/         0,
                       /*format*/         dataFormat,
                       /*type*/           GL_UNSIGNED_BYTE,
                       /*pixels*/         data);

        if (texture) {
            const auto coordS{
                target == GL_TEXTURE_RECTANGLE
                    ? static_cast<GLfloat>(width)
                    : 1.f
            };

            const auto coordT{
                target == GL_TEXTURE_RECTANGLE
                    ? static_cast<GLfloat>(height)
                    : 1.f
            };

            try {
                return pointer{
                    new OpenGLImage{
                        width, height,
                        coordS, coordT,
                        target,
                        std::move(texture)
                    }
                };
            } catch (...) {
                assert(false);
            }
        }

        return {};
    }

    //--------------------------------------------------------------------------

    OpenGLImage::pointer OpenGLImage::NewImage(::GLsizei width,
                                               ::GLsizei height,
                                               const ::GLvoid* data) noexcept {
        return NewImage(width, height, GL_BGRA, data);
    }

    //--------------------------------------------------------------------------

    OpenGLImage::pointer OpenGLImage::NewImage(const data_type& data) noexcept {
        if (!data) { return {}; }
        return NewImage(data.GetWidth(), data.GetHeight(),
                        GL_BGRA,
                        data.GetPixels());
    }

    //**************************************************************************
    // OpenGLImage
    //**************************************************************************
    void OpenGLImage::Destroy() noexcept {
        if (m_Texture) { m_Texture.destroy(); }
        m_Target = 0;
        m_CoordS = m_CoordT = 1.f;
        base_type::Destroy();
    }

    //--------------------------------------------------------------------------

    void OpenGLImage::Draw(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1) const noexcept {
        const ::OpenGL::glScopedPushAttrib glAttrib{
            GL_COLOR_BUFFER_BIT |
                GL_ENABLE_BIT   |
                GL_TEXTURE_BIT
        };

        ::glEnable(m_Target);
        ::glDisable(GL_BLEND);
        ::glColor4f(1.f, 1.f, 1.f, 1.f);
        ::DrawOpenGLImage(m_Target, m_Texture,
                          x0      , y0,
                          x1      , y1,
                          m_CoordS, m_CoordT);
    }

    //--------------------------------------------------------------------------

    void OpenGLImage::Blend(coord_type x0, coord_type y0,
                            coord_type x1, coord_type y1,
                            float a) const noexcept {
        if (a <= 0) { return; }
        const ::OpenGL::glScopedPushAttrib glAttrib{
            GL_COLOR_BUFFER_BIT |
                GL_ENABLE_BIT   |
                GL_TEXTURE_BIT
        };

        ::glEnable(m_Target);
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glColor4f(1.f, 1.f, 1.f, a);
        ::DrawOpenGLImage(m_Target, m_Texture,
                          x0      , y0,
                          x1      , y1,
                          m_CoordS, m_CoordT);
    }
} // namespace Image
