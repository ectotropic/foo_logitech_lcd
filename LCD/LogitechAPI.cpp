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
//
#include "CommonHeaders.h"
#include "LogitechAPI.h"
//--------------------------------------

namespace {
    //**************************************************************************
    //  Common Helpers
    //**************************************************************************
    bool ConnectionStatus(_In_ DWORD dwStatus,
                          _In_ LCD::Logitech::HCONNECTION hConnection) noexcept {
        switch (dwStatus) {
        case ERROR_SUCCESS:
            if (hConnection == LGLCD_INVALID_CONNECTION) {
                SafeLogError("[Logitech LCD SDK] lgLcdConnectEx returned \"ERROR_SUCCESS\", but hConnection handle is invalid");
                return false;
            } else {
                SafeLogTrace("[Logitech LCD SDK] Successfully connected to Logitech LCD hDevice.");
                return true;
            }
        case ERROR_SERVICE_NOT_ACTIVE:
            SafeLogError("[Logitech LCD SDK] LCD API not yet initialised");
            return false;
        case ERROR_INVALID_PARAMETER:
            SafeLogError("[Logitech LCD SDK] Invalid connect context structure");
            return false;
        case ERROR_FILE_NOT_FOUND:
            SafeLogError("[Logitech LCD SDK] LCDMon does not appear to be running");
            return false;
        case ERROR_ALREADY_EXISTS:
            SafeLogError("[Logitech LCD SDK] Connection already exists for this application");
            return false;
        case RPC_X_WRONG_PIPE_VERSION:
            SafeLogError("[Logitech LCD SDK] LCDMon version mismatch");
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} from lgLcdConnectEx", dwStatus);
            return false;
        }
    }
} // namespace <anonymous>

//==============================================================================
//==============================================================================

namespace LCD::Logitech {
    //*************************************************************************
    //  API Wrappers/Helpers
    //*************************************************************************
    bool InitialiseAPI() noexcept {
        const auto status = ::lgLcdInit();
        switch (status) {
        case ERROR_SUCCESS:
            SafeLogTrace("[Logitech LCD SDK] Successfully initialised API.");
            return true;
        case ERROR_ALREADY_INITIALIZED:
            SafeLogInfo("[Logitech LCD SDK] API already initialised.");
            return true;
        case RPC_S_SERVER_UNAVAILABLE:
            SafeLogError("[Logitech LCD SDK] LCD drivers not installed or incorrect version.");
            return false;
        case ERROR_OLD_WIN_VERSION:
            SafeLogError("[Logitech LCD SDK] API not supported on this operating system.");
            return false;
        case ERROR_NO_SYSTEM_RESOURCES:
            SafeLogError("[Logitech LCD SDK] Insufficient system resources to initialise API.");
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} while initialising API.", status);
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool UninitialiseAPI() noexcept {
        const auto status = ::lgLcdDeInit();
        SafeLogErrorUnless(status == ERROR_SUCCESS,
                           "[Logitech LCD SDK] Failed to uninitialize Logitech LCD API with error: {}", status);
        return status == ERROR_SUCCESS;
    }

    //--------------------------------------------------------------------------

    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExW& params) noexcept {
        auto connectParam{ params };
        connectParam.connection = LGLCD_INVALID_CONNECTION;
        const auto status = ::lgLcdConnectExW(&connectParam);
        return ConnectionStatus(status, connectParam.connection) ?
            connectParam.connection :
            LGLCD_INVALID_CONNECTION;
    }

