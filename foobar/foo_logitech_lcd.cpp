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
#include "foobar/foobar_sdk.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/foobar_sdk.h"
#include "foobar/foobar_audio_data_manager.h"
#include "foobar/Config/foobar_config_manager.h"
//--------------------------------------

//--------------------------------------
//
#include "LCD/LCD.h"
#include "LCD/LogitechLCD.h"
//--------------------------------------

//--------------------------------------
//
#include "Visualisation/Visualisation_Manager.h"
#include "Util/ScopeExit.h"
//--------------------------------------

//--------------------------------------
// spdlog
#include <spdlog/sinks/base_sink.h>
#include <mutex>
//--------------------------------------

namespace {
    //**************************************************************************
    // LOGITECH_APP_NAME
    //**************************************************************************
    constexpr const wchar_t* LOGITECH_APP_NAME{ L"Foobar2000 LCD Display & Visualisations" };

    //**************************************************************************
    // foo_logitech_lcd_worker
    //**************************************************************************
    class foo_logitech_lcd_worker final : public pfc::thread {
    public:
        ~foo_logitech_lcd_worker() noexcept {
            try {
                if (auto pVisualisationManager = VisualisationManager::instance_peek()) {
                    pVisualisationManager->Stop();
                }
                waitTillDone();
                VisualisationManager::destroy();
            } catch(...) {
                assert(false);
            }
        }

    protected: //pfc::thread
        virtual void threadProc() override {
            if (core_api::is_shutting_down()) { return; }

            // Disabling this exception catching wrapper in debug causes
            // Visual Studio to break if an exception is thrown, making
            // it easier to track bugs. In release the logging of the
            // issue is likely more helpful.
    #if (!defined(DEBUG) && !defined(_DEBUG))
            try {
    #endif
                auto pVisualisationManager = VisualisationManager::instance();
                const auto _destroy{ util::at_scope_exit([]() noexcept { VisualisationManager::destroy(); }) };
                if (pVisualisationManager->Initialise()) {
                    const auto _uninitialise{
                        util::at_scope_exit(
                            [&]() noexcept {
                                if (auto pCfgMgr = Config::IConfigManager::instance_peek()) {
                                    auto config = pCfgMgr->GetGeneralConfig();
                                    if (config.m_Visualisation.m_Start.m_bRememberLast) {
                                        config.m_Visualisation.m_Start.m_LastMode  = pVisualisationManager->GetCurrentVisMode();
                                        config.m_Visualisation.m_Start.m_LastIndex = pVisualisationManager->GetCurrentVisIndex();
                                        pCfgMgr->SetGeneralConfig(config);
                                    }
                                }
                                pVisualisationManager->Uninitialise();
                            }
                        )
                    };

                    auto callbackHandle = Config::IConfigManager::instance()->RegisterUpdateCallback(
                        []() noexcept {
                            if (auto pVisMan = VisualisationManager::instance_peek()) {
                                if (pVisMan->IsRunning()) { pVisMan->UpdateConfig(); }
                            }
                        }
                    );

                    pVisualisationManager->Run();
                } else {
                    SPDLOG_ERROR("Could not initialise LCD. Plugin will be unavailable.");
                }
    #if (!defined(DEBUG) && !defined(_DEBUG))
            } catch (const std::exception& e) {
                SPDLOG_ERROR("Unhandled std::exception in worker thread. [%s]", e.what());
            } catch (...) {
                SPDLOG_ERROR("Unhandled exception in worker thread. No further information.");
            }
    #endif
        }
    }; // class foo_logitech_lcd_worker final

    //**************************************************************************
    // foobar_console_sink
    //
    //      Adapted from spdlog::sinks::msvc_sink
    //**************************************************************************
    template<typename MutexT>
    class foobar_console_sink final : public ::spdlog::sinks::base_sink<MutexT> {
    private:
        using base_type = ::spdlog::sinks::base_sink<MutexT>;
        using this_type = foobar_console_sink<MutexT>;

    public:
        using log_msg_type = ::spdlog::details::log_msg;
        using buffer_type  = ::spdlog::memory_buf_t;

    public:
        foobar_console_sink() noexcept = default;

    protected:
        void sink_it_(const log_msg_type& msg) override {
            buffer_type formatted;
            base_type::formatter_->format(msg, formatted);
            formatted.push_back('\0');

            // Output to foobar console
            switch (msg.level) {
            case ::logger::level_t::critical: [[fallthrough]];
            case ::logger::level_t::err:
                ::console::error(formatted.data());
                break;
            case ::logger::level_t::warn:
                ::console::warning(formatted.data());
                break;
            case ::logger::level_t::info:
                ::console::info(formatted.data());
                break;
            default:
                ::console::print(formatted.data());
                break;
            }
        }

