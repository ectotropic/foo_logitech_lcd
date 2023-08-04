#pragma once
#ifndef GUID_3B1F19F7_863C_481F_9C24_C43EC52FD702
#define GUID_3B1F19F7_863C_481F_9C24_C43EC52FD702
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
#include "Visualisation/VUMeter.h"
//--------------------------------------

//--------------------------------------
//
#include "Image_GDIPlus.h"
#include "Image_OpenGL.h"
//--------------------------------------

namespace Visualisation::VUMeter {
    //**************************************************************************
    // ImageVU
    //**************************************************************************
    class ImageVU final : public IVUMeter {
    private:
        using base_class = IVUMeter;
        using this_class = ImageVU;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Image;
        }

    public:
        ImageVU(const config_type& config,
                const dimensions_type& dim);

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;

    private:
        ::Image::GDIPlusImagePtr m_pImageGDIPlus{};
        ::Image::OpenGLImagePtr  m_pImageOpenGL{};
    }; // class ImageVU final
} // namespace Visualisation::VUMeter

#endif // GUID_3B1F19F7_863C_481F_9C24_C43EC52FD702