    //--------------------------------------------------------------------------

    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExA& params) noexcept {
        auto connectParam{ params };
        connectParam.connection = LGLCD_INVALID_CONNECTION;
        const auto status = ::lgLcdConnectExA(&connectParam);
        return ConnectionStatus(status, connectParam.connection) ?
            connectParam.connection :
            LGLCD_INVALID_CONNECTION;
    }

    //--------------------------------------------------------------------------

    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExW* pParams) noexcept {
        assert(pParams);
        if (!pParams) {
            SafeLogError("[Logitech LCD SDK] Invalid (null) connection context.");
            return LGLCD_INVALID_CONNECTION;
        }
        return ::LCD::Logitech::Connect(*pParams);
    }

    //--------------------------------------------------------------------------

    HCONNECTION Connect(_In_ const ::lgLcdConnectContextExA* pParams) noexcept {
        assert(pParams);
        if (!pParams) {
            SafeLogError("[Logitech LCD SDK] Invalid (null) connection context.");
            return LGLCD_INVALID_CONNECTION;
        }
        return ::LCD::Logitech::Connect(*pParams);
    }

    //--------------------------------------------------------------------------

    bool Disconnect(_In_ HCONNECTION hConnection) noexcept {
        assert(hConnection != LGLCD_INVALID_CONNECTION);
        const auto status = ::lgLcdDisconnect(hConnection);
        switch (status) {
        case ERROR_SUCCESS:
            SafeLogTrace("[Logitech LCD SDK] Successfully closed connection.");
            return true;
        case ERROR_SERVICE_NOT_ACTIVE:
            SafeLogError("[Logitech LCD SDK] LCD API not yet initialised.");
            return false;
        case ERROR_INVALID_PARAMETER:
            SafeLogError("[Logitech LCD SDK] Invalid handle passed to lgLcdDisconnect.");
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} while closing LCD connection.", status);
            return false;
        }
    }

    //--------------------------------------------------------------------------

    HDEVICE OpenDevice(_In_ const ::lgLcdOpenByTypeContext& params) noexcept {
        auto connectParams{ params };
        connectParams.device = LGLCD_INVALID_DEVICE;
        const auto status = ::lgLcdOpenByType(&connectParams);
        switch (status) {
        case ERROR_SUCCESS: {
            if (connectParams.device == LGLCD_INVALID_DEVICE) {
                SafeLogError("[Logitech LCD SDK] lgLcdOpenByType returned \"ERROR_SUCCESS\", but device handle is invalid");
            } else {
                SafeLogTrace("[Logitech LCD SDK] Successfully connected to Logitech LCD device.");
            }
            return connectParams.device;
        }
        case ERROR_SERVICE_NOT_ACTIVE:
            SafeLogError("[Logitech LCD SDK] LCD API not yet initialised.");
            return LGLCD_INVALID_DEVICE;
        case ERROR_INVALID_PARAMETER:
            SafeLogError("Invalid type context structure.");
            return LGLCD_INVALID_DEVICE;
        case ERROR_ALREADY_EXISTS:
            SafeLogError("Same hConnection to device already open.");
            return LGLCD_INVALID_DEVICE;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error \"{}\" from lgLcdOpenByType.", status);
            return LGLCD_INVALID_DEVICE;
        }
    }

    //--------------------------------------------------------------------------

    HDEVICE OpenDevice(_In_ const ::lgLcdOpenByTypeContext* pParams) noexcept {
        assert(pParams);
        if (!pParams) {
            SafeLogError("[Logitech LCD SDK] Invalid (null) hConnection context.");
            return {};
        }
        return ::LCD::Logitech::OpenDevice(*pParams);
    }

    //--------------------------------------------------------------------------

    bool CloseDevice(_In_ HDEVICE hDevice) noexcept {
        assert(hDevice != LGLCD_INVALID_DEVICE);
        const auto status = ::lgLcdClose(hDevice);
        switch (status) {
        case ERROR_SUCCESS:
            SafeLogTrace("[Logitech LCD SDK] Successfully closed hDevice.");
            return true;
        case ERROR_DEVICE_NOT_CONNECTED:
            SafeLogInfo("[Logitech LCD SDK] Device already closed.");
            return true;
        case ERROR_SERVICE_NOT_ACTIVE:
            SafeLogError("[Logitech LCD SDK] LCD API not yet initialised.");
            return false;
        case ERROR_INVALID_PARAMETER:
            SafeLogError("[Logitech LCD SDK] Invalid handle passed to lgLcdClose.");
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} while closing LCD hDevice.", status);
            return false;
        }
    }

    //--------------------------------------------------------------------------

    DWORD UpdateBitmap(_In_ HDEVICE hDevice,
                       _In_ const ::lgLcdBitmapHeader& bitmap,
                       _In_ DWORD dwPriority,
                       _In_ bool bVSync) noexcept {
        assert(hDevice != LGLCD_INVALID_DEVICE);

        if (bVSync) {
            dwPriority = LGLCD_SYNC_COMPLETE_WITHIN_FRAME(dwPriority);
        } else {
            dwPriority = LGLCD_ASYNC_UPDATE(dwPriority);
        }

        // Logging a message for every failure can cause a massive number of messages
        // to be generated, so only changes in the status are logged
        static DWORD s_LastStatus{ ERROR_SUCCESS };
        const auto status = ::lgLcdUpdateBitmap(hDevice, &bitmap, dwPriority);
        switch (status) {
        case ERROR_SUCCESS:
            return LGLCD_UPDATE_STATUS_SUCCESS;
        case ERROR_ACCESS_DENIED:
            if (s_LastStatus != status) {
                s_LastStatus = status;
                if (dwPriority == LGLCD_SYNC_COMPLETE_WITHIN_FRAME(dwPriority)) {
                    SafeLogError("[Logitech LCD SDK] API error does not make sense for priority type");
                }
                SafeLogInfo("[Logitech LCD SDK] LCD Bitmap update was skipped");
            }
            return LGLCD_UPDATE_STATUS_SKIPPED;
        case ERROR_SERVICE_NOT_ACTIVE:
            if (s_LastStatus != status) {
                s_LastStatus = status;
                SafeLogError("[Logitech LCD SDK] LCD API not yet initialised");
            }
            return LGLCD_UPDATE_STATUS_FAILURE;
        case ERROR_INVALID_PARAMETER:
            if (s_LastStatus != status) {
                s_LastStatus = status;
                SafeLogError("[Logitech LCD SDK] Update bitmap not valid");
            }
            return LGLCD_UPDATE_STATUS_FAILURE;
        case ERROR_DEVICE_NOT_CONNECTED:
            if (s_LastStatus != status) {
                s_LastStatus = status;
                SafeLogWarning("[Logitech LCD SDK] LCD hDevice has been disconnected");
            }
            return LGLCD_UPDATE_STATUS_DEVICE_DISCONNECTED;
        default:
            if (s_LastStatus != status) {
                s_LastStatus = status;
                SafeLogError("[Logitech LCD SDK] Unrecognised error {} returned by lgLcdUpdateBitmap", status);
            }
            return LGLCD_UPDATE_STATUS_FAILURE;
        }
    }

    //--------------------------------------------------------------------------

    DWORD UpdateBitmap(_In_ HDEVICE hDevice,
                       _In_ const ::lgLcdBitmapHeader* pBitmap,
                       _In_ DWORD dwPriority,
                       _In_ bool bVSync) noexcept {
        assert(pBitmap);
        if (!pBitmap) {
            SafeLogError("[Logitech LCD SDK] Invalid (null) bitmap.");
            return LGLCD_UPDATE_STATUS_FAILURE;
        }
        return ::LCD::Logitech::UpdateBitmap(hDevice, *pBitmap, dwPriority, bVSync);
    }

    //--------------------------------------------------------------------------

    bool SetForegroundMode(_In_ HDEVICE hDevice,
                           _In_ bool bForeground) noexcept {
        assert(hDevice != LGLCD_INVALID_DEVICE);
        const auto status = ::lgLcdSetAsLCDForegroundApp(hDevice,
                                                         bForeground
                                                         ? LGLCD_LCD_FOREGROUND_APP_YES
                                                         : LGLCD_LCD_FOREGROUND_APP_NO);
        switch (status) {
        case ERROR_SUCCESS:
            SafeLogTrace("[Logitech LCD SDK] Successfully closed hDevice.");
            return true;
        case ERROR_LOCK_FAILED:
            SafeLogError("[Logitech LCD SDK] Setting foreground mode operation could not be completed.");
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} returned by lgLcdSetAsLCDForegroundApp", status);
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool ReadButtons(_In_ HDEVICE hDevice,
                     _Out_ DWORD& dwButtons) noexcept {
        assert(hDevice != LGLCD_INVALID_DEVICE);
        dwButtons = 0;
        const auto status = ::lgLcdReadSoftButtons(hDevice, &dwButtons);
        switch (status) {
        case ERROR_SUCCESS:
            return true;
        case ERROR_SERVICE_NOT_ACTIVE:
            SafeLogError("[Logitech LCD SDK] LCD API not yet initialised");
            dwButtons = 0;
            return false;
        case ERROR_INVALID_PARAMETER:
            SafeLogError("[Logitech LCD SDK] Invalid parameter passed to lgLcdReadSoftButtons");
            dwButtons = 0;
            return false;
        case ERROR_DEVICE_NOT_CONNECTED:
            SafeLogWarning("[Logitech LCD SDK] LCD hDevice has been disconnected");
            dwButtons = 0;
            return false;
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised error {} returned by lgLcdReadSoftButtons", status);
            dwButtons = 0;
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool ReadButtons(_In_  HDEVICE hDevice,
                     _Out_ LPDWORD lpdwButtons) noexcept {
        assert(lpdwButtons);
        if (!lpdwButtons) {
            SafeLogError("[Logitech LCD SDK] Invalid (null) output argument.");
            return {};
        }
        return ::LCD::Logitech::ReadButtons(hDevice, *lpdwButtons);
    }

    //**************************************************************************
    // Connection
    //**************************************************************************
    Connection::Device Connection::OpenDevice(_In_ DWORD dwType,
                                              _In_ const ButtonContext& context) const noexcept {
        assert(m_hConnection != LGLCD_INVALID_CONNECTION);
        ::lgLcdOpenByTypeContext openParams{ 0 };
        openParams.connection           = m_hConnection;
        openParams.deviceType           = dwType;
        openParams.device               = LGLCD_INVALID_DEVICE;
        openParams.onSoftbuttonsChanged = context;
        return Device{ ::LCD::Logitech::OpenDevice(openParams) };
    }

    //**************************************************************************
    // NotifyHandler
    //**************************************************************************
    DWORD WINAPI NotifyHandler::CallbackDispatcher(_In_ [[maybe_unused]] HCONNECTION hConnection,
                                                   _In_ const PVOID pContext,
                                                   _In_ DWORD notificationCode,
                                                   _In_ DWORD param1,
                                                   _In_ DWORD param2,
                                                   _In_ DWORD param3,
                                                   _In_ DWORD param4) noexcept {
        assert(hConnection != LGLCD_INVALID_CONNECTION);
        assert(pContext); if (!pContext) { return ERROR_INVALID_PARAMETER; }
        return ((NotifyHandler*)pContext)->OnNotify(notificationCode,
                                                    param1, param2,
                                                    param3, param4);
    }

    //--------------------------------------------------------------------------

    NotifyHandler::Status NotifyHandler::OnNotify(_In_ NotificationCode notification,
                                                  _In_ Param param1,
                                                  _In_ [[maybe_unused]] Param param2,
                                                  _In_ [[maybe_unused]] Param param3,
                                                  _In_ [[maybe_unused]] Param param4) noexcept {
        switch (notification) {
        case LGLCD_NOTIFICATION_DEVICE_ARRIVAL:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_DEVICE_ARRIVAL\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnDeviceArrival(param1);
        case LGLCD_NOTIFICATION_DEVICE_REMOVAL:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_DEVICE_REMOVAL\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnDeviceRemoval(param1);
        case LGLCD_NOTIFICATION_CLOSE_CONNECTION:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_CLOSE_CONNECTION\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnCloseConnection();
        case LGLCD_NOTIFICATION_APPLET_DISABLED:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_APPLET_DISABLED\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnAppletDisabled();
        case LGLCD_NOTIFICATION_APPLET_ENABLED:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_APPLET_ENABLED\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnAppletEnabled();
        case LGLCD_NOTIFICATION_TERMINATE_APPLET:
            SafeLogTrace("[Logitech LCD SDK] Received notification \"LGLCD_NOTIFICATION_TERMINATE_APPLET\" with params {}, {}, {}, {}", param1, param2, param3, param4);
            return this->OnTerminateApplet();
        default:
            SafeLogError("[Logitech LCD SDK] Unrecognised notification code {} received by notification callback (params {}, {}, {}, {})", notification, param1, param2, param3, param4);
            return ERROR_INVALID_PARAMETER;
        }
    }
} // namespace LCD::Logitech
