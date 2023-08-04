#pragma once
#ifndef GUID_A6FB1EFE_9FA3_4A0B_BF5D_65D532A23A1B
#define GUID_A6FB1EFE_9FA3_4A0B_BF5D_65D532A23A1B
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
 *******************************************************************************/

//--------------------------------------
// Windows header is required
// by the Logitech LCD API
#include "Windows/Windows_Core.h"
//--------------------------------------

//--------------------------------------
//
#include <lglcd.h>
//--------------------------------------

//--------------------------------------
//  Update status for clearer intent
#ifndef LGLCD_UPDATE_STATUS_FAILURE
#   define LGLCD_UPDATE_STATUS_FAILURE 0
#endif
#ifndef LGLCD_UPDATE_STATUS_SUCCESS
#   define LGLCD_UPDATE_STATUS_SUCCESS 1
#endif
#ifndef LGLCD_UPDATE_STATUS_SKIPPED
#   define LGLCD_UPDATE_STATUS_SKIPPED 2
#endif
#ifndef LGLCD_UPDATE_STATUS_DEVICE_DISCONNECTED
#   define LGLCD_UPDATE_STATUS_DEVICE_DISCONNECTED 3
#endif
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <utility>
#include <atomic>
//--------------------------------------

namespace LCD::Logitech {
    //**************************************************************************
    //
    //**************************************************************************

    using HCONNECTION = int;
    using HDEVICE     = int;

    //----------------------------------

    bool InitialiseAPI  () noexcept;
    bool UninitialiseAPI() noexcept;

