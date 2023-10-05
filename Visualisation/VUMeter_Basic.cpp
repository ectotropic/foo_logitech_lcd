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
#include "Visualisation/VUMeter_Basic.h"
#include "Visualisation/VUMeter_Util.h"
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

namespace Visualisation::VUMeter::HorizontalSplit {
    //**************************************************************************
    // Stereo
    //**************************************************************************
    void Stereo::Activate(request_param_type& params,
                          const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Decibel;
        // Peak decay happens AFTER transform, range is same as output of transform
        // here that means decay is % of screen per update (default update being
        // 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Decibel.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Decibel.fPeakMininum = 0;
        }
        params.Decibel.fnTransform = Util::Transformer{ Config() };
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
        const auto canvasSize = GetDimensions();

        const auto nWidth = canvasSize.cx;
        const auto fWidth = static_cast<float>(nWidth);

        const auto nHeight = canvasSize.cy;
        const auto nHalfHeight = nHeight / 2;

        // Transformer will have converted these values to the range [0,1]
        const auto fLevelL = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Left,  fInterp)) * fWidth;
        const auto fLevelR = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Right, fInterp)) * fWidth;

        const auto fBarLengthL = std::max(0.0f, std::min(fWidth, fLevelL));
        const auto fBarLengthR = std::max(0.0f, std::min(fWidth, fLevelR));

        {
            const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
            {
                const auto nBarL_X0 = 0;
                const auto nBarL_Y0 = 0;
                const auto nBarL_X1 = static_cast<coord_type>(fBarLengthL);
                const auto nBarL_Y1 = nHalfHeight;
                ::glVertex2i(nBarL_X0, nBarL_Y0);
                ::glVertex2i(nBarL_X1, nBarL_Y0);
                ::glVertex2i(nBarL_X1, nBarL_Y1);
                ::glVertex2i(nBarL_X0, nBarL_Y1);
            }

            {
                const auto nBarR_X0 = 0;
                const auto nBarR_Y0 = nHalfHeight + 1;
                const auto nBarR_X1 = static_cast<coord_type>(fBarLengthR);
                const auto nBarR_Y1 = nHeight;
                ::glVertex2i(nBarR_X0, nBarR_Y0);
                ::glVertex2i(nBarR_X1, nBarR_Y0);
                ::glVertex2i(nBarR_X1, nBarR_Y1);
                ::glVertex2i(nBarR_X0, nBarR_Y1);
            }
        }

        if (Config().m_Peak.m_bEnable) {
            // Transformer will have converted these values to the range [0,1]
            const auto fPeakL = AudioDataManager.GetDBPeaks(Audio::Channel::Left,  fInterp) * fWidth;
            const auto fPeakR = AudioDataManager.GetDBPeaks(Audio::Channel::Right, fInterp) * fWidth;

            const ::OpenGL::glScopedBegin _begin{ GL_LINES };
            {
                const auto nPeakL_X0 = static_cast<coord_type>(fPeakL);
                const auto nPeakL_Y0 = 0;
                const auto nPeakL_X1 = nPeakL_X0;
                const auto nPeakL_Y1 = nHalfHeight;
                ::glVertex2i(nPeakL_X0, nPeakL_Y0);
                ::glVertex2i(nPeakL_X1, nPeakL_Y1);
            }

            {
                const auto nPeakR_X0 = static_cast<coord_type>(fPeakR);
                const auto nPeakR_Y0 = nHalfHeight + 1;
                const auto nPeakR_X1 = nPeakR_X0;
                const auto nPeakR_Y1 = nHeight;
                ::glVertex2i(nPeakR_X0, nPeakR_Y0);
                ::glVertex2i(nPeakR_X1, nPeakR_Y1);
            }
        }
    }

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    void StereoGradient::Activate(request_param_type& params,
                                  const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Decibel;
        // Peak decay happens AFTER transform, range is same as output of transform
        // here that means decay is % of screen per update (default update being
        // 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Decibel.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Decibel.fPeakMininum = 0;
        }
        params.Decibel.fnTransform = Util::Transformer{ Config() };
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

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        const auto canvasSize = GetDimensions();

        const auto nWidth = canvasSize.cx;
        const auto fWidth = static_cast<float>(nWidth);

        const auto nHeight = canvasSize.cy;
        const auto nHalfHeight = nHeight / 2;

        // Transformer will have converted these values to the range [0,1]
        const auto fLevelL = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Left,  fInterp)) * fWidth;
        const auto fLevelR = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Right, fInterp)) * fWidth;

        const auto fBarLengthL = std::max(0.0f, std::min(fWidth, fLevelL));
        const auto fBarLengthR = std::max(0.0f, std::min(fWidth, fLevelR));

        {
            const auto nBarL_X0 = 0;
            const auto nBarL_Y0 = 0;
            const auto nBarL_X1 = static_cast<coord_type>(fBarLengthL);
            const auto nBarL_Y1 = nHalfHeight;

            const auto nBarR_X0 = 0;
            const auto nBarR_Y0 = nHalfHeight + 1;
            const auto nBarR_X1 = static_cast<coord_type>(fBarLengthR);
            const auto nBarR_Y1 = nHeight;

            const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
            if (Config().m_Color.m_bAltGradientMode) {
                {
                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarL_X0, nBarL_Y0);

                    ::glColor3f(color1.r(), color1.g(), color1.b());
                    ::glVertex2i(nBarL_X1, nBarL_Y0);
                    ::glVertex2i(nBarL_X1, nBarL_Y1);

                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarL_X0, nBarL_Y1);
                }

                {
                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarR_X0, nBarR_Y0);

                    ::glColor3f(color1.r(), color1.g(), color1.b());
                    ::glVertex2i(nBarR_X1, nBarR_Y0);
                    ::glVertex2i(nBarR_X1, nBarR_Y1);

                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarR_X0, nBarR_Y1);
                }
            } else {
                {
                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarL_X0, nBarL_Y0);

                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fBarLengthL / fWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nBarL_X1, nBarL_Y0);
                    ::glVertex2i(nBarL_X1, nBarL_Y1);

                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarL_X0, nBarL_Y1);
                }

                {
                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarR_X0, nBarR_Y0);

                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fBarLengthR / fWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nBarR_X1, nBarR_Y0);
                    ::glVertex2i(nBarR_X1, nBarR_Y1);

                    ::glColor3f(color0.r(), color0.g(), color0.b());
                    ::glVertex2i(nBarR_X0, nBarR_Y1);
                }
            }
        }

        if (Config().m_Peak.m_bEnable) {
            // Transformer will have converted these values to the range [0,1]
            const auto fPeakL = AudioDataManager.GetDBPeaks(Audio::Channel::Left,  fInterp) * fWidth;
            const auto fPeakR = AudioDataManager.GetDBPeaks(Audio::Channel::Right, fInterp) * fWidth;

            const auto nPeakL_X0 = static_cast<coord_type>(fPeakL);
            const auto nPeakL_Y0 = 0;
            const auto nPeakL_X1 = nPeakL_X0;
            const auto nPeakL_Y1 = nHalfHeight;

            const auto nPeakR_X0 = static_cast<coord_type>(fPeakR);
            const auto nPeakR_Y0 = nHalfHeight + 1;
            const auto nPeakR_X1 = nPeakR_X0;
            const auto nPeakR_Y1 = nHeight;

            const ::OpenGL::glScopedBegin _begin{ GL_LINES };
            if (Config().m_Color.m_bAltGradientMode) {
                ::glColor3f(color1.r(), color1.g(), color1.b());

                {
                    ::glVertex2i(nPeakL_X0, nPeakL_Y0);
                    ::glVertex2i(nPeakL_X1, nPeakL_Y1);
                }

                {
                    ::glVertex2i(nPeakR_X0, nPeakR_Y0);
                    ::glVertex2i(nPeakR_X1, nPeakR_Y1);
                }
            } else {
                {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeakL / fWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());

                    ::glVertex2i(nPeakL_X0, nPeakL_Y0);
                    ::glVertex2i(nPeakL_X1, nPeakL_Y1);
                }

                {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeakR / fWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());

                    ::glVertex2i(nPeakR_X0, nPeakR_Y0);
                    ::glVertex2i(nPeakR_X1, nPeakR_Y1);
                }
            }
        }
    }
} // namespace Visualisation::VUMeter::HorizontalSplit

