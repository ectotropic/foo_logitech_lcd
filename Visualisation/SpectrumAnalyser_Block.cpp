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
#include "Visualisation/SpectrumAnalyser_Block.h"
#include "Visualisation/SpectrumAnalyser_Util.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorBlend.h"
#include "ColorPacker.h"
//--------------------------------------

//--------------------------------------
//
#include "GL/glcommon.h"
#include "GL/glscopedutil.h"
//--------------------------------------

namespace Visualisation::SpectrumAnalyser::Block {
    //**************************************************************************
    // Mono
    //**************************************************************************
    void Mono::Activate(request_param_type& params,
                        const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::CombinedSpectrum;
        params.Spectrum.nSampleCountHint = Config().m_Block.m_uCount;
        // Peak decay happens AFTER transform, units/range is same as output of
        // transform here that means decay is % of screen per update (default
        // update being 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Spectrum.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Spectrum.fPeakMininum = 0;
        }
        params.Spectrum.fnTransform = Util::Transformer{ Config() };
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void Mono::Draw(render_pass_type ePass,
                    const audio_data_manager_type& AudioDataManager,
                    float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto bGap = Config().m_Block.m_bGap;
        const auto nBlockCount = Config().m_Block.m_uCount;

        const auto canvasSize = GetDimensions();

        const auto nTotalBlockWidth = bGap ? (canvasSize.cx - (nBlockCount - 1)) : canvasSize.cx;
        const auto nBlockWidth = static_cast<coord_type>(nTotalBlockWidth / nBlockCount);

        {
            const auto samples = AudioDataManager.GetSpectrum(fInterp);
            assert(samples.size() == nBlockCount);
            {
                const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                Util::Draw(
                    samples, canvasSize,
                    [
                        nBlockWidth, bGap
                    ] (auto /*fSample*/, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                        const auto nY0 = 0;
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY0);
                        ::glVertex2i(nX0, nY1);
                        ::glVertex2i(nX1, nY1);
                        ::glVertex2i(nX1, nY0);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }

        if (Config().m_Peak.m_bEnable) {
            const auto peaks = AudioDataManager.GetSpectrumPeaks(fInterp);
            assert(peaks.size() == nBlockCount);
            {
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::Draw(
                    peaks, canvasSize,
                    [
                        nBlockWidth, bGap
                    ] (auto /*fPeak*/, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }
    } // void Mono::Draw(...)

    //**************************************************************************
    // MonoGradient
    //**************************************************************************
    void MonoGradient::Activate(request_param_type& params,
                                const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::CombinedSpectrum;
        params.Spectrum.nSampleCountHint = Config().m_Block.m_uCount;
        // Peak decay happens AFTER transform, units/range is same as output of
        // transform here that means decay is % of screen per update (default
        // update being 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Spectrum.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Spectrum.fPeakMininum = 0;
        }
        params.Spectrum.fnTransform = Util::Transformer{ Config() };
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void MonoGradient::Draw(render_pass_type ePass,
                            const audio_data_manager_type& AudioDataManager,
                            float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto bGap = Config().m_Block.m_bGap;
        const auto nBlockCount = Config().m_Block.m_uCount;

        const auto canvasSize = GetDimensions();

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        const auto nTotalBlockWidth = bGap ? (canvasSize.cx - (nBlockCount - 1)) : canvasSize.cx;
        const auto nBlockWidth = static_cast<coord_type>(nTotalBlockWidth / nBlockCount);

        {
            const auto samples = AudioDataManager.GetSpectrum(fInterp);
            assert(samples.size() == nBlockCount);
            if (Config().m_Color.m_bAltGradientMode) {
                const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                Util::Draw(
                    samples, canvasSize,
                    [
                        nBlockWidth, bGap, color0, color1
                    ] (auto /*fSample*/, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                        const auto nY0 = 0;
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nX0, nY0);

                        ::glColor3f(color1.r(), color1.g(), color1.b());
                        ::glVertex2i(nX0, nY1);
                        ::glVertex2i(nX1, nY1);

                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nX1, nY0);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            } else {
                const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                Util::Draw(
                    samples, canvasSize,
                    [
                        nBlockWidth, bGap, color0, color1
                    ] (auto fSample, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                        const auto colorBlend = ::Color::ColorBlend(color0, color1, fSample);
                        const auto nY0 = 0;
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nX0, nY0);

                        ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                        ::glVertex2i(nX0, nY1);
                        ::glVertex2i(nX1, nY1);

                        ::glColor3f(color0.r(), color0.g(), color0.b());
                        ::glVertex2i(nX1, nY0);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }

        if (Config().m_Peak.m_bEnable) {
            const auto peaks = AudioDataManager.GetSpectrumPeaks(fInterp);
            assert(peaks.size() == nBlockCount);
            if (Config().m_Color.m_bAltGradientMode) {
                ::glColor3f(color1.r(), color1.g(), color1.b());
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::Draw(
                    peaks, canvasSize,
                    [
                        nBlockWidth, bGap
                    ] (auto /*fPeak*/, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            } else {
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::Draw(
                    peaks, canvasSize,
                    [
                        nBlockWidth, bGap, color0, color1
                    ] (auto fPeak, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeak);
                        ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }
    } // void MonoGradient::Draw(...)

    //**************************************************************************
    // Stereo
    //**************************************************************************
    void Stereo::Activate(request_param_type& params,
                          const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Spectrum;
        params.Spectrum.nSampleCountHint = Config().m_Block.m_uCount;
        // Peak decay happens AFTER transform, units/range is same as output of
        // transform here that means decay is % of screen per update (default
        // update being 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Spectrum.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Spectrum.fPeakMininum = 0;
        }
        params.Spectrum.fnTransform = Util::Transformer{ Config() };
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void Stereo::Draw(render_pass_type ePass,
                      const audio_data_manager_type& AudioDataManager,
                      float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto bGap = Config().m_Block.m_bGap;
        const auto nBlockCount = Config().m_Block.m_uCount;

        const auto canvasSize = GetDimensions();

        const auto nTotalBlockWidth = bGap ? (canvasSize.cx - (nBlockCount - 1)) : canvasSize.cx;
        const auto nBlockWidth = static_cast<coord_type>(nTotalBlockWidth / nBlockCount);

        {
            const auto samplesL = AudioDataManager.GetSpectrum(Audio::Channel::Left, fInterp);
            assert(samplesL.size() == nBlockCount);
            const auto samplesR = AudioDataManager.GetSpectrum(Audio::Channel::Right, fInterp);
            assert(samplesR.size() == nBlockCount);
            const auto nHalfHeight = canvasSize.cy / 2;
            {
                const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                Util::Draw(
                    samplesL, samplesR, canvasSize,
                    [
                        nHalfHeight, nBlockWidth, bGap
                    ] (auto /*fSample*/, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                        const auto nY0 = nHalfHeight;
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY0);
                        ::glVertex2i(nX0, nY1);
                        ::glVertex2i(nX1, nY1);
                        ::glVertex2i(nX1, nY0);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }

        if (Config().m_Peak.m_bEnable) {
            const auto peaksL = AudioDataManager.GetSpectrumPeaks(Audio::Channel::Left, fInterp);
            assert(peaksL.size() == nBlockCount);
            const auto peaksR = AudioDataManager.GetSpectrumPeaks(Audio::Channel::Right, fInterp);
            assert(peaksR.size() == nBlockCount);
            {
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                Util::Draw(
                    peaksL, peaksR, canvasSize,
                    [
                        nBlockWidth, bGap
                    ] (auto /*fSample*/, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }
    } // void Stereo::Draw(...)

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    void StereoGradient::Activate(request_param_type& params,
                                  const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Spectrum;
        params.Spectrum.nSampleCountHint = Config().m_Block.m_uCount;
        // Peak decay happens AFTER transform, units/range is same as output of
        // transform here that means decay is % of screen per update (default
        // update being 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Spectrum.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Spectrum.fPeakMininum = 0;
        }
        params.Spectrum.fnTransform = Util::Transformer{ Config() };
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Primary,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Color.m_Palette.Background,
                                               params.ClearColor);
    }

    //--------------------------------------------------------------------------

    void StereoGradient::Draw(render_pass_type ePass,
                              const audio_data_manager_type& AudioDataManager,
                              float fInterp) {
        if (ePass != RenderPass::OpenGL) { return; }

        const auto bGap = Config().m_Block.m_bGap;
        const auto nBlockCount = Config().m_Block.m_uCount;

        const auto canvasSize = GetDimensions();

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        const auto nTotalBlockWidth = bGap ? (canvasSize.cx - (nBlockCount - 1)) : canvasSize.cx;
        const auto nBlockWidth = static_cast<coord_type>(nTotalBlockWidth / nBlockCount);

        {
            const auto samplesL = AudioDataManager.GetSpectrum(Audio::Channel::Left, fInterp);
            const auto samplesR = AudioDataManager.GetSpectrum(Audio::Channel::Right, fInterp);
            const auto nHalfHeight = canvasSize.cy / 2;
            {
                if (Config().m_Color.m_bAltGradientMode) {
                    const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                    Util::Draw(
                        samplesL, samplesR, canvasSize,
                        [
                            nHalfHeight, nBlockWidth, bGap, color0, color1
                        ] (auto /*fSample*/, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                            const auto nY0 = nHalfHeight;
                            const auto nX1 = nX0 + nBlockWidth;
                            ::glColor3f(color0.r(), color0.g(), color0.b());
                            ::glVertex2i(nX0, nY0);

                            ::glColor3f(color1.r(), color1.g(), color1.b());
                            ::glVertex2i(nX0, nY1);
                            ::glVertex2i(nX1, nY1);

                            ::glColor3f(color0.r(), color0.g(), color0.b());
                            ::glVertex2i(nX1, nY0);
                            return bGap ? nX1 + 1 : nX1;
                        }
                    );
                } else {
                    const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
                    Util::Draw(
                        samplesL, samplesR, canvasSize,
                        [
                            nHalfHeight, nBlockWidth, bGap, color0, color1
                        ] (auto fSample, auto nX0, auto nY1) constexpr noexcept -> decltype(nX0) {
                            const auto colorBlend = ::Color::ColorBlend(color0, color1, fSample);
                            const auto nY0 = nHalfHeight;
                            const auto nX1 = nX0 + nBlockWidth;
                            ::glColor3f(color0.r(), color0.g(), color0.b());
                            ::glVertex2i(nX0, nY0);

                            ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                            ::glVertex2i(nX0, nY1);
                            ::glVertex2i(nX1, nY1);

                            ::glColor3f(color0.r(), color0.g(), color0.b());
                            ::glVertex2i(nX1, nY0);
                            return bGap ? nX1 + 1 : nX1;
                        }
                    );
                }
            }
        }

        if (Config().m_Peak.m_bEnable) {
            const auto peaksL = AudioDataManager.GetSpectrumPeaks(Audio::Channel::Left, fInterp);
            const auto peaksR = AudioDataManager.GetSpectrumPeaks(Audio::Channel::Right, fInterp);
            const ::OpenGL::glScopedBegin _begin{ GL_LINES };
            if (Config().m_Color.m_bAltGradientMode) {
                ::glColor3f(color1.r(), color1.g(), color1.b());
                Util::Draw(
                    peaksL, peaksR, canvasSize,
                    [
                        nBlockWidth, bGap
                    ] (auto /*fPeak*/, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            } else {
                Util::Draw(
                    peaksL, peaksR, canvasSize,
                    [
                        nBlockWidth, bGap, color0, color1
                    ] (auto fPeak, auto nX0, auto nY) constexpr noexcept -> decltype(nX0) {
                        const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeak);
                        ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                        const auto nX1 = nX0 + nBlockWidth;
                        ::glVertex2i(nX0, nY);
                        ::glVertex2i(nX1, nY);
                        return bGap ? nX1 + 1 : nX1;
                    }
                );
            }
        }
    } // void StereoGradient::Draw(...)
} // namespace Visualisation::SpectrumAnalyser::Block