        // No explicit flush for foobar console
        void flush_() noexcept override {}
    };

    //----------------------------------

    using foobar_console_sink_mt = foobar_console_sink<std::mutex>;
    using foobar_console_sink_st = foobar_console_sink<::spdlog::details::null_mutex>;

    //**************************************************************************
    // foo_logitech_lcd_initquit
    //**************************************************************************
    class foo_logitech_lcd_initquit : public initquit {
    public:
        // class should avoid using constructors/destructors as at call time
        // the state of the DLL is not likely to be useful (e.g. calls to
        // Windows API will often fail).
    protected:
        ~foo_logitech_lcd_initquit() = default;

    public: //initquit
        virtual void on_init() override {
            try {
                ::spdlog::level::level_enum logLevel = ::spdlog::level::warn;
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
                logLevel = ::spdlog::level::debug;
#endif
                ::logger::initialise("foo_logitech_lcd",
                                     logLevel,
                                     ::std::make_shared<foobar_console_sink_mt>(),
                                     "[%n] %v");
            } catch (const ::std::exception& e) {
                ::console::complain("[foo_logitech_lcd] Failed to initialise SPDLOG.", e.what());
            }

            Windows::SetInstanceHandle(core_api::get_my_instance());

            ::foobar::Config::ConfigManager::initialise();

            ::foobar::foobar_audio_data_manager::initialise();
            auto pAudioDataManager = ::foobar::foobar_audio_data_manager::instance();
            if (!(pAudioDataManager && pAudioDataManager->IsValid())) {
                SPDLOG_ERROR("AudioManager not valid - plugin requires foobar version 9.5.2 or greater. Plugin will be unavailable.");
                return;
            }

            m_CallbackHandle = Config::IConfigManager::instance()->RegisterUpdateCallback(
                []() {
                    if (auto pAudioMgr = Audio::IAudioDataManager::instance_peek()) {
                        pAudioMgr->UpdateConfig();
                    }
                }
            );

            ::LCD::Logitech::LogitechLCD::initialise(LOGITECH_APP_NAME);

            auto pLCD = ::LCD::Logitech::LogitechLCD::instance();
            if (!pLCD) {
                SPDLOG_ERROR("Failed to connect to Logitech LCD.");
                return;
            }

            // The LCD API sends a notification on initialisation, but
            // it's sent in another thread, so this causes issues
            // TODO: Proper synchronisation in callback!!
            //start_display();

            pLCD->SetNotificationCallback(
                [&](::LCD::Device::Notification::What what,
                    void* /*pdata*/,
                    const ::LCD::ILCD& /*lcd*/) {
                    on_lcd_notify(what);
                }
            );
        }

        virtual void on_quit() noexcept override {
            stop_display();
            ::LCD::ILCD::destroy();
            m_CallbackHandle.reset();
            ::Audio::IAudioDataManager::destroy();
        }

    private:
        void start_display() {
            if (m_pWorker) { return; }
            if (::LCD::ILCD::expired()) { return; }

            auto pLCD = ::LCD::ILCD::instance_peek();
            if (pLCD->Connected() || pLCD->Connect(::LCD::Device::Type::Unknown)) {
                m_pWorker.reset(new foo_logitech_lcd_worker{});
                m_pWorker->start();
            }
        }

        void stop_display() noexcept {
            if (m_pWorker) { m_pWorker.reset(); }
            auto pLCD = ::LCD::ILCD::instance_peek();
            if (pLCD && pLCD->Connected()) { pLCD->Disconnect(); }
        }

        void on_lcd_notify(::LCD::Device::Notification::What what) {
            switch (what) {
                case ::LCD::Device::Notification::DeviceConnected:
                    [[fallthrough]];
                case ::LCD::Device::Notification::AppletEnabled:
                    SPDLOG_INFO("LCD Connected, staring plugin...");
                    start_display();
                    break;
                case ::LCD::Device::Notification::DeviceDisconnected:
                    [[fallthrough]];
                case ::LCD::Device::Notification::AppletDisabled:
                    SPDLOG_INFO("LCD Disconnected. Plugin will be unavailable till connection is re-established.");
                    stop_display();
                    break;
            }
        }

    private:
        Config::IConfigManager::CallbackHandle m_CallbackHandle{};
        std::shared_ptr<foo_logitech_lcd_worker> m_pWorker{};
    }; // class foo_logitech_lcd_initquit

    //----------------------------------

    initquit_factory_t<foo_logitech_lcd_initquit> g_InitQuit;
} // namespace <anonymous>

