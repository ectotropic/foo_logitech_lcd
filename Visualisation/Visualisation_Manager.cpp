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
#include "Visualisation_Manager.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/SpectrumAnalyser.h"
#include "Visualisation/Oscilloscope.h"
#include "Visualisation/VUMeter.h"
#include "Visualisation/TrackDetails.h"
//--------------------------------------

//--------------------------------------
//
#include "Canvas.hpp"
//--------------------------------------

//--------------------------------------
//
#include "Image_ImageLoader.h"
//--------------------------------------

//--------------------------------------
//
#include "ColorCast.h"
//--------------------------------------

//******************************************************************************
// VisualisationManager
//******************************************************************************
bool VisualisationManager::Initialise() {
    m_Config = Config::GeneralConfig::get();

    {
        m_pDataManager = ::Audio::IAudioDataManager::instance();
        assert(m_pDataManager); if (!m_pDataManager) { return false; }
    }

    {
        m_pDisplay = ::LCD::ILCD::instance();
        assert(m_pDisplay); if (!m_pDisplay) { return false; }
    }

    {
        m_pCanvas = ::Windows::Canvas::instance();
        assert(m_pCanvas); if (!m_pCanvas) { return false; }

        if (!m_pCanvas->IsValid()) {
            m_pCanvas->Initialise(GetWidth(), GetHeight(), (IsMonochrome() ? 1 : 32), CanvasConfig().m_bPreferHardwareCanvas);
            m_pCanvas->SetTransparentClears(CanvasConfig().m_bUseTrailEffect);
            if (!(m_pCanvas && m_pCanvas->IsValid())) {
                Uninitialise();
                LogError("Failed to create canvas. Plugin will be unavailable.");
                return false;
            }
        }
    }

    m_bAutoChange = VisualisationConfig().m_AutoChange.m_bEnable;
    const auto fChangeSeconds{ VisualisationConfig().m_AutoChange.m_fChangeSeconds };
    m_AutoChange.Reset(TimedEvent::Duration::Seconds{ fChangeSeconds });

    m_bTrackChangePopup = VisualisationConfig().m_OnTrackChange.m_bShowTrackDetails;
    const auto fShowSeconds{ VisualisationConfig().m_OnTrackChange.m_fShowSeconds };
    m_TrackChangePopup.Reset(TimedEvent::Duration::Seconds{ fShowSeconds });

    m_bTrackChangeAlert = DisplayConfig().m_bPopUpOnTrackChange;
    m_TrackChangeAlert.Reset(TimedEvent::Duration::Seconds{ 1.f });

    InitialiseVisualisations();
    return true;
}

//------------------------------------------------------------------------------

void VisualisationManager::Uninitialise() noexcept {
    try {
        UninitialiseVisualisations();
        if (m_pCanvas) {
            m_pCanvas->Uninitialise();
            m_pCanvas.reset();
        }
        if (m_pDataManager) { m_pDataManager.reset(); }
        if (m_pDisplay) { m_pDisplay.reset(); }
    } catch (...) {
        SafeLogCritical("Unhandled exception during '" __FUNCTION__ "'. Ignoring.");
        assert(false);
    }
}

//------------------------------------------------------------------------------

