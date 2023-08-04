#pragma once
#ifndef GUID_5802D78E_2F03_4857_8BD8_3C91BACEDEA8
#define GUID_5802D78E_2F03_4857_8BD8_3C91BACEDEA8
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
#include "Image.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
#include "GL/gltexture.h"
//--------------------------------------

namespace Image {
    //**************************************************************************
    // OpenGLImage
    //**************************************************************************
    class OpenGLImage final : public ::Image::Image {
    private:
        using base_type = ::Image::Image;
        using this_type = ::Image::OpenGLImage;

    public:
        using pointer              = managed_pointer_type<this_type>;
        using native_extent_type   = ::GLsizei;
        using native_coord_type    = ::GLint;
        using native_texcoord_type = ::GLint;
        using texture_type         = ::OpenGL::glTexture;

    public:
        static pointer NewImage(::GLsizei width,
                                ::GLsizei height,
                                ::GLenum dataFormat,
                                const ::GLvoid* data) noexcept;

        static pointer NewImage(::GLsizei width,
                                ::GLsizei height,
                                const ::GLvoid* data) noexcept;

        static pointer NewImage(const data_type& data) noexcept;

    private:
        OpenGLImage(::GLsizei width,
                    ::GLsizei height,
                    ::GLfloat coordS,
                    ::GLfloat coordT,
                    ::GLenum target,
                    texture_type&& texture) noexcept :
            base_type{ width, height },
            m_CoordS { coordS },
            m_CoordT { coordT },
            m_Target { target  },
            m_Texture{ std::forward<texture_type>(texture) } {}

        OpenGLImage(const OpenGLImage&)            = delete; // No Copy
        OpenGLImage& operator=(const OpenGLImage&) = delete; // No Copy

    public:
        OpenGLImage() = default;

        OpenGLImage(OpenGLImage&& other) noexcept :
            base_type{ std::forward<OpenGLImage>(other) },
            m_CoordS { std::exchange(other.m_CoordS, 0.f) },
            m_CoordT { std::exchange(other.m_CoordT, 0.f) },
            m_Target { std::exchange(other.m_Target, 0) },
            m_Texture{ std::forward<texture_type>(other.m_Texture) } {}

        OpenGLImage& operator=(OpenGLImage&& other) noexcept {
            Destroy();
            base_type::operator=(std::forward<OpenGLImage>(other));
            m_CoordS  = std::exchange(other.m_CoordS, 0.f);
            m_CoordT  = std::exchange(other.m_CoordT, 0.f);
            m_Target  = std::exchange(other.m_Target, 0);
            m_Texture = std::forward<texture_type>(other.m_Texture);
            return *this;
        }

    public: // Overrides
        virtual void Destroy() noexcept;

        virtual void Draw(coord_type x0, coord_type y0,
                          coord_type x1, coord_type y1) const noexcept override;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           float alpha = 1.f) const noexcept override;

        virtual void Blend(coord_type x0, coord_type y0,
                           coord_type x1, coord_type y1,
                           byte_type alpha) const noexcept override {
            Blend(x0, y0, x1, y1, static_cast<GLfloat>(alpha) / 255.f);
        }

        virtual bool Valid() const noexcept override {
            return base_type::Valid() && m_Texture;
        }

    private:
        ::GLfloat    m_CoordS { 1.f };
        ::GLfloat    m_CoordT { 1.f };
        ::GLenum     m_Target {};
        texture_type m_Texture{};
    }; // class OpenGLImage final

    //**************************************************************************
    // OpenGLImagePtr
    //**************************************************************************
    using OpenGLImagePtr = ::Image::OpenGLImage::pointer;
} // namespace Image

#endif //GUID_5802D78E_2F03_4857_8BD8_3C91BACEDEA8