//******************************************************************************
//******************************************************************************
// foobar plugin declarations
//******************************************************************************
//******************************************************************************
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#	define VERSION_STR "1.0.0 (Resurrection) [DEVELOPEMENT VERSION - DO NOT DISTRIBUTE]"
#else
#	define VERSION_STR "1.0.0 (Resurrection)"
#endif

#ifndef _PREFAST_
DECLARE_COMPONENT_VERSION(\
                          "Logitech LCD Display", \
                          VERSION_STR,
                          "Logitech LCD Display [foo_logitech_lcd] v" VERSION_STR "\n\
\n\
A component enabling track information and visualisations to be displayed on a \
range of Logitech LCD products.\n\
\n\
Uses the following third party libraries:\n\
    * GLew\n\
    * Logitech LCD API\n\
\n\
For all versions of the component, the dynamic library (.dll) is distributed \
under the Boost Software License: (http://www.boost.org/LICENSE_1_0.txt)\n\
\n\
The source code for the component is available from \
https://github.com/ectotropic and is distributed under the GNU Lesser General \
Public License.\n\
\n\
Version History\n\
===============\n\
\n\
1.0.0 (Resurrection) - Open Sourced\n\
    * New comprehensively updated version, finally released as Open Source.\n\
    * Fixed many bugs and issues.\n\
    * Improved accuracy and correctness of many calculations.\n\
    * Improved general performance and robustness.\n\
    * Removed dependence on multiple external libraries.\n\
    * Updated to more modern external libraries where possible.\n\
\n\
0.5.5 -  Bug fixes\n\
    * [FIXED]	Crash on exit\n\
    * [FIXED]	Small performance issue\n\
\n\
0.5.4 - Minor Update\n\
    * [FIXED]	Handling of device connections & disconnections\n\
    * [FIXED]	Text overlapping progress bar in some cases\n\
    * [FIXED]	Rare OpenGL crash\n\
    * [CHANGED]	Better handling of windows messages to hardware window\n\
\n\
0.5.3 - Minor Update\n\
    * [NEW]		Progress bar timings can be replaced by system time and date [requested]\n\
    * [NEW]		When using background pictures it is now possible to have 'cut-outs' (or background clears) for text (improves legibility)\n\
    * [CHANGED]	updated to latest SDK (2010-05-21)\n\
    * [CHANGED]	'About' text - also added change log to about text (so those who update via foobar updater can find out what has changed)\n\
    * [CHANGED]	Faster loading of OpenGL textures\n\
    * [FIXED]	Crash when loading album art and track changes\n\
    * [FIXED]	Album art not being cleared on screen correctly in certain situations\n\
    * [FIXED]	Minor bugs and inconsistencies\n\
\n\
0.5.2 - Bug fixes\n\
    * [FIXED]	Non-hardware accelerated Spectrum Analyzer 1 being inverted\n\
    * [FIXED]	Progress bar not resetting on stop\n\
    * [FIXED]	Track info time selection combo-boxes text not corresponding to action\n\
\n\
0.5.1 - Minor Update\n\
    * [NEW]		Added additional VU preferences (fixes issue with left VU meter not matching right)\n\
    * [CHANGED]	updated to latest SDK (2010-01-19)\n\
    * [TWEAKED]	\"Apply\" button behavior for edit boxes\n\
    * [FIXED]	Laggy dialog boxes\n\
\n\
0.5.0 - Major Update\n\
    * [NEW]		foobar2000 v1.0.0 support\n\
    * [NEW]		Hardware acceleration support\n\
    * [NEW]		Background mode (only displays when song changes)\n\
    * [NEW]		Trail effect for color displays\n\
    * [NEW]		Exposed more options for configuration\n\
    * [CHANGED]	Improved text preferences dialogs\n\
    * [CHANGED]	Album art now comes from foobar and can be set to be correct aspect ratio\n\
    * [CHANGED]	Attempted to make spectrum analyser more reactive\n\
    * [CHANGED]	Major rewrite of preferences and configuration code\n\
    * [CHANGED]	foobar image from jpeg to raw data\n\
    * [REMOVED]	Support for older versions of foobar2000\n\
    * [FIXED]	Time display occasionally showing 0:60 seconds\n\
    * [FIXED]	Multiple crashes\n\
\n\
0.4.0 - Bug fixes\n\
    * [FIXED]	Multiple crashes\n\
    * [FIXED]	Dynamic track information\n\
    * [FIXED]	Font occasionally not being correctly set\n\
    * [FIXED]	All options now update instantly\n\
    * [FIXED]	Block Stereo Spectrum block count not being correctly used\n\
    * [TWEAKED]	Track info pages - scrolling should be much improved\n\
    * [TWEAKED]	Progress bar\n\
    * [REMOVED]	Excess (unused) code\n\
\n\
0.3.6 - Bug fixes\n\
    * [FIXED]	Text colorisation\n\
\n\
0.3.5 - Updates & fixes\n\
    * [NEW]		Stereo Spectrum visualisations\n\
    * [NEW]		Peak markers for both VU and Spectrum visualisations\n\
    * [NEW]		Added an option to allow a gap line between spectrum lines (block spectrum only)\n\
    * [NEW]		Yet more configuration options\n\
    * [CHANGED] Scaling and offset factors - now unified across color and monochrome displays\n\
    * [CHANGED] Spectrum calculations - look better now\n\
    * [FIXED]	Wallpaper OutOfMemory error\n\
    * [FIXED]	Wallpaper not displaying on track info pages\n\
    * [FIXED]	Config options not saving correctly in certain circumstances\n\
    * [FIXED]	Enabling/disabling wallpaper now applies immediately (no track change required)\n\
\n\
0.3.1 - Bug fixes\n\
    * [FIXED]	Preferences crash bug (removed 3rd party library that was causing problems)\n\
    * [FIXED]	Threading synchronisation in certain cases\n\
    * [FIXED]	Error display (removed some output not required in release builds)\n\
\n\
0.3.0 - Major update\n\
    * [NEW]		Additional track info pages\n\
    * [NEW]		Text handling - including vertical scrolling support - text quality on color displays should be improved\n\
    * [NEW]		Configuration options (including control of scrolling parameters)\n\
    * [NEW]		Error handling - improved reporting of errors, many additional error checks\n\
    * [NEW]		Many optimisations (CPU usage should be much better)\n\
    * [NEW]		New source layout and (better) helper functions (minor code refactor)\n\
    * [CHANGED]	Removed dependencies on DLLs - VCRedist should no longer be required. (URL remains in case something was missed, or using an older version)\
    * [CHANGED]	Removed use of Logitech EZ LCD wrapper\
    * [CHANGED] Progress bar height detached from font height\n\
    * [FIXED]	Conflicting libraries\n\
    * [FIXED]	Handling of multiple LCD screen types (now will only use one type of screen at any one time)\n\
    * [FIXED]	Font weight not being passed through correctly (i.e. bold)\n\
\n\
0.2.6 - Bug fix\n\
    * [FIXED]	Crash bug on exit\n\
\n\
0.2.5 - Mostly internal changes affecting color users\n\
    * [NEW]		Initial support for image display on color screens (requires GDI+)\n\
    * [CHANGED]	Now using OpenGL instead of GDI to render visualisations (text and image display still use GDI)\n\
    * [CHANGED]	Improved gradient calculations for color screens\n\
    * [FIXED]	Performance issues (performance is vastly improved though still needs work in some cases)\n\
    * [FIXED]	Several minor bugs including wrong config being read for some variables\n\
\n\
0.2.1 - Bug fixes\n\
    * [FIXED]	Font dialog not displaying\n\
\n\
0.2.0 - Major update\n\
    * [NEW]		Config dialog & more options (including ability to disable specific visualisations)\n\
    * [NEW]		Support for color gradients (including new line drawing algorithm)\n\
    * [NEW]		Support for font selection\n\
    * [NEW]		Expert mode, including the ability to use advance track info details\n\
    * [NEW]		Ability to change time of randomisations and track info display\n\
    * [NEW]		Ability to change options of individual effects\n\
    * [UPDATED]	Vastly improved G19 support\n\
    * [FIXED]	Config so that all changes that can be applied instantly are\n\
    * [FIXED]	Floating point infinity error\n\
    * [FIXED]	Track information display at incorrect times\n\
    * [PARTIAL]	Added initial implementation of many other options not yet enabled\n\
\n\
0.1.2 - Bug fixes\n\
    * [FIXED]	Visualisations on G19 (with help from ragganiczu - many thanks)\n\
    * [FIXED]	Config dialog bugs\n\
    * [FIXED]	Color support on G19 track info page\n\
    * [FIXED]	Other minor bugs\n\
    * [NEW]		Better quality foobar images for both monochrome and color screens\n\
\n\
0.1.1 - Bug fixes & additions\n\
    * [FIXED]	Removed potential divide by zero\n\
    * [NEW]		Added track time counters and associated options\n\
    * [NEW]		2 new oscilloscope visualisations: Circle & Star Burst\n\
    * [TWEAKED] Oscilloscope visualisation 2 (more substantial)\n\
\n\
0.1.0 - First Release\n\
");
#endif
