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
#include "Visualisation/Oscilloscope_Basic.h"
#include "Visualisation/Oscilloscope_Util.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorPacker.h"
#include "ColorBlend.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
#include "GL/glscopedutil.h"
//--------------------------------------

namespace Visualisation::Oscilloscope::detail {
    //**************************************************************************
    // DotLineMono
    //**************************************************************************
    void DotLineMono::Activate(request_param_type& params,
                               const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = GetDimensions().cx;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //-------------------------------------------------------------------------

    void DotLineMono::Draw(GLenum mode,
                           render_pass_type ePass,
                           const audio_data_manager_type& AudioDataManager,
                           float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }
        assert(mode == GL_POINTS || mode == GL_LINE_STRIP);

        const auto canvasSize = GetDimensions();

        ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
        ::OpenGL::glScopedSetPointSize _pointSize{ Config().m_fPointSize };
        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samples, canvasSize, Config().m_fScale,
                [](auto /*fSample*/, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    ::glVertex2i(nX, nY);
                    return nX + 1;
                }
            );
        }
    }

    //**************************************************************************
    // DotLineMonoGradient
    //**************************************************************************
    void DotLineMonoGradient::Activate(request_param_type& params,
                                       const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = GetDimensions().cx;

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;

        ColorUnpacker::Unpack(Config().m_Color.m_Palette.Primary,
                              params.PrimaryColor);
        ColorUnpacker::Unpack(Config().m_Color.m_Palette.Background,
                              params.ClearColor);
    }

    //-------------------------------------------------------------------------

    void DotLineMonoGradient::Draw(GLenum mode,
                                   render_pass_type ePass,
                                   const audio_data_manager_type& AudioDataManager,
                                   float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }
        assert(mode == GL_POINTS || mode == GL_LINE_STRIP);

        const auto canvasSize = GetDimensions();

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
        ::OpenGL::glScopedSetPointSize _pointSize{ Config().m_fPointSize };
        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samples, canvasSize, Config().m_fScale,
                [
                    color0, color1
                ] (auto fSample, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fSample);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nX, nY);
                    return nX + 1;
                }
            );
        }
    }

    //**************************************************************************
    // DotLineStereo
    //**************************************************************************
    void DotLineStereo::Activate(request_param_type& params,
                                 const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::Waveform;
        params.Waveform.nSampleCountHint = GetDimensions().cx / 2;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //-------------------------------------------------------------------------

    void DotLineStereo::Draw(GLenum mode,
                             render_pass_type ePass,
                             const audio_data_manager_type& AudioDataManager,
                             float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }
        assert(mode == GL_POINTS || mode == GL_LINE_STRIP);

        const auto canvasSize = GetDimensions();
        const auto nWidth = canvasSize.cx;

        ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
        ::OpenGL::glScopedSetPointSize _pointSize{ Config().m_fPointSize };
        {
            const auto samplesL = AudioDataManager.GetWaveform(Audio::Channel::Left, fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samplesL, canvasSize, Config().m_fScale,
                [](auto /*fSample*/, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    ::glVertex2i(nX, nY);
                    return nX + 1;
                }
            );
        }

        {
            const auto samplesR = AudioDataManager.GetWaveform(Audio::Channel::Right, fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samplesR, canvasSize, Config().m_fScale,
                [
                    nWidth
                ] (auto /*fSample*/, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    ::glVertex2i(nWidth - nX, nY);
                    return nX + 1;
                }
            );
        }
    }

    //**************************************************************************
    // DotLineStereoGradient
    //**************************************************************************
    void DotLineStereoGradient::Activate(request_param_type& params,
                                         const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::Waveform;
        params.Waveform.nSampleCountHint = GetDimensions().cx / 2;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //-------------------------------------------------------------------------

    void DotLineStereoGradient::Draw(GLenum mode,
                                     render_pass_type ePass,
                                     const audio_data_manager_type& AudioDataManager,
                                     float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }
        assert(mode == GL_POINTS || mode == GL_LINE_STRIP);

        const auto canvasSize = GetDimensions();
        const auto nWidth = canvasSize.cx;

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
        ::OpenGL::glScopedSetPointSize _pointSize{ Config().m_fPointSize };
        {
            const auto samplesL = AudioDataManager.GetWaveform(Audio::Channel::Left, fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samplesL, canvasSize, Config().m_fScale,
                [
                    color0, color1
                ] (auto fSample, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fSample);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nX, nY);
                    return nX + 1;
                }
            );
        }

        {
            const auto samplesR = AudioDataManager.GetWaveform(Audio::Channel::Right, fInterp);
            const ::OpenGL::glScopedBegin _begin{ mode };
            Util::Draw(
                samplesR, canvasSize, Config().m_fScale,
                [
                    nWidth, color0, color1
                ] (auto fSample, auto nX, auto nY) constexpr noexcept -> decltype(nX) {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fSample);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nWidth - nX, nY);
                    return nX + 1;
                }
            );
        }
    }
} // namespace Visualisation::Oscilloscope::detail