// Allocate and store all visualisations
void VisualisationManager::InitialiseVisualisations() {
    m_bHaveVisualisations = false;
    const auto dim = Visualisation::Dimensions{ m_pCanvas->GetWidth(), m_pCanvas->GetHeight() };
    const auto bMonoChrome = m_pCanvas->IsMonochrome();

    std::size_t modeActiveCount{ 0 }, visMaxIndex{ 0 };

    const auto& startConfig{ VisualisationConfig().m_Start };

    m_eCurrentVisMode = VisualisationMode::ModeError;

    //Spectrum vis
    {
        namespace Spec = Visualisation::SpectrumAnalyser;
        constexpr const auto visMode{ VisualisationMode::SpectrumAnalyser };
        for (const auto s : SpectrumAnalyserType{}) {
            const auto& specConfig = Config::SpectrumAnalyserConfig::get(s);
            if (specConfig.m_bEnabled) {
                auto pVis = Spec::ISpectrumAnalyser::MakeVisualisation(s,
                                                                       specConfig,
                                                                       dim,
                                                                       bMonoChrome);
                m_Visualisations[visMode].push_back(std::move(pVis));
            }
        }

        const auto visCount{ m_Visualisations[visMode].size() };
        if (visCount > 0) {
            ++modeActiveCount;
            if (visCount > visMaxIndex) { visMaxIndex = visCount; }
            if (startConfig.m_bRememberLast && startConfig.m_LastMode == visMode) {
                m_eCurrentVisMode = visMode;
                m_CurrentVisIndex[visMode] = startConfig.m_LastIndex;
            } else {
                m_CurrentVisIndex[visMode] = 0;
            }
        } else {
            m_CurrentVisIndex[visMode] = -1;
        }
    }

    //Oscilloscope vis
    {
        namespace Osc = Visualisation::Oscilloscope;
        constexpr const auto visMode{ VisualisationMode::Oscilloscope };
        for (const auto o : OscilloscopeType{}) {
            const auto& oscConfig = Config::OscilloscopeConfig::get(o);
            if (oscConfig.m_bEnabled) {
                auto pVis = Osc::IOscilloscope::MakeVisualisation(o,
                                                                  oscConfig,
                                                                  dim,
                                                                  bMonoChrome);
                m_Visualisations[VisualisationMode::Oscilloscope].push_back(std::move(pVis));
            }
        }

        const auto visCount{ m_Visualisations[visMode].size() };
        if (visCount > 0) {
            ++modeActiveCount;
            if (visCount > visMaxIndex) { visMaxIndex = visCount; }
            if (startConfig.m_bRememberLast && startConfig.m_LastMode == visMode) {
                m_eCurrentVisMode = visMode;
                m_CurrentVisIndex[visMode] = startConfig.m_LastIndex;
            } else {
                m_CurrentVisIndex[visMode] = 0;
            }
        } else {
            m_CurrentVisIndex[visMode] = -1;
        }
    }

    //VU vis
    {
        namespace VU = Visualisation::VUMeter;
        constexpr const auto visMode{ VisualisationMode::VUMeter };
        for (const auto v : VUMeterType{}) {
            const auto& vuConfig = Config::VUMeterConfig::get(v);
            if (vuConfig.m_bEnabled) {
                auto pVis = VU::IVUMeter::MakeVisualisation(v,
                                                            vuConfig,
                                                            dim,
                                                            bMonoChrome);
                m_Visualisations[VisualisationMode::VUMeter].push_back(std::move(pVis));
            }
        }

        const auto visCount{ m_Visualisations[visMode].size() };
        if (visCount > 0) {
            ++modeActiveCount;
            if (visCount > visMaxIndex) { visMaxIndex = visCount; }
            if (startConfig.m_bRememberLast && startConfig.m_LastMode == visMode) {
                m_eCurrentVisMode = visMode;
                m_CurrentVisIndex[visMode] = startConfig.m_LastIndex;
            } else {
                m_CurrentVisIndex[visMode] = 0;
            }
        } else {
            m_CurrentVisIndex[visMode] = -1;
        }
    }

    //Track Info vis
    {
        namespace Text = Visualisation::Text;
        constexpr const auto visMode{ VisualisationMode::TrackDetails };
        for (const auto page : TrackDetailsType{}) {
            const auto tdConfig = Config::TrackDetailsConfig::get(page);
            if (tdConfig.m_bEnabled) {
                auto pVis = Text::ITrackDetails::MakeVisualisation(page, tdConfig,
                                                                   dim, bMonoChrome);
                m_Visualisations[VisualisationMode::TrackDetails].push_back(std::move(pVis));
            }
        }

        const auto visCount{ m_Visualisations[visMode].size() };
        if (visCount > 0) {
            ++modeActiveCount;
            if (visCount > visMaxIndex) { visMaxIndex = visCount; }
            m_pTrackDetails = m_Visualisations[visMode][0];
            if (startConfig.m_bRememberLast && startConfig.m_LastMode == visMode) {
                m_eCurrentVisMode = visMode;
                m_CurrentVisIndex[visMode] = startConfig.m_LastIndex;
            } else {
                m_CurrentVisIndex[visMode] = 0;
            }
        } else {
            m_CurrentVisIndex[visMode] = -1;
            const auto ePage = TrackDetailsType::begin();
            const auto tdConfig = Config::TrackDetailsConfig::get(ePage);
            auto pVis = Text::ITrackDetails::MakeVisualisation(ePage, tdConfig,
                                                               dim, bMonoChrome);
            m_pTrackDetails = std::move(pVis);
        }
    }

    m_bHaveVisualisations = (modeActiveCount > 0);
    m_MaxVisIndex         = static_cast<random_value_type>(visMaxIndex);

    //Set and activate the chosen visualisation
    if (m_eCurrentVisMode == VisualisationMode::ModeError) {
        m_eCurrentVisMode = VisualisationMode::Mode1;
    }
    SetCurrentVisualisation(m_eCurrentVisMode, m_CurrentVisIndex[m_eCurrentVisMode]);
    m_bCurrentChanged = true;

    //Set initial LCD settings (done here so it correctly picks up config changes)
    m_pDisplay->SetForceDisplayPriority(DisplayConfig().m_bForceToForeground);
    m_pDisplay->SetDisplayPriority(::LCD::Priority::Foreground);
    m_pDisplay->SetPreferVSync(DisplayConfig().m_bVSync);

    UpdateWallpaper(true);
}