//==============================================================================

namespace Visualisation::VUMeter::VerticalSplit {
    //**************************************************************************
    // Stereo
    //**************************************************************************
    void Stereo::Activate(request_param_type& params,
                          const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Decibel;
        // Peak decay happens AFTER transform, range is same as output of transform
        // here that means decay is % of screen per update (default update being
        // 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Decibel.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Decibel.fPeakMininum = 0;
        }
        params.Decibel.fnTransform = Util::Transformer{ Config() };
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
        const auto canvasSize = GetDimensions();

        const auto nWidth = canvasSize.cx;
        const auto nHalfWidth = nWidth / 2;
        const auto fHalfWidth = static_cast<float>(nHalfWidth);

        const auto nHeight = canvasSize.cy;

        // Transformer will have converted these values to the range [0,1]
        const auto fLevelL = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Left,  fInterp)) * fHalfWidth;
        const auto fLevelR = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Right, fInterp)) * fHalfWidth;

        const auto fBarLengthL = std::max(0.0f, std::min(fHalfWidth, fLevelL));
        const auto fBarLengthR = std::max(0.0f, std::min(fHalfWidth, fLevelR));

        {
            const auto nBarL_X0 = 0;
            const auto nBarL_Y0 = 0;
            const auto nBarL_X1 = static_cast<coord_type>(fBarLengthL);
            const auto nBarL_Y1 = nHeight;

            const auto nBarR_X0 = nWidth;
            const auto nBarR_Y0 = 0;
            const auto nBarR_X1 = nWidth - static_cast<coord_type>(fBarLengthR);
            const auto nBarR_Y1 = nHeight;

            const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
            {
                ::glVertex2i(nBarL_X0, nBarL_Y0);
                ::glVertex2i(nBarL_X0, nBarL_Y1);
                ::glVertex2i(nBarL_X1, nBarL_Y1);
                ::glVertex2i(nBarL_X1, nBarL_Y0);
            }

            {
                ::glVertex2i(nBarR_X0, nBarR_Y0);
                ::glVertex2i(nBarR_X0, nBarR_Y1);
                ::glVertex2i(nBarR_X1, nBarR_Y1);
                ::glVertex2i(nBarR_X1, nBarR_Y0);
            }
        }

        if (Config().m_Peak.m_bEnable) {
            // Transformer will have converted these values to the range [0,1]
            const auto fPeakL = AudioDataManager.GetDBPeaks(Audio::Channel::Left,  fInterp) * fHalfWidth;
            const auto fPeakR = AudioDataManager.GetDBPeaks(Audio::Channel::Right, fInterp) * fHalfWidth;

            const auto nPeakL_X0 = static_cast<coord_type>(fPeakL);
            const auto nPeakL_Y0 = 0;
            const auto nPeakL_X1 = nPeakL_X0;
            const auto nPeakL_Y1 = nHeight;

            const auto nPeakR_X0 = nWidth - static_cast<coord_type>(fPeakR);
            const auto nPeakR_Y0 = 0;
            const auto nPeakR_X1 = nPeakR_X0;
            const auto nPeakR_Y1 = nHeight;

            const ::OpenGL::glScopedBegin _begin{ GL_LINES };
            {
                ::glVertex2i(nPeakL_X0, nPeakL_Y0);
                ::glVertex2i(nPeakL_X1, nPeakL_Y1);
            }

            {
                ::glVertex2i(nPeakR_X0, nPeakR_Y0);
                ::glVertex2i(nPeakR_X1, nPeakR_Y1);
            }
        }
    }

    //**************************************************************************
    // StereoGradient
    //**************************************************************************
    void StereoGradient::Activate(request_param_type& params,
                                  const audio_data_manager_type& /*AudioDataManager*/) {
        params.Want = request_param_type::want_type::Decibel;
        // Peak decay happens AFTER transform, range is same as output of transform
        // here that means decay is % of screen per update (default update being
        // 15fps). (% expressed as float with 1.f being 100%).
        if (Config().m_Peak.m_bEnable) {
            params.Decibel.fPeakDecayRate = Config().m_Peak.m_fDecayRate;
            params.Decibel.fPeakMininum = 0;
        }
        params.Decibel.fnTransform = Util::Transformer{ Config() };
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
        const auto canvasSize = GetDimensions();
        const auto nHalfWidth = canvasSize.cx / 2;
        const auto fHalfWidth = static_cast<float>(nHalfWidth);

        using ColorUnpacker = ::Color::PackedColor32ui::ABGR;
        const auto color0{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Primary) };
        const auto color1{ ColorUnpacker::Unpack<::Color::Color3f>(Config().m_Color.m_Palette.Secondary) };

        // Transformer will have converted these values to the range [0,1]
        const auto fLevelL = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Left,  fInterp)) * fHalfWidth;
        const auto fLevelR = static_cast<float>(AudioDataManager.GetDB(Audio::Channel::Right, fInterp)) * fHalfWidth;

        const auto fBarLengthL = std::max(0.0f, std::min(fHalfWidth, fLevelL));
        const auto fBarLengthR = std::max(0.0f, std::min(fHalfWidth, fLevelR));
        const auto nBarLengthL = static_cast<coord_type>(fBarLengthL);
        const auto nBarLengthR = static_cast<coord_type>(fBarLengthR);

        {
            const ::OpenGL::glScopedBegin _begin{ GL_QUADS };
            {
                constexpr const coord_type nX0 = 0, nY0 = 0;
                const coord_type nX1 = nBarLengthL, nY1 = canvasSize.cy;
                ::glColor3f(color0.r(), color0.g(), color0.b());
                ::glVertex2i(nX0, 0);

                const auto colorBlend = ::Color::ColorBlend(color0, color1, fBarLengthL / fHalfWidth);
                ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                ::glVertex2i(nX1, nY0);
                ::glVertex2i(nX1, nY1);

                ::glColor3f(color0.r(), color0.g(), color0.b());
                ::glVertex2i(nX0, nY1);
            }

            {
                const coord_type nX0 = canvasSize.cx, nY0 = 0;
                const coord_type nX1 = canvasSize.cx - nBarLengthR, nY1 = canvasSize.cy;
                ::glColor3f(color0.r(), color0.g(), color0.b());
                ::glVertex2i(nX0, nY0);

                const auto colorBlend = ::Color::ColorBlend(color0, color1, fBarLengthR / fHalfWidth);
                ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                ::glVertex2i(nX1, nY0);
                ::glVertex2i(nX1, nY1);

                ::glColor3f(color0.r(), color0.g(), color0.b());
                ::glVertex2i(nX0, nY1);
            }
        }

        if (Config().m_Peak.m_bEnable) {
            // Transformer will have converted these values to the range [0,1]
            const auto fPeakL = AudioDataManager.GetDBPeaks(Audio::Channel::Left,  fInterp) * fHalfWidth;
            const auto fPeakR = AudioDataManager.GetDBPeaks(Audio::Channel::Right, fInterp) * fHalfWidth;

            const auto nPeakL = static_cast<coord_type>(fPeakL);
            const auto nPeakR = static_cast<coord_type>(fPeakR);

            {
                const ::OpenGL::glScopedBegin _begin{ GL_LINES };
                {
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeakL / fHalfWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nPeakL, 0);
                    ::glVertex2i(nPeakL, canvasSize.cy);
                }

                {
                    const auto nX = canvasSize.cx - nPeakR;
                    const auto colorBlend = ::Color::ColorBlend(color0, color1, fPeakR / fHalfWidth);
                    ::glColor3f(colorBlend.r(), colorBlend.g(), colorBlend.b());
                    ::glVertex2i(nX, 0);
                    ::glVertex2i(nX, canvasSize.cy);
                }
            }
        }
    }
} // namespace Visualisation::VUMeter::VerticalSplit

