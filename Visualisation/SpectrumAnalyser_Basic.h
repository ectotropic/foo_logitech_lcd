#pragma once
#ifndef GUID_DA2F31FB_1C41_4760_8A6F_D7117FB90698
#define GUID_DA2F31FB_1C41_4760_8A6F_D7117FB90698
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

namespace Visualisation::SpectrumAnalyser {
    //**************************************************************************
    // Mono
    //**************************************************************************
    class Mono final : public ISpectrumAnalyser {
    private:
        using base_class = ISpectrumAnalyser;
        using this_class = Mono;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::Mono;
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
            return visualisation_type::Mono;
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
            return visualisation_type::Stereo;
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
            return visualisation_type::Stereo;
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
} // namespace Visualisation::SpectrumAnalyser

#endif // GUID_DA2F31FB_1C41_4760_8A6F_D7117FB90698