//------------------------------------------------------------------------------

void VisualisationManager::UninitialiseVisualisations() {
    if (m_pCurrent) {
        m_pCurrent->Deactivate();
        m_pCurrent.reset();
    }

    if (m_pTrackDetails) {
        m_pTrackDetails->Deactivate();
        m_pTrackDetails.reset();
    }

    for (auto& v : m_Visualisations) { v.clear(); }

    if (m_pCanvas) {
        m_pCanvas->RemoveWallpaper();
    }
}

//------------------------------------------------------------------------------

void VisualisationManager::UpdateConfig() noexcept {
    //Delay the actual update till we process a new frame
    m_bConfigChanged.Set(true);
}

//------------------------------------------------------------------------------

void VisualisationManager::ActivateVisualisation(visualisation_pointer pVis) {
    if (!pVis) {
        pVis = m_pTrackDetails;
        assert(pVis);
    }

    if (m_pCurrent) {
        if (m_pCurrent->Is(pVis->GetMode(), pVis->GetIndex())) {
            return;
        } else {
            m_bCurrentChanged = true;
            m_pCurrent->Deactivate();
        }
    }

    m_pCurrent = pVis;
    assert(m_pCurrent);

    using size_type = typename Visualisation::RequestParams::size_type;
    const auto nDefaultSampleCount{ static_cast<size_type>(m_pCanvas->GetWidth()) };
    m_RequestParams = Visualisation::RequestParams{ nDefaultSampleCount };

    auto& audioDataManager{ GetAudioDataManager() };
    m_pCurrent->Activate(m_RequestParams, audioDataManager);

    const auto clearColor = ::Color::color_cast<::Color::Color3ub>(m_RequestParams.ClearColor);
    m_pCanvas->SetBGColor(RGB(clearColor.r(), clearColor.g(), clearColor.b()));
    const auto primColor = ::Color::color_cast<::Color::Color3ub>(m_RequestParams.PrimaryColor);
    m_pCanvas->SetFGColor(RGB(primColor.r(), primColor.g(), primColor.b()));
    //audioDataManager.Update(m_RequestParams);
    const auto flags = m_pCurrent->GetFlags();
    float fTickHz = 30.f;
    if (flags & Visualisation::VisualisationFlags::LowFrameRate) {
        fTickHz = 15.f;
    }

    float fUpdateHz = fTickHz;
    if (flags & Visualisation::VisualisationFlags::PreferFrequentUpdates) {
        fUpdateHz = fTickHz;
    } else {
        fUpdateHz = fTickHz * .5f;
    }

    SetFrequency(fTickHz, fUpdateHz);
}

