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
#include "Visualisation/TrackDetails_Basic.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorPacker.h"
//--------------------------------------

namespace Visualisation::Text {
    //**************************************************************************
    // TrackDetails
    //**************************************************************************
    TrackDetails::TrackDetails(page_type ePage,
                               const config_type& config,
                               const dimensions_type& dim,
                               bool bMonochrome) :
        ITrackDetails{
            ePage,
            config,
            dim,
            VisualisationFlags::LowFrameRate
        },
        m_TrackDetails(Windows::GDI::GetCurrentDC()),
        m_Progress(Windows::GDI::GetCurrentDC()),
        m_bShowProgress(false),
        m_nMaxLines(-1) {

        const auto nCanvasWidth  = dim.cx;
        const auto nCanvasHeight = dim.cy;
        const auto fCanvasHeight = static_cast<float>(nCanvasHeight);

        m_TrackDetails.SetFont(Config().m_Text.m_Font, Config().m_Text.m_bAllowOverlap);

        if (Config().m_ProgressBar.m_bEnabled) {
            const auto fTextHeight = fCanvasHeight - Config().m_ProgressBar.m_fHeight;
            const auto nTextHeight = static_cast<coord_type>(std::round(fTextHeight));
            const auto nProgressHeight = static_cast<coord_type>(std::round(Config().m_ProgressBar.m_fHeight));

            m_TrackDetails.SetPosition(0, 0);
            m_TrackDetails.SetSize(nCanvasWidth, nTextHeight);
            m_TrackDetails.SetClip(0, 0, nCanvasWidth, nTextHeight);
            m_TrackDetails.SetBGClear(Config().m_Text.m_bClearBackground);

            m_Progress.SetPosition(0, nTextHeight);
            m_Progress.SetSize(nCanvasWidth, nProgressHeight);
            m_Progress.SetClip(0, nTextHeight, nCanvasWidth, static_cast<coord_type>(std::round(fTextHeight + Config().m_ProgressBar.m_fHeight)));
            Windows::GDI::ProgressText eLeft{ Windows::GDI::ProgressText::None };
            switch (Config().m_ProgressBar.m_Time.m_Left) {
                case TrackTime::TotalTime:        eLeft = Windows::GDI::ProgressText::TotalTime;        break;
                case TrackTime::ElapsedPercent:   eLeft = Windows::GDI::ProgressText::ElapsedPercent;   break;
                case TrackTime::RemainingPercent: eLeft = Windows::GDI::ProgressText::RemainingPercent; break;
                case TrackTime::ElapsedTime:      eLeft = Windows::GDI::ProgressText::ElapsedTime;      break;
                case TrackTime::RemainingTime:    eLeft = Windows::GDI::ProgressText::RemainingTime;    break;
                case TrackTime::SystemTime:       eLeft = Windows::GDI::ProgressText::SystemTime;       break;
                case TrackTime::SystemDate:       eLeft = Windows::GDI::ProgressText::SystemDate;       break;
            }
            Windows::GDI::ProgressText eRight{ Windows::GDI::ProgressText::None };
            switch (Config().m_ProgressBar.m_Time.m_Right) {
                case TrackTime::TotalTime:        eRight = Windows::GDI::ProgressText::TotalTime;        break;
                case TrackTime::ElapsedPercent:   eRight = Windows::GDI::ProgressText::ElapsedPercent;   break;
                case TrackTime::RemainingPercent: eRight = Windows::GDI::ProgressText::RemainingPercent; break;
                case TrackTime::ElapsedTime:      eRight = Windows::GDI::ProgressText::ElapsedTime;      break;
                case TrackTime::RemainingTime:    eRight = Windows::GDI::ProgressText::RemainingTime;    break;
                case TrackTime::SystemTime:       eRight = Windows::GDI::ProgressText::SystemTime;       break;
                case TrackTime::SystemDate:       eRight = Windows::GDI::ProgressText::SystemDate;       break;
            }
            m_Progress.SetTextMode(eLeft, eRight);
            m_Progress.SetClear(Config().m_Text.m_bClearBackground);

            if (Config().m_bSyncProgressBarConfig) {
                m_Progress.SetFont(Config().m_Text.m_Font);
            } else {
                m_Progress.SetFont(TEXT("Arial"), nProgressHeight);
            }

            if (!bMonochrome) {
                m_Progress.PenWidth(Windows::GDI::Layer::Foreground, 2);
                m_Progress.PenWidth(Windows::GDI::Layer::Background, 2);
            }

            if (Config().m_Text.m_bRestrictedLineCount) { m_nMaxLines = 3; }
        } else {
            m_TrackDetails.SetClip(0, 0, nCanvasWidth, nCanvasHeight);

            if (Config().m_Text.m_bRestrictedLineCount) { m_nMaxLines = 4; }
        }

        m_TrackDetails.SetHAlign(Config().m_Text.m_Horizontal.m_Align);
        m_TrackDetails.SetHScroll(Config().m_Text.m_Horizontal.m_Scroll.m_Mode,
                                  Config().m_Text.m_Horizontal.m_Scroll.m_fSpeed,
                                  Config().m_Text.m_Horizontal.m_Scroll.m_fDelay,
                                  Config().m_Text.m_Horizontal.m_Scroll.m_fGap);

        m_TrackDetails.SetVAlign(Config().m_Text.m_Vertical.m_Align);
        m_TrackDetails.SetVScroll(Config().m_Text.m_Vertical.m_Scroll.m_Mode,
                                  Config().m_Text.m_Vertical.m_Scroll.m_fSpeed,
                                  Config().m_Text.m_Vertical.m_Scroll.m_fDelay,
                                  Config().m_Text.m_Vertical.m_Scroll.m_fGap);
    }

