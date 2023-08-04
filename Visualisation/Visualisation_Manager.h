#pragma once
#ifndef GUID_4A604C90_698C_4C0E_B360_CE67298E7C28
#define GUID_4A604C90_698C_4C0E_B360_CE67298E7C28
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
#include "logger.h"
//--------------------------------------

//--------------------------------------
//
#include "Audio_DataManager.h"
#include "Visualisation/Visualisation.h"
#include "LCD/LCD.h"
#include "Canvas.hpp"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Thread/Thread_Worker.h"
#include "Windows/Thread/Thread_CriticalSection.h"
#include "Windows/Windows_StopWatch.h"
//--------------------------------------

//--------------------------------------
//
#include "Config/Config_General.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/Random.h"
//--------------------------------------

//******************************************************************************
// VisualisationManager
//******************************************************************************
class VisualisationManager final :
    public ::util::singleton<VisualisationManager>,
    public ::Windows::Thread::Worker {
private:
    using this_class  = VisualisationManager;
    using base_class  = ::Windows::Thread::Worker;

    using TimedEvent = ::Windows::TimedEvent;

public:
    using random_value_type     = int;
    using random_distribution   = std::uniform_int_distribution<random_value_type>;
    using random_generator      = ::util::random_number_generator<random_value_type, random_distribution>;

    using visualisation         = Visualisation::IVisualisation;
    using visualisation_pointer = typename visualisation::pointer_type;
    using visualisation_buffer  = std::vector<visualisation_pointer>;
    using visualisation_pages   = std::array<visualisation_buffer, VisualisationMode::count()>;

    using display            = ::LCD::ILCD;
    using display_pointer    = typename display::pointer_type;
    using display_desc       = ::LCD::Device::Desc;
    using display_color_type = ::LCD::Device::Type;
    using button_state       = ::LCD::ButtonState;

    using canvas         = ::Windows::Canvas;
    using canvas_pointer = typename canvas::pointer_type;

    using data_manger         = ::Audio::IAudioDataManager;
    using data_manger_pointer = typename data_manger::pointer_type;

public:
    VisualisationManager(singleton_constructor_tag /*tag*/) {};

    ~VisualisationManager() noexcept {
        Uninitialise();
    }

    bool Initialise  ();
    void Uninitialise() noexcept;

    void UpdateConfig() noexcept;

public:
    void SetDisplay    (display_pointer     display) noexcept { m_pDisplay     = display; }
    void SetDataManager(data_manger_pointer manager) noexcept { m_pDataManager = manager; }

public:
    decltype(auto) GetDeviceDesc   () const noexcept { assert(m_pDisplay); return m_pDisplay->GetDeviceDesc(); }
    decltype(auto) GetWidth        () const noexcept { assert(m_pDisplay); return m_pDisplay->GetWidth(); }
    decltype(auto) GetHeight       () const noexcept { assert(m_pDisplay); return m_pDisplay->GetHeight(); }
    decltype(auto) GetBytesPerPixel() const noexcept { assert(m_pDisplay); return m_pDisplay->GetBitsPerPixel() * 8; }
    decltype(auto) GetBitsPerPixel () const noexcept { assert(m_pDisplay); return m_pDisplay->GetBitsPerPixel(); }
    decltype(auto) GetButtonCount  () const noexcept { assert(m_pDisplay); return m_pDisplay->GetButtonCount(); }
    decltype(auto) GetDeviceType   () const noexcept { assert(m_pDisplay); return m_pDisplay->GetDeviceType(); }

    decltype(auto) IsColor     () const noexcept { assert(m_pDisplay); return m_pDisplay->GetBitsPerPixel() >  8; }
    decltype(auto) IsMonochrome() const noexcept { assert(m_pDisplay); return m_pDisplay->GetBitsPerPixel() <= 8; }

    //Following are currently aliases for above -
    // eventually should be for systems with multiple
    // LCD displays
    decltype(auto) HasColor     () const noexcept { return IsColor(); }
    decltype(auto) HasMonochrome() const noexcept { return IsMonochrome(); }

    decltype(auto) GetCurrentVisMode () const noexcept { return m_eCurrentVisMode; }
    decltype(auto) GetCurrentVisIndex() const noexcept { return m_CurrentVisIndex[m_eCurrentVisMode]; }

protected:
    virtual WorkerStatus OnTick  (float fInterp) override;
    virtual WorkerStatus OnUpdate()              override;

private:
    void InitialiseVisualisations();
    void UninitialiseVisualisations();

    void ProcessButtons();

    void SetCurrentVisualisation(int iMode,
                                 int iIndex);

    void ActivateVisualisation(visualisation_pointer pVis);

    void SetDisplayPriority() noexcept;
    void SetVisualisation  ();
    void UpdateWallpaper   (bool bForce = false);

    const auto& GetAudioDataManager() const noexcept {
        assert(m_pDataManager);
        return *m_pDataManager;
    }

    auto& GetAudioDataManager() noexcept {
        assert(m_pDataManager);
        return *m_pDataManager;
    }

private:
    random_generator  m_RandomMode { 0, VisualisationMode::count() };
    random_generator  m_RandomIndex{ };
    random_value_type m_MaxVisIndex{ 0 };

    data_manger_pointer m_pDataManager   {};
    canvas_pointer      m_pCanvas        { nullptr };
    display_pointer     m_pDisplay       {};
    button_state        m_LastButtonState{ button_state::None };

    visualisation_pages   m_Visualisations{};
    visualisation_pointer m_pCurrent      {};
    visualisation_pointer m_pTrackDetails {};

    bool m_bHaveVisualisations{ false };
    bool m_bCurrentIsPopup    { false };
    bool m_bCurrentChanged    { true };

    int               m_CurrentVisIndex[VisualisationMode::count()]{ 0 };
    VisualisationMode m_eCurrentVisMode{ VisualisationMode::Mode1 };

    bool       m_bAutoChange      { false };
    TimedEvent m_AutoChange       {};
    bool       m_bTrackChangePopup{ false };
    TimedEvent m_TrackChangePopup {};
    bool       m_bTrackChangeAlert{ false };
    TimedEvent m_TrackChangeAlert {};

private:
    constexpr const auto& Config() const noexcept { return m_Config; }

    constexpr const auto& DisplayConfig      () const noexcept { return Config().m_Display; }
    constexpr const auto& CanvasConfig       () const noexcept { return Config().m_Canvas; }
    constexpr const auto& VisualisationConfig() const noexcept { return Config().m_Visualisation; }

    Visualisation::RequestParams m_RequestParams{};
    Config::GeneralConfig        m_Config       {};
private:
    ::Windows::Thread::CriticalSectionProtectedVariableT<bool> m_bConfigChanged{ false };
}; // class VisualisationManager final

#endif // GUID_4A604C90_698C_4C0E_B360_CE67298E7C28