//------------------------------------------------------------------------------

//Update vis helper - activates and deactivates as required
void VisualisationManager::SetCurrentVisualisation(int iMode,
                                                   int iIndex) {
    if (!m_bHaveVisualisations) {
        if (!m_pCurrent) { m_pCurrent = m_pTrackDetails; }
        return;
    }

    if (!VisualisationMode::valid(iMode) || m_Visualisations[iMode].empty()) {
        iMode %= VisualisationMode::count();
        if (iMode < 0) { iMode += VisualisationMode::count(); }
        assert(VisualisationMode::valid(iMode));

        // NOTE: This loop can't be infinite if m_bHaveVisualisations is true
        while (m_Visualisations[iMode].empty()) {
            ++iMode %= VisualisationMode::count();
            assert(VisualisationMode::valid(iMode));
        }
    }

    const auto eMode{ VisualisationMode::to_enum(iMode) };

    assert(VisualisationMode::valid(eMode));
    assert(!m_Visualisations[eMode].empty());

    const auto visCount = static_cast<int>(m_Visualisations[eMode].size());
    iIndex %= visCount;
    if (iIndex < 0) { iIndex += visCount; }
    assert(iIndex >= 0 && iIndex < visCount);

    if (m_pCurrent && m_pCurrent->Is(eMode, iIndex)) {
        return;
    }

    m_eCurrentVisMode = eMode;
    m_CurrentVisIndex[eMode] = iIndex;

    if (m_bCurrentIsPopup) {
        return;
    }

    ActivateVisualisation(m_Visualisations[eMode][iIndex]);
}

//------------------------------------------------------------------------------

