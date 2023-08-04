#pragma once
#ifndef GUID_D92AD2A6_8C55_49D0_BE61_B31FA3DF4E6F
#define GUID_D92AD2A6_8C55_49D0_BE61_B31FA3DF4E6F
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
#include "Visualisation/SpectrumAnalyser.h"
//--------------------------------------

namespace Visualisation::SpectrumAnalyser::Block {
    //**************************************************************************
    // Mono
    //**************************************************************************
    class Mono final : public ISpectrumAnalyser {
    private:
        using base_class = ISpectrumAnalyser;
        using this_class = Mono;

    public:
        static const auto Type() noexcept {
            return visualisation_type::MonoBlock;
        }

    public:
        Mono(const config_type& config,
             const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class Mono final

    //**************************************************************************
    // MonoGradient
    //**************************************************************************
    class MonoGradient final : public ISpectrumAnalyser {
    private:
        using base_class = ISpectrumAnalyser;
        using this_class = MonoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::MonoBlock;
        }

    public:
        MonoGradient(const config_type& config,
                     const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class MonoGradient final

    //**************************************************************************
    // Stereo
    //**************************************************************************
    class Stereo final : public ISpectrumAnalyser {
    private:
        using base_class = ISpectrumAnalyser;
        using this_class = Stereo;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::StereoBlock;
        }

    public:
        Stereo(const config_type& config,
               const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class Stereo final

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    class StereoGradient final : public ISpectrumAnalyser {
    private:
        using base_class = ISpectrumAnalyser;
        using this_class = StereoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::StereoBlock;
        }

    public:
        StereoGradient(const config_type& config,
                       const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class StereoGradient final
} // namespace Visualisation::SpectrumAnalyser::Block

#endif // GUID_D92AD2A6_8C55_49D0_BE61_B31FA3DF4E6F