    //----------------------------------

    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExW& params)  noexcept;
    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExA& params)  noexcept;
    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExW* pParams) noexcept;
    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExA* pParams) noexcept;

    bool Disconnect(_In_ HCONNECTION connection) noexcept;

    //----------------------------------

    HDEVICE OpenDevice(_In_ const ::lgLcdOpenByTypeContext& params)  noexcept;
    HDEVICE OpenDevice(_In_ const ::lgLcdOpenByTypeContext* pParams) noexcept;

    bool CloseDevice(_In_ HDEVICE hDevice) noexcept;

    //----------------------------------

    DWORD UpdateBitmap(_In_ HDEVICE hDevice,
                       _In_ const ::lgLcdBitmapHeader& bitmap,
                       _In_ DWORD dwPriority,
                       _In_ bool bVSync) noexcept;

    DWORD UpdateBitmap(_In_ HDEVICE hDevice,
                       _In_ const ::lgLcdBitmapHeader* pBitmap,
                       _In_ DWORD dwPriority,
                       _In_ bool bVSync) noexcept;

    //----------------------------------

    bool SetForegroundMode(_In_ HDEVICE hDevice,
                           _In_ bool bForeground) noexcept;

    //----------------------------------

    bool ReadButtons(_In_ HDEVICE hDevice,
                     _Out_ DWORD& dwButtons) noexcept;

    bool ReadButtons(_In_ HDEVICE hDevice,
                     _Out_ LPDWORD lpdwButtons) noexcept;

    //----------------------------------

    inline DWORD GetButtons(_In_ HDEVICE hDevice) noexcept {
        DWORD buttons{ 0 };
        return ::LCD::Logitech::ReadButtons(hDevice, buttons) ? buttons : 0;
    }

    //----------------------------------

    inline DWORD GetButtons(_In_ HDEVICE hDevice,
                            _In_ DWORD dwPreviousState) noexcept {
        return ::LCD::Logitech::GetButtons(hDevice) & ~dwPreviousState;
    }

    //**************************************************************************
    // Device
    //**************************************************************************
    class Device {
    public:
        using Handle       = ::LCD::Logitech::HDEVICE;

        using State        = ::DWORD;
        using Priority     = ::DWORD;
        using ButtonState  = ::DWORD;

        using BitmapDesc   = ::lgLcdBitmap;
        using BitmapHeader = ::lgLcdBitmapHeader;

    private:
        Device(const Device&)            = delete; // No Copy
        Device& operator=(const Device&) = delete; // No Copy

    public:
        Device() noexcept = default;

        Device(_In_ Handle hDevice) noexcept :
            m_hDevice{ hDevice } {}

        Device(_In_ Device&& other) noexcept :
            m_hDevice{ std::exchange(other.m_hDevice, LGLCD_INVALID_DEVICE) } {}

        ~Device() noexcept { Close(); }

        Device& operator=(_In_ Device&& other) noexcept {
            Close();
            using std::swap;
            swap(m_hDevice, other.m_hDevice);
            return *this;
        }

    public:
        bool Close() noexcept {
            const auto hDevice{ std::exchange(m_hDevice, LGLCD_INVALID_DEVICE) };
            return (hDevice == LGLCD_INVALID_DEVICE) ||
                ::LCD::Logitech::CloseDevice(hDevice);
        }

        DWORD UpdateBitmap(_In_ const BitmapHeader& bitmap,
                           _In_ Priority priority,
                           _In_ bool bVSync) noexcept {
            assert(m_hDevice != LGLCD_INVALID_DEVICE);
            return ::LCD::Logitech::UpdateBitmap(m_hDevice, bitmap,
                                                 priority, bVSync);
        }

        DWORD UpdateBitmap(_In_ const BitmapHeader* pBitmap,
                           _In_ Priority priority,
                           _In_ bool bVSync) noexcept {
            assert(m_hDevice != LGLCD_INVALID_DEVICE);
            return ::LCD::Logitech::UpdateBitmap(m_hDevice, pBitmap,
                                                 priority, bVSync);
        }

        bool SetForegroundMode(_In_ bool bForeground) noexcept {
            assert(m_hDevice != LGLCD_INVALID_DEVICE);
            return ::LCD::Logitech::SetForegroundMode(m_hDevice, bForeground);
        }

        bool ReadButtons(_Out_ ButtonState& buttons) const noexcept {
            assert(m_hDevice != LGLCD_INVALID_DEVICE);
            return ::LCD::Logitech::ReadButtons(m_hDevice, buttons);
        }

        bool ReadButtons(_Out_ ButtonState* buttons) const noexcept {
            assert(m_hDevice != LGLCD_INVALID_DEVICE);
            return ::LCD::Logitech::ReadButtons(m_hDevice, buttons);
        }

        ButtonState GetButtons() const noexcept {
            return ::LCD::Logitech::GetButtons(m_hDevice);
        }

        ButtonState GetButtons(_In_ ButtonState lastState) const noexcept {
            return ::LCD::Logitech::GetButtons(m_hDevice, lastState);
        }

        constexpr operator Handle() const noexcept {
            return m_hDevice;
        }

        explicit constexpr operator bool() const noexcept {
            return m_hDevice != LGLCD_INVALID_DEVICE;
        }

    private:
        Handle m_hDevice{ LGLCD_INVALID_DEVICE };
    }; // class Device

    //**************************************************************************
    // Connection
    //**************************************************************************
    class Connection {
    public:
        using Handle         = ::LCD::Logitech::HCONNECTION;
        using ButtonContext  = ::lgLcdSoftbuttonsChangedContext;
        using ButtonCallback = ::lgLcdOnSoftButtonsCB;
        using Device         = ::LCD::Logitech::Device;

    private:
        Connection(const Connection&) = delete; // No Copy
        Connection& operator=(const Connection&) = delete; // No Copy

    public:
        Connection() noexcept = default;

        Connection(_In_ Handle hConnection) noexcept :
            m_hConnection{ hConnection } {}

        Connection(_In_ Connection&& other) noexcept :
            m_hConnection{ std::exchange(other.m_hConnection, LGLCD_INVALID_CONNECTION) } {}

        ~Connection() noexcept { Disconnect(); }

        Connection& operator=(_In_ Connection&& other) noexcept {
            Disconnect();
            using std::swap;
            swap(m_hConnection, other.m_hConnection);
            return *this;
        }

        bool Disconnect() noexcept {
            if (m_hConnection == LGLCD_INVALID_CONNECTION) { return false; }
            return ::LCD::Logitech::Disconnect(std::exchange(m_hConnection, LGLCD_INVALID_CONNECTION));
        }

        Device OpenDevice(_In_ DWORD dwType,
                          _In_ const ButtonContext& context) const noexcept;

        Device OpenDevice(_In_ DWORD dwType,
                          _In_opt_ const ButtonContext* pContext = nullptr) const noexcept {
            const ButtonContext context{ nullptr, nullptr };
            return OpenDevice(dwType, (pContext ? *pContext : context));
        }

        Device OpenDevice(_In_ DWORD dwType,
                          _In_opt_ const ButtonCallback callback,
                          _In_opt_ void* pUserContext) const noexcept {
            const ButtonContext context{ callback, pUserContext };
            return OpenDevice(dwType, context);
        }

        constexpr operator Handle() const noexcept {
            return m_hConnection;
        }

        explicit constexpr operator bool() const noexcept {
            return m_hConnection != LGLCD_INVALID_CONNECTION;
        }

    private:
        Handle m_hConnection{ LGLCD_INVALID_CONNECTION };
    }; // class Connection

    //**************************************************************************
    // API
    //**************************************************************************
    class API {
    public:
        using ConnectContextA = ::lgLcdConnectContextExA;
        using ConnectContextW = ::lgLcdConnectContextExW;
        using Connection      = ::LCD::Logitech::Connection;

    private:
        API(const API&) = delete; // No Copy
        API& operator=(const API&) = delete; // No Copy

    public:
        API() noexcept = default;

        ~API() noexcept { Uninitialise(); }

        API(API&& other) noexcept :
            m_bInitialised{ std::exchange(other.m_bInitialised, false) } {}

        API& operator=(API&& other) noexcept {
            Uninitialise();
            using std::swap;
            swap(m_bInitialised, other.m_bInitialised);
            return *this;
        }

        bool Initialise() noexcept {
            if (m_bInitialised) { return false; }
            m_bInitialised = ::LCD::Logitech::InitialiseAPI();
            if (m_bInitialised) { m_InitCount++; }
            return m_bInitialised;
        }

        bool Uninitialise() noexcept {
            if (!m_bInitialised) { return false; }
            m_bInitialised = false;
            if (m_InitCount == 1) {
                m_InitCount = 0;
                return ::LCD::Logitech::UninitialiseAPI();
            } else if (m_InitCount != 0) {
               --m_InitCount;
               return true;
            } else {
                return false;
            }
        }

        Connection OpenConnection(_In_ const ConnectContextA& params) const noexcept {
            if (!m_bInitialised) { return Connection{}; }
            return Connection{ ::LCD::Logitech::Connect(params) };
        }

        Connection OpenConnection(_In_ const ConnectContextA* pParams) const noexcept {
            if (!m_bInitialised) { return Connection{}; }
            return Connection{ ::LCD::Logitech::Connect(pParams) };
        }

        Connection OpenConnection(_In_ const ConnectContextW& params) const noexcept {
            if (!m_bInitialised) { return Connection{}; }
            return Connection{ ::LCD::Logitech::Connect(params) };
        }

        Connection OpenConnection(_In_ const ConnectContextW* pParams) const noexcept {
            if (!m_bInitialised) { return Connection{}; }
            return Connection{ ::LCD::Logitech::Connect(pParams) };
        }

        constexpr explicit operator bool() const noexcept { return m_bInitialised; }

    private:
        bool m_bInitialised{ false };
        inline static std::atomic_uint m_InitCount{ 0 };
    }; // class API

    //**************************************************************************
    // ButtonHandler
    //**************************************************************************
    class ButtonHandler {
    public:
        using DeviceHandle = ::LCD::Logitech::HDEVICE;
        using Context      = ::lgLcdSoftbuttonsChangedContext;
        using Callback     = ::lgLcdOnSoftButtonsCB;
        using ButtonState  = ::DWORD;
        using Status       = ::DWORD;

    private:
        // For some reason the API uses `const` pointers for
        // user context data, this is not very helpful so
        // the `const` is removed when calling the callback.
        // It's not ideal, but in practice is more useful
        // and although technically iffy is pretty safe.
        static DWORD WINAPI CallbackDispatcher(_In_ HDEVICE hDevice,
                                               _In_ DWORD  buttons,
                                               _In_ const PVOID pContext) noexcept {
            assert(pContext); if (!pContext) { return ERROR_INVALID_PARAMETER; }
            return ((ButtonHandler*)pContext)->OnButton(hDevice, buttons);
        }

    private:
        // Can't copy or move the handler (the pointer to
        // the handler is stored in the API)
        ButtonHandler(const ButtonHandler& ) = delete;
        ButtonHandler(      ButtonHandler&&) = delete;

        ButtonHandler& operator=(const ButtonHandler& ) = delete;
        ButtonHandler& operator=(      ButtonHandler&&) = delete;

    protected:
        ButtonHandler() noexcept :
            m_Context{ &ButtonHandler::CallbackDispatcher, this } {}

        ~ButtonHandler() noexcept = default;

        virtual Status OnButton(_In_ DeviceHandle hDevice,
                                _In_ ButtonState  buttons) const noexcept = 0;

    protected:
        //--------------------------------------------------
        // It's relatively likely users will inherit from
        // multiple callback classes, so give the accessors
        // full names for easy access.
        auto GetButtonChangedHandlerContext() const noexcept {
            return m_Context;
        }

        auto GetButtonChangedHandlerCallback() const noexcept {
            return m_Context.softbuttonsChangedCallback;
        }

        auto GetButtonChangedHandlerUserContext() const noexcept {
            return m_Context.softbuttonsChangedContext;
        }
        //--------------------------------------------------

        //--------------------------------------------------
        // Accessors aliases for simpler use cases.
        decltype(auto) GetContext() const noexcept {
            return GetButtonChangedHandlerContext();
        }

        decltype(auto) GetCallback() const noexcept {
            return GetButtonChangedHandlerCallback();
        }

        decltype(auto) GetUserContext() const noexcept {
            return GetButtonChangedHandlerUserContext();
        }
        //--------------------------------------------------

    private:
        const Context m_Context{};
    }; // class ButtonHandler

    //**************************************************************************
    // NotifyHandler
    //**************************************************************************
    class NotifyHandler {
    public:
        using ConnectionHandle = ::LCD::Logitech::HCONNECTION;
        using Context           = ::lgLcdNotificationContext;
        using Callback          = ::lgLcdOnNotificationCB;
        using Status            = ::DWORD;
        using NotificationCode  = ::DWORD;
        using Param             = ::DWORD;

    private:
        // For some reason the API uses `const` pointers for
        // user context data, this is not very helpful so
        // the `const` is removed when calling the callback.
        // It's not ideal, but in practice is more useful
        // and although technically iffy is pretty safe.
        static DWORD WINAPI CallbackDispatcher(_In_ HCONNECTION hConnection,
                                               _In_ const PVOID pContext,
                                               _In_ DWORD notificationCode,
                                               _In_ DWORD notifyParm1,
                                               _In_ DWORD notifyParm2,
                                               _In_ DWORD notifyParm3,
                                               _In_ DWORD notifyParm4) noexcept;

    public:
        // Can't copy or move the handler (the pointer to
        // the handler is stored in the API)
        NotifyHandler(const NotifyHandler&) = delete;
        NotifyHandler(      NotifyHandler&&) = delete;

        NotifyHandler& operator=(const NotifyHandler&) = delete;
        NotifyHandler& operator=(      NotifyHandler&&) = delete;

    protected:
        NotifyHandler() noexcept :
            m_Context{ &NotifyHandler::CallbackDispatcher, this } {}

        ~NotifyHandler() noexcept = default;

        virtual Status OnNotify(_In_ NotificationCode notification,
                                _In_ Param param1,
                                _In_ Param param2,
                                _In_ Param param3,
                                _In_ Param param4) noexcept;

        virtual Status OnDeviceArrival  (_In_ Param /*deviceType*/) noexcept { return ERROR_SUCCESS; }
        virtual Status OnDeviceRemoval  (_In_ Param /*deviceType*/) noexcept { return ERROR_SUCCESS; }
        virtual Status OnCloseConnection() noexcept { return ERROR_SUCCESS; }
        virtual Status OnAppletDisabled () noexcept { return ERROR_SUCCESS; }
        virtual Status OnAppletEnabled  () noexcept { return ERROR_SUCCESS; }

        // Undocumented notification
        virtual Status OnTerminateApplet() noexcept { return ERROR_SUCCESS; }

    protected:
        //--------------------------------------------------
        // It's relatively likely users will inherit from
        // multiple callback classes, so give the accessors
        // full names for easy access.
        auto GetNotifyHandlerContext() const noexcept {
            return m_Context;
        }

        auto GetNotifyHandlerCallback() const noexcept {
            return m_Context.notificationCallback;
        }

        auto GetNotifyHandlerUserContext() const noexcept {
            return m_Context.notifyContext;
        }
        //--------------------------------------------------

        //--------------------------------------------------
        // Accessors aliases for simpler use cases.
        decltype(auto) GetContext() const noexcept {
            return GetNotifyHandlerContext();
        }

        decltype(auto) GetCallback() const noexcept {
            return GetNotifyHandlerCallback();
        }

        decltype(auto) GetUserContext() const noexcept {
            return GetNotifyHandlerUserContext();
        }
        //--------------------------------------------------

    private:
        const Context m_Context{};
    }; // class NotifyHandler

    //**************************************************************************
    // ConfigureHandler
    //**************************************************************************
    class ConfigureHandler {
    public:
        using ConnectionHandle = ::LCD::Logitech::HCONNECTION;
        using Context          = ::lgLcdConfigureContext;
        using Callback         = ::lgLcdOnConfigureCB;
        using Status           = ::DWORD;

    private:
        // For some reason the API uses `const` pointers for
        // user context data, this is not very helpful so
        // the `const` is removed when calling the callback.
        // It's not ideal, but in practice is more useful
        // and although technically iffy is pretty safe.
        static DWORD WINAPI CallbackDispatcher(_In_ HCONNECTION hConnection,
                                               _In_ const PVOID pContext) noexcept {
            assert(pContext); if (!pContext) { return ERROR_INVALID_PARAMETER; }
            return ((ConfigureHandler*)pContext)->OnConfigure(hConnection);
        }

    public:
        // Can't copy or move the handler (the pointer to
        // the handler is stored in the API)
        ConfigureHandler(const ConfigureHandler&) = delete;
        ConfigureHandler(      ConfigureHandler&&) = delete;

        ConfigureHandler& operator=(const ConfigureHandler&) = delete;
        ConfigureHandler& operator=(      ConfigureHandler&&) = delete;

    protected:
        ConfigureHandler() noexcept :
            m_Context{ &ConfigureHandler::CallbackDispatcher, this } {}

        ~ConfigureHandler() noexcept = default;

        virtual Status OnConfigure(_In_ ConnectionHandle hConnection) const noexcept = 0;

    protected:
        //--------------------------------------------------
        // It's relatively likely users will inherit from
        // multiple callback classes, so give the accessors
        // full names for easy access.
        auto GetConfigureHandlerContext() const noexcept {
            return m_Context;
        }

        auto GetConfigureHandlerCallback() const noexcept {
            return m_Context.configCallback;
        }

        auto GetConfigureHandlerUserContext() const noexcept {
            return m_Context.configContext;
        }
        //--------------------------------------------------

        //--------------------------------------------------
        // Accessors aliases for simpler use cases.
        decltype(auto) GetContext() const noexcept {
            return GetConfigureHandlerContext();
        }

        decltype(auto) GetCallback() const noexcept {
            return GetConfigureHandlerCallback();
        }

        decltype(auto) GetUserContext() const noexcept {
            return GetConfigureHandlerUserContext();
        }
        //--------------------------------------------------

    private:
        const Context m_Context{};
    }; // class ConfigureHandler
} // namespace LCD::Logitech
#endif //GUID_A6FB1EFE_9FA3_4A0B_BF5D_65D532A23A1B