    //--------------------------------------------------------------------------

    void TrackDetails::UpdateText(const audio_data_manager_type& AudioDataManager) {
        const auto strPage{ AudioDataManager.GetTrackText(GetPage()) };
        m_TrackDetails.SetText(strPage.c_str(),
                                static_cast<int>(strPage.size()),
                                m_nMaxLines);
    }

    //--------------------------------------------------------------------------

    void TrackDetails::UpdateProgress(const audio_data_manager_type& AudioDataManager) {
        const auto fTrackLength = AudioDataManager.GetTrackLength();
        m_bShowProgress = (fTrackLength > 0.f);
        if (m_bShowProgress) {
            m_Progress.Range(0.f, fTrackLength);
        }
    }

    //--------------------------------------------------------------------------

    void TrackDetails::Activate(request_param_type& params,
                                const audio_data_manager_type& AudioDataManager) {
        params.Want = request_param_type::want_type::None;
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Text.m_Color,
                                               params.PrimaryColor);
        ::Color::PackedColor32ui::ABGR::Unpack(Config().m_Text.m_BackgroundColor,
                                               params.ClearColor);

        m_TrackDetails.Color(Windows::GDI::Layer::Foreground, Config().m_Text.m_Color);
        m_TrackDetails.Color(Windows::GDI::Layer::Background, Config().m_Text.m_BackgroundColor);

        m_Progress.Color(Windows::GDI::Layer::Foreground, Config().m_ProgressBar.m_Color);
        m_Progress.Color(Windows::GDI::Layer::Background, Config().m_ProgressBar.m_BackgroundColor);

        UpdateText(AudioDataManager);
        if (Config().m_ProgressBar.m_bEnabled) {
            UpdateProgress(AudioDataManager);
        }
    }

    //--------------------------------------------------------------------------

    void TrackDetails::Deactivate() noexcept {
        m_TrackDetails.Reset();
    }

    //--------------------------------------------------------------------------

    void TrackDetails::Update(const audio_data_manager_type& AudioDataManager) {
        const bool bFirstUpdate = !m_TrackDetails;
        if (AudioDataManager.HasTrackTextChanged() || bFirstUpdate) {
            UpdateText(AudioDataManager);
        }
        m_TrackDetails.Update();

        if (Config().m_ProgressBar.m_bEnabled) {
            if (AudioDataManager.HasTrackChanged() || bFirstUpdate) {
                UpdateProgress(AudioDataManager);
            }

            m_Progress.Update(AudioDataManager.GetTrackTime());
        }
    }

    //--------------------------------------------------------------------------

    void TrackDetails::Draw(render_pass_type ePass,
                            const audio_data_manager_type& /*AudioDataManager*/,
                            float fInterp) {
        if (ePass != RenderPass::GDI) { return; }

        m_TrackDetails.Draw(fInterp);

        if (Config().m_ProgressBar.m_bEnabled && m_bShowProgress) {
            m_Progress.Draw(fInterp);
        }
    }
} // namespace Visualisation::Text