// Deal with button presses
//
// NOTE:
//  - This is not the best way of dealing with buttons; its tightly coupled
//    to the display and the number of buttons it has and is not easily
//    user configurable. Would be better to do something akin to the old
//    Quake style "bind" where button presses are bound to a function that
//    does whatever work the user wants, the display would process the buttons
//    and call the appropriate command for each. This would be much cleaner
//    and would more easily support alternative displays.
void VisualisationManager::ProcessButtons() {
    const auto buttons = m_pDisplay->GetButtons();
    if (m_LastButtonState == buttons) { return; }

    const auto changedBtns{ buttons & ~m_LastButtonState };
    m_LastButtonState = buttons;

    bool bChanged{ false };
    auto mode  = m_eCurrentVisMode;
    auto index = m_CurrentVisIndex[m_eCurrentVisMode];
    // Only process a single button at a time
    if ((changedBtns & ::LCD::ButtonState::Button0) == ::LCD::ButtonState::Button0) {
        bChanged = true;
        if (mode == VisualisationMode::SpectrumAnalyser) {
            ++index;
        } else {
            mode  = VisualisationMode::SpectrumAnalyser;
            index = m_CurrentVisIndex[mode];
        }
    } else if ((changedBtns & ::LCD::ButtonState::Button1) == ::LCD::ButtonState::Button1) {
        bChanged = true;
        if (mode == VisualisationMode::Oscilloscope) {
            ++index;
        } else {
            mode  = VisualisationMode::Oscilloscope;
            index = m_CurrentVisIndex[mode];
        }
    } else if ((changedBtns & ::LCD::ButtonState::Button2) == ::LCD::ButtonState::Button2) {
        bChanged = true;
        if (mode == VisualisationMode::VUMeter) {
            ++index;
        } else {
            mode  = VisualisationMode::VUMeter;
            index = m_CurrentVisIndex[mode];
        }
    } else if ((changedBtns & ::LCD::ButtonState::Button3) == ::LCD::ButtonState::Button3) {
        bChanged = true;
        if (mode == VisualisationMode::TrackDetails) {
            ++index;
        } else {
            mode  = VisualisationMode::TrackDetails;
            index = m_CurrentVisIndex[mode];
        }
    } else if ((changedBtns & ::LCD::ButtonState::Up) == ::LCD::ButtonState::Up) {
        bChanged = true;
        ++index;
    } else if ((changedBtns & ::LCD::ButtonState::Down) == ::LCD::ButtonState::Down) {
        bChanged = true;
        --index;
    } else if ((changedBtns & ::LCD::ButtonState::Right) == ::LCD::ButtonState::Right) {
        bChanged = true;
        mode = (mode < VisualisationMode::last())
            ? (mode + 1)
            : VisualisationMode::first();
        index = m_CurrentVisIndex[mode];
    } else if ((changedBtns & ::LCD::ButtonState::Left) == ::LCD::ButtonState::Left) {
        bChanged = true;
        mode = (mode > 0)
            ? (mode - 1)
            : VisualisationMode::last();
        index = m_CurrentVisIndex[mode];
    }

    if (bChanged) {
        // Button presses override pop-ups, but changing
        // from the previous visualisation "feels"(!) wrong,
        // so just make it an override.
        if (std::exchange(m_bCurrentIsPopup, false)) {
            SetCurrentVisualisation(m_eCurrentVisMode,
                                    m_CurrentVisIndex[m_eCurrentVisMode]);
        } else {
            SetCurrentVisualisation(mode, index);
        }
    }
}

//------------------------------------------------------------------------------

// Process the display state
void VisualisationManager::SetDisplayPriority() noexcept {
    //No changing foreground state if we are in background mode
    if (DisplayConfig().m_bBackgroundMode) {
        return;
    }

    const auto currentPriority{ m_pDisplay->GetDisplayPriority() };
    if (GetAudioDataManager().GetPlayState() == Audio::PlayState::Playing) {
        const auto defaultPriority{
            DisplayConfig().m_bBackgroundMode
                ? LCD::Priority::Background
                : LCD::Priority::Foreground
        };

        auto priority{ currentPriority };

        if (DisplayConfig().m_bBackgroundOnPause &&
            currentPriority == LCD::Priority::NoShow) {
            priority = defaultPriority;
        }

        if (m_bTrackChangeAlert) {
            if (GetAudioDataManager().HasTrackChanged()) {
                m_TrackChangeAlert.Start();
                priority = LCD::Priority::Alert;
            } else if (m_TrackChangeAlert.IsRunning()) {
                if (m_TrackChangeAlert.HasElapsed()) {
                    m_TrackChangeAlert.Stop();
                    priority = defaultPriority;
                } else {
                    priority = LCD::Priority::Alert;
                }
            }
        }

        if (priority != currentPriority) {
            m_pDisplay->SetForceDisplayPriority(DisplayConfig().m_bForceToForeground);
            m_pDisplay->SetDisplayPriority(priority);
        }
    } else {
        if (DisplayConfig().m_bBackgroundOnPause &&
            currentPriority != LCD::Priority::NoShow) {
            m_pDisplay->SetForceDisplayPriority(false);
            m_pDisplay->SetDisplayPriority(LCD::Priority::NoShow);
        }
    }
}

//------------------------------------------------------------------------------

