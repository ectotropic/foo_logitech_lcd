#pragma once
#ifndef GUID_E806FAB5_E192_46A1_9B58_EF175B082408
#define GUID_E806FAB5_E192_46A1_9B58_EF175B082408
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

namespace Visualisation::Oscilloscope::detail {
    //**************************************************************************
    // DotLineMono
    //**************************************************************************
    class DotLineMono : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = DotLineMono;

    public:
        using base_class::base_class;

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

    protected:
        void Draw(GLenum mode,
                  render_pass_type ePass,
                  const audio_data_manager_type& AudioDataManager,
                  float fInterp);
    }; // class DotLineMono

    //**************************************************************************
    // DotLineMonoGradient
    //**************************************************************************
    class DotLineMonoGradient : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = DotLineMonoGradient;

    public:
        using base_class::base_class;

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

    protected:
        void Draw(GLenum mode,
                  render_pass_type ePass,
                  const audio_data_manager_type& AudioDataManager,
                  float fInterp);
    }; // class DotLineMonoGradient

    //**************************************************************************
    // DotLineStereo
    //**************************************************************************
    class DotLineStereo : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = DotLineStereo;

    public:
        using base_class::base_class;

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

    protected:
        void Draw(GLenum mode,
                  render_pass_type ePass,
                  const audio_data_manager_type& AudioDataManager,
                  float fInterp);
    }; // class DotLineStereo

    //**************************************************************************
    // DotLineStereoGradient
    //**************************************************************************
    class DotLineStereoGradient : public IOscilloscope {
    private:
        using base_class = IOscilloscope;
        using this_class = DotLineStereoGradient;

    public:
        using base_class::base_class;

        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) noexcept override;

    protected:
        void Draw(GLenum mode,
                  render_pass_type ePass,
                  const audio_data_manager_type& AudioDataManager,
                  float fInterp);
    }; // class DotLineStereoGradient
} // namespace Visualisation::Oscilloscope::detail

//==============================================================================

namespace Visualisation::Oscilloscope {
    //**************************************************************************
    // DotMono
    //**************************************************************************
    class DotMono final : public detail::DotLineMono {
    private:
        using base_class = detail::DotLineMono;
        using this_class = DotMono;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::DotMono;
        }

    public:
        DotMono(const config_type& config,
                const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_POINTS, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class DotMono final

    //**************************************************************************
    // DotMonoGradient
    //**************************************************************************
    class DotMonoGradient final : public detail::DotLineMonoGradient {
    private:
        using base_class = detail::DotLineMonoGradient;
        using this_class = DotMonoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::DotMono;
        }

    public:
        DotMonoGradient(const config_type& config,
                        const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_POINTS, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class DotMonoGradient final

    //**************************************************************************
    // DotStereo
    //**************************************************************************
    class DotStereo final : public detail::DotLineStereo {
    private:
        using base_class = detail::DotLineStereo;
        using this_class = DotStereo;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::DotStereo;
        }

    public:
        DotStereo(const config_type& config,
                  const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_POINTS, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class DotStereo final

    //**************************************************************************
    // DotStereoGradient
    //**************************************************************************
    class DotStereoGradient final : public detail::DotLineStereoGradient {
    private:
        using base_class = detail::DotLineStereoGradient;
        using this_class = DotStereoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::DotStereo;
        }

    public:
        DotStereoGradient(const config_type& config,
                          const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_POINTS, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class DotStereoGradient final

    //**************************************************************************
    // LineMono
    //**************************************************************************
    class LineMono final : public detail::DotLineMono {
    private:
        using base_class = detail::DotLineMono;
        using this_class = LineMono;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::LineMono;
        }

    public:
        LineMono(const config_type& config,
                 const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_LINE_STRIP, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class LineMono final

    //**************************************************************************
    // LineMonoGradient
    //**************************************************************************
    class LineMonoGradient final : public detail::DotLineMonoGradient {
    private:
        using base_class = detail::DotLineMonoGradient;
        using this_class = LineMonoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::LineMono;
        }

    public:
        LineMonoGradient(const config_type& config,
                         const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_LINE_STRIP, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class LineMonoGradient final

    //**************************************************************************
    // LineStereo
    //**************************************************************************
    class LineStereo final : public detail::DotLineStereo {
    private:
        using base_class = detail::DotLineStereo;
        using this_class = LineStereo;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::LineStereo;
        }

    public:
        LineStereo(const config_type& config,
                   const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_LINE_STRIP, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class LineStereo final

    //**************************************************************************
    // LineStereoGradient
    //**************************************************************************
    class LineStereoGradient final : public detail::DotLineStereoGradient {
    private:
        using base_class = detail::DotLineStereoGradient;
        using this_class = LineStereoGradient;

    public:
        static constexpr auto Type() noexcept {
            return visualisation_type::LineStereo;
        }

    public:
        LineStereoGradient(const config_type& config,
                           const dimensions_type& dim) noexcept :
            base_class{ Type(), config, dim } {};

        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override {
            base_class::Draw(GL_LINE_STRIP, ePass,
                             AudioDataManager, fInterp);
        }
    }; // class LineStereoGradient final
} // namespace Visualisation::Oscilloscope

#endif // GUID_E806FAB5_E192_46A1_9B58_EF175B082408
