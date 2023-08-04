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
#include "Visualisation/Oscilloscope_Radial.h"
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

namespace Visualisation::Oscilloscope::Radial {
    //**************************************************************************
    // CircleMono
    //**************************************************************************
    void CircleMono::Activate(request_param_type& params,
                              const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = Util::RadialSampleCount;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void CircleMono::Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto canvasSize = GetDimensions();
        const auto nHalfWidth = canvasSize.cx / 2;
        const auto nHalfHeight = canvasSize.cy / 2;

        sample_type fMaxDimension{ 0 }, fMinDimension{ 0 };
        if (canvasSize.cx > canvasSize.cy) {
            fMaxDimension = static_cast<sample_type>(canvasSize.cx);
            fMinDimension = static_cast<sample_type>(canvasSize.cy);
        } else {
            fMaxDimension = static_cast<sample_type>(canvasSize.cy);
            fMinDimension = static_cast<sample_type>(canvasSize.cx);
        }

        const auto fTargetRadius = (fMinDimension - (fMinDimension * .1f)) * .5f; //< 1% margin
        const auto fRadiusOffset = fTargetRadius / fMaxDimension;
        const auto fRadiusFactor = 1.0f - fRadiusOffset;

        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            const ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
            const ::OpenGL::glScopedBegin _begin{ GL_LINE_LOOP };
            Util::DrawRadial(
                samples, canvasSize, Config().m_fScale,
                [
                    nHalfWidth, nHalfHeight, fRadiusOffset, fRadiusFactor
                ] (auto fSample, auto fX, auto fY) constexpr noexcept -> void {
                    const auto nX = nHalfWidth + static_cast<coord_type>(fX * fRadiusOffset + fX * fRadiusFactor * fSample);
                    const auto nY = nHalfHeight + static_cast<coord_type>(fY * fRadiusOffset + fY * fRadiusFactor * fSample);
                    ::glVertex2i(nX, nY);
                }
            );
        }
    }

    //**************************************************************************
    // CircleMonoGradient
    //**************************************************************************
    void CircleMonoGradient::Activate(request_param_type& params,
                                      const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = Util::RadialSampleCount;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void CircleMonoGradient::Draw(render_pass_type ePass,
                                  const audio_data_manager_type& AudioDataManager,
                                  float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto canvasSize = GetDimensions();
        const auto nHalfWidth = canvasSize.cx / 2;
        const auto nHalfHeight = canvasSize.cy / 2;

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        sample_type fMaxDimension{ 0 }, fMinDimension{ 0 };
        if (canvasSize.cx > canvasSize.cy) {
            fMaxDimension = static_cast<sample_type>(canvasSize.cx);
            fMinDimension = static_cast<sample_type>(canvasSize.cy);
        } else {
            fMaxDimension = static_cast<sample_type>(canvasSize.cy);
            fMinDimension = static_cast<sample_type>(canvasSize.cx);
        }

        const auto fTargetRadius = (fMinDimension - (fMinDimension * .1f)) * .5f; //< 1% margin
        const auto fRadiusOffset = fTargetRadius / fMaxDimension;
        const auto fRadiusFactor = 1.0f - fRadiusOffset;

        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            const ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
            const ::OpenGL::glScopedBegin _begin{ GL_LINE_LOOP };
            Util::DrawRadial(
                samples, canvasSize, Config().m_fScale,
                [
                    nHalfWidth, nHalfHeight, fRadiusOffset, fRadiusFactor, color0, color1
                ] (auto fSample, auto fX, auto fY) constexpr noexcept -> void {
                    const auto nX = nHalfWidth + static_cast<coord_type>(fX * fRadiusOffset + fX * fRadiusFactor * fSample);
                    const auto nY = nHalfHeight + static_cast<coord_type>(fY * fRadiusOffset + fY * fRadiusFactor * fSample);
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, (fSample * 4.0f) * 0.5f + 0.5f);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nX, nY);
                }
            );
        }
    }

    //**************************************************************************
    // StarBurstMono
    //**************************************************************************
    void StarBurstMono::Activate(request_param_type& params,
                                 const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = Util::RadialSampleCount;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void StarBurstMono::Draw(render_pass_type ePass,
                             const audio_data_manager_type& AudioDataManager,
                             float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto canvasSize = GetDimensions();
        const auto nHalfWidth = canvasSize.cx / 2;
        const auto nHalfHeight = canvasSize.cy / 2;

        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            {
                const ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::DrawRadial(
                    samples, canvasSize, 2.f * Config().m_fScale,
                    [
                        nHalfWidth, nHalfHeight
                    ] (auto fSample, auto fX, auto fY) constexpr noexcept -> void {
                        const auto nX = nHalfWidth + static_cast<coord_type>(fX * fSample);
                        const auto nY = nHalfHeight + static_cast<coord_type>(fY * fSample);
                        ::glVertex2i(nHalfWidth, nHalfHeight);
                        ::glVertex2i(nX, nY);
                    }
                );
            }
        }
    }

    //**************************************************************************
    // StarBurstMonoGradient
    //**************************************************************************
    void StarBurstMonoGradient::Activate(request_param_type& params,
                                         const audio_data_manager_type& /*AudioDataManager*/) noexcept {
        params.Want = request_param_type::want_type::CombinedWaveform;
        params.Waveform.nSampleCountHint = Util::RadialSampleCount;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void StarBurstMonoGradient::Draw(render_pass_type ePass,
                                     const audio_data_manager_type& AudioDataManager,
                                     float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto canvasSize = GetDimensions();
        const auto nHalfWidth = canvasSize.cx / 2;
        const auto nHalfHeight = canvasSize.cy / 2;

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        {
            const auto samples = AudioDataManager.GetWaveform(fInterp);
            if (Config().m_Color.m_bAltGradientMode) {
                const ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::DrawRadial(
                    samples, canvasSize, Config().m_fScale,
                    [
                        nHalfWidth, nHalfHeight, color0, color1
                    ] (auto fSample, auto fX, auto fY) constexpr noexcept -> void {
                        const auto nX = nHalfWidth + static_cast<coord_type>(fX * fSample);
                        const auto nY = nHalfHeight + static_cast<coord_type>(fY * fSample);
                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nHalfWidth, nHalfHeight);
                        ::glColor3f(color1.r(), color1.g(), color1.b());
                        ::glVertex2i(nX, nY);
                    }
                );
            } else {
                const ::OpenGL::glScopedSetLineWidth _lineWidth{ Config().m_fLineWidth };
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::DrawRadial(
                    samples, canvasSize, Config().m_fScale,
                    [
                        nHalfWidth, nHalfHeight, color0, color1
                    ] (auto fSample, auto fX, auto fY) constexpr noexcept -> void {
                        const auto nX = nHalfWidth + static_cast<coord_type>(fX * fSample);
                        const auto nY = nHalfHeight + static_cast<coord_type>(fY * fSample);
                        const auto colorBlend = ::Color::ColorBlend(color0, color1, std::fabs(fSample) * 4.0f);
                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nHalfWidth, nHalfHeight);
                        ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                        ::glVertex2i(nX, nY);
                    }
                );
            }
        }
    }
} // namespace Visualisation::Oscilloscope::Radial