// Process automatic visualisation changes
void VisualisationManager::SetVisualisation() {
    if (m_bTrackChangePopup) {
        if (GetAudioDataManager().HasTrackChanged()) {
            m_TrackChangePopup.Start();
            ActivateVisualisation(m_pTrackDetails);
            m_bCurrentIsPopup = true;
        } else {
            if (m_TrackChangePopup.IsRunning()) {
                if (m_TrackChangePopup.HasElapsed()) {
                    // Button presses will override pop-up
                    if (m_bCurrentIsPopup) {
                        SetCurrentVisualisation(m_eCurrentVisMode,
                                                m_CurrentVisIndex[m_eCurrentVisMode]);
                        m_bCurrentIsPopup = false;
                    }
                    m_TrackChangePopup.Stop();
                }
            }
        }
    }

    if (m_bAutoChange) {
        if (m_AutoChange.IsRunning()) {
            if (m_AutoChange.HasElapsed()) {
                const auto mode { m_RandomMode.get() };
                const auto index{ m_RandomIndex.get(0, m_MaxVisIndex) };
                SetCurrentVisualisation(mode, index);
                m_AutoChange.Start();
            }
        } else {
            m_AutoChange.Start();
        }
    } else {
        m_AutoChange.Stop();
    }
}

//------------------------------------------------------------------------------

void VisualisationManager::UpdateWallpaper(bool bForce) {
    if (!m_pCanvas->IsColor()) { return; }

    const auto& cfgWallpaper{ CanvasConfig().m_Wallpaper };
    switch (cfgWallpaper.m_Mode) {
        case WallpaperMode::File: {
            if ((!m_pCanvas->HasWallpaper()) || bForce) {
                const auto strFile{ cfgWallpaper.m_File };
                auto image = ::Image::ImageLoader::Load(strFile.c_str());
                if (image) {
                    m_pCanvas->SetWallpaper(image, cfgWallpaper.m_bStretchToFit);
                }
            }
            break;
        }
        case WallpaperMode::AlbumArt: {
            if (GetAudioDataManager().HasAlbumArtChanged() || bForce) {
                auto image = GetAudioDataManager().GetAlbumArt();
                if (image) {
                    m_pCanvas->SetWallpaper(image, cfgWallpaper.m_bStretchToFit);
                } else {
                    m_pCanvas->RemoveWallpaper();
                }
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------

VisualisationManager::WorkerStatus VisualisationManager::OnUpdate() {
    GetAudioDataManager().Update(m_RequestParams);

    if (m_pCurrent && ((!DisplayConfig().m_bBackgroundMode) || m_bCurrentIsPopup)) {
        m_pCurrent->Update(GetAudioDataManager());
    }
    return WorkerStatus::Continue;
}

//------------------------------------------------------------------------------

//Called once per tick (i.e. every 1/Frequency seconds)
VisualisationManager::WorkerStatus VisualisationManager::OnTick(float fInterp) {
    if (!m_pDisplay || !m_pCanvas || !m_pDataManager) { return WorkerStatus::Quit; }

    if (m_bConfigChanged.Exchange(false)) {
        Uninitialise();
        Initialise();
    }

    if (!m_pCurrent) { return WorkerStatus::Quit; }

    //Deal with user input
    ProcessButtons();

    //Process and update events
    SetDisplayPriority();
    SetVisualisation();

    if (std::exchange(m_bCurrentChanged, false)) {
        OnUpdate();
    }

    GetAudioDataManager().StartFrame();

    if (CanvasConfig().m_Wallpaper.m_Mode != WallpaperMode::None) {
        UpdateWallpaper();
    }

    m_pCanvas->StartFrame();

    if ((!DisplayConfig().m_bBackgroundMode) || m_bCurrentIsPopup) {
        for (const auto pass : RenderPass{}) {
            m_pCanvas->StartPass(pass);
            m_pCurrent->Draw(pass, GetAudioDataManager(), fInterp);
            m_pCanvas->EndPass(pass);
        }
    }

    void* pData = m_pCanvas->EndFrame();

    //Send the update to the LCD, if using VSync this may take a long time to return
    if (pData) { m_pDisplay->Update(pData); }

    GetAudioDataManager().EndFrame();

    return WorkerStatus::Continue;
}
