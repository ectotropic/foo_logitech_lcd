#pragma once
#ifndef GUID_0E0D3ACD_4470_4E55_8F4F_897B04702C9C
#define GUID_0E0D3ACD_4470_4E55_8F4F_897B04702C9C
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
#include "Visualisation/Oscilloscope.h"
//--------------------------------------

namespace Visualisation::Oscilloscope::Radial {
    //**************************************************************************
    // CircleMono
    //**************************************************************************
    class CircleMono final : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = CircleMono;

    public:
        static constexpr auto Type() noexcept { return visualisation_type::CircleMono; }

    public:
        CircleMono(const config_type& config,
                   const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class CircleMono final

    //**************************************************************************
    // CircleMonoGradient
    //**************************************************************************
    class CircleMonoGradient final : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = CircleMonoGradient;

    public:
        static constexpr auto Type() noexcept { return visualisation_type::CircleMono; }

    public:
        CircleMonoGradient(const config_type& config,
                           const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class CircleMonoGradient final

    //**************************************************************************
    // StarBurstMono
    //**************************************************************************
    class StarBurstMono final : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = StarBurstMono;

    public:
        static constexpr auto Type() noexcept { return visualisation_type::StarBurstMono; }

    public:
        StarBurstMono(const config_type& config,
                      const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class StarBurstMono final

    //**************************************************************************
    // StarBurstMonoGradient
    //**************************************************************************
    class StarBurstMonoGradient final : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = StarBurstMonoGradient;

    public:
        static constexpr auto Type() noexcept { return visualisation_type::StarBurstMono; }

    public:
        StarBurstMonoGradient(const config_type& config,
                              const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {}

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;
    }; // class StarBurstMonoGradient final
} // namespace Visualisation::Oscilloscope::Radial

#endif // GUID_0E0D3ACD_4470_4E55_8F4F_897B04702C9C
