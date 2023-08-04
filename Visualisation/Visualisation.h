#pragma once
#ifndef GUID_5B611B62_9918_490F_A4D6_6DDAD3157EBE
#define GUID_5B611B62_9918_490F_A4D6_6DDAD3157EBE
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
#include "Visualisation/Visualisation_Params.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/FlagEnum.h"
//--------------------------------------

//--------------------------------------
//
#include "RenderCommon.h"
#include "Config/Config_General.h"
//--------------------------------------

//--------------------------------------
//
#include <cstddef>
#include <memory>
//--------------------------------------

namespace Audio {
    //**************************************************************************
    // IAudioDataManager [Forward Declaration]
    //**************************************************************************
    class IAudioDataManager;
}

//==============================================================================

namespace Visualisation {
    //**************************************************************************
    // VisualisationFlags
    //**************************************************************************
    FLAG_ENUM(VisualisationFlags,
              FLAG_ENUM_VALUES(None                  = 0,
                               LowFrameRate          = 1 << 0,
                               PreferFrequentUpdates = 1 << 1));

    //**************************************************************************
    // VisualisationModeFlags
    //**************************************************************************
    FLAG_ENUM(VisualisationModeFlags,
              FLAG_ENUM_VALUES(None       = 0,
                               Monochrome = 1 << 0,
                               ExpertMode = 1 << 1));

    //**************************************************************************
    // Dimensions
    //**************************************************************************
    struct Dimensions {
        int cx{ 0 };
        int cy{ 0 };
    };

    //**************************************************************************
    // IVisualisation
    //**************************************************************************
    class IVisualisation {
    public:
        using pointer_type = std::shared_ptr<IVisualisation>;

        using size_type       = std::size_t;
        using dimensions_type = Dimensions;
        using flags_type      = ::Visualisation::VisualisationFlags;
        using mode_type       = ::VisualisationMode;

        using render_pass_type        = ::RenderPass;
        using audio_data_manager_type = ::Audio::IAudioDataManager;
        using request_param_type      = ::Visualisation::RequestParams;

    public:
        //static visualisation_ptr MakeVisualisation(mode_type eMode,
        //                                           size_type eType,
        //                                           const ... config,
        //                                           const dimensions_type& dim,
        //                                           flags_type flags);

    protected:
        IVisualisation(size_type nMode,
                       size_type nIndex,
                       const dimensions_type& dim,
                       flags_type flags) noexcept :
            m_nMode     { nMode },
            m_nIndex    { nIndex },
            m_Flags     { flags },
            m_Dimensions{ dim } {}

        IVisualisation(size_type nMode,
                       size_type nIndex,
                       const dimensions_type& dim) noexcept :
            IVisualisation{ nMode, nIndex, dim, VisualisationFlags::None } {}

    public:
        virtual ~IVisualisation() = default;

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) = 0;

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void Deactivate() {};

        gsl_suppress(26440) // C26440: Function '...' can be declared 'noexcept' (f.6).
        virtual void Update(const audio_data_manager_type& /*AudioDataManager*/) {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) = 0;

    public:
        constexpr auto GetMode      () const noexcept { return m_nMode; }
        constexpr auto GetIndex     () const noexcept { return m_nIndex; }
        constexpr auto GetDimensions() const noexcept { return m_Dimensions; }
        constexpr auto GetFlags     () const noexcept { return m_Flags; }

        template <typename ModeT, typename IndexT>
        constexpr auto Is(ModeT mode, IndexT index) const noexcept {
            return m_nMode == static_cast<size_type>(mode) &&
                   m_nIndex == static_cast<size_type>(index);
        }

    private:
        size_type       m_nMode     { 0 };
        size_type       m_nIndex    { 0 };
        dimensions_type m_Dimensions{ 0, 0 };
        flags_type      m_Flags     { VisualisationFlags::None };
    }; // class IVisualisation
} // namespace Visualisation

#endif // GUID_5B611B62_9918_490F_A4D6_6DDAD3157EBE