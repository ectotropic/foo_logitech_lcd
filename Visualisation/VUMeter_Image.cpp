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
#include "Visualisation/VUMeter_Image.h"
#include "Visualisation/VUMeter_Util.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorPacker.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
//--------------------------------------

namespace Image {
    /**************************************************************************
     * Image Data *
     **************************************************************************/
    extern const ImageData::Sparse& GetLogo() noexcept;
}

namespace Visualisation::VUMeter {
    /**************************************************************************
     * ImageVU *
     **************************************************************************/
    ImageVU::ImageVU(const config_type& config,
                     const dimensions_type& dim) :
        base_class{ Type(), config, dim } {
        if (GLEW_ARB_texture_non_power_of_two || GLEW_ARB_texture_rectangle) {
            m_pImageOpenGL = ::Image::OpenGLImage::NewImage(::Image::GetLogo());
        }

        if (!m_pImageOpenGL) {
            m_pImageGDIPlus = ::Image::GDIPlusImage::NewImage(::Image::GetLogo());
        }
    }

    //-------------------------------------------------------------------------

    void ImageVU::Activate(request_param_type& params,
                           const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Decibel;
        params.Decibel.fnTransform = Util::Transformer{ Config() };
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //-------------------------------------------------------------------------

    void ImageVU::Draw(render_pass_type ePass,
                       const audio_data_manager_type& AudioDataManager,
                       float fInterp) {
        const auto canvasSize  = GetDimensions();
        const auto fHalfWidth  = static_cast<float>(canvasSize.cx / 2);
        const auto fHalfHeight = static_cast<float>(canvasSize.cy / 2);

        // Transformer will have converted these values to the range [0,1]
        const auto fLevelL = AudioDataManager.GetDB(Audio::Channel::Left,  fInterp);
        const auto fLevelR = AudioDataManager.GetDB(Audio::Channel::Right, fInterp);

        const auto fImageWidth  = fHalfWidth +  fHalfWidth  * fLevelL;
        const auto fImageHeight = fHalfHeight + fHalfHeight * fLevelR;

        const auto nX0 = static_cast<coord_type>(std::round(fHalfWidth  - (fImageWidth  * .5f)));
        const auto nY0 = static_cast<coord_type>(std::round(fHalfHeight - (fImageHeight * .5f)));
        const auto nX1 = nX0 + static_cast<coord_type>(fImageWidth);
        const auto nY1 = nY0 + static_cast<coord_type>(fImageHeight);

        if (m_pImageOpenGL && ePass == render_pass_type::OpenGL) {
            m_pImageOpenGL->Blend(nX0, nY0,
                                  nX1, nY1);
        } else if (m_pImageGDIPlus && ePass == render_pass_type::GDI) {
            m_pImageGDIPlus->Blend(nX0, nY0,
                                   nX1, nY1);
        }
    }
} //namespace Visualisation::VUMeter
