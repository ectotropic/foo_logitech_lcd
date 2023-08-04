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
#include "LogitechLCD.h"
//--------------------------------------

namespace LCD::Logitech {
    //**************************************************************************
    // LogitechLCD
    //**************************************************************************
    LogitechLCD::LogitechLCD(singleton_constructor_tag /*tag*/,
                             const char* szName,
                             bool bAutoStartable,
                             bool bPersistent) noexcept {
        if (m_API.Initialise()) {
            lgAPI::ConnectContextA params{ 0 };
            params.appFriendlyName               = szName;
            params.isAutostartable               = bAutoStartable ? TRUE : FALSE;
            params.isPersistent                  = bPersistent ? TRUE : FALSE;
            params.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW | LGLCD_APPLET_CAP_QVGA;
            params.connection                    = 0;
            params.onConfigure.configCallback    = nullptr;
            params.onConfigure.configContext     = nullptr;
            params.onNotify                      = GetNotifyHandlerContext();
            m_Connection = m_API.OpenConnection(params);
        }
    }

    //--------------------------------------------------------------------------

    LogitechLCD::LogitechLCD(singleton_constructor_tag /*tag*/,
                             const wchar_t* szName,
                             bool bAutoStartable,
                             bool bPersistent) noexcept {
        if (m_API.Initialise()) {
            lgAPI::ConnectContextW params{ 0 };
            params.appFriendlyName               = szName;
            params.isAutostartable               = bAutoStartable ? TRUE : FALSE;
            params.isPersistent                  = bPersistent ? TRUE : FALSE;
            params.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW | LGLCD_APPLET_CAP_QVGA;
            params.connection                    = 0;
            params.onConfigure.configCallback    = nullptr;
            params.onConfigure.configContext     = nullptr;
            params.onNotify                      = GetNotifyHandlerContext();
            m_Connection = m_API.OpenConnection(params);
        }
    }

    //--------------------------------------------------------------------------

    bool LogitechLCD::OnConnect(DeviceDesc& desc,
                                DeviceType prefered,
                                void* data) {
        if (!m_Connection) {
            SafeLogError("Failed to open device: Not connected to LCD API.");
            return false;
        }

        if (!m_bAppletEnabled) {
            SafeLogWarning("Failed to open device: Applet is currently disabled.");
            return false;
        }

        Disconnect(data);

        bool bSuccess = false;
        if (prefered == DeviceType::Unknown) {
            SafeLogTrace("Attempting to open \"Color\" LCD");
            bSuccess = ConnectToType(desc, DeviceType::Color);
            if (!bSuccess) {
                SafeLogTrace("Failed to open \"Color\" LCD, attempting to open \"Monochrome\"");
                bSuccess = ConnectToType(desc, DeviceType::Monochrome);
            }
        } else {
            SafeLogTrace("Attempting to open \"Color\" LCD");
            bSuccess = ConnectToType(desc, prefered);
        }

        if (bSuccess) {
            return true;
        } else {
            SafeLogError("Failed to open an LCD device of the specified type");
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool LogitechLCD::ConnectToType(DeviceDesc& desc,
                                    DeviceType type) noexcept {
        if (!m_Connection) {
            SafeLogError("Failed to open device: Not connected to LCD API.");
            return false;
        }

        switch (type) {
            case DeviceType::Color: {
                m_Device = m_Connection.OpenDevice(LGLCD_DEVICE_QVGA);

                desc.eType         = DeviceType::Color;
                desc.iWidth        = LGLCD_QVGA_BMP_WIDTH;
                desc.iHeight       = LGLCD_QVGA_BMP_HEIGHT;
                desc.iBitsPerPixel = LGLCD_QVGA_BMP_BPP * 8;
                desc.iButtonCount  = 8;

                m_LGLcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
                break;
            }

            case DeviceType::Monochrome: {
                m_Device = m_Connection.OpenDevice(LGLCD_DEVICE_BW);

                desc.eType         = DeviceType::Monochrome;
                desc.iWidth        = LGLCD_BW_BMP_WIDTH;
                desc.iHeight       = LGLCD_BW_BMP_HEIGHT;
                desc.iBitsPerPixel = 8;
                desc.iButtonCount  = 4;

                m_LGLcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
                break;
            }

            DEFAULT_UNREACHABLE;
        }

        return DeviceOpened();
    }

    //--------------------------------------------------------------------------

    bool LogitechLCD::OnDisconnect(void* /*data*/) noexcept {
        return (!m_Device || m_Device.Close());
    }

    //--------------------------------------------------------------------------

    LogitechLCD::UpdateStatus LogitechLCD::Update(void* data) {
        assert(data);
        if (!data) {
            SafeLogError("Invalid parameter: update data is null.");
            return UpdateStatus::Skipped;
        }

        if (!m_Device) {
            SafeLogTrace("Skipping LCD Update: No device currently initialised.");
            return UpdateStatus::Skipped;
        }

        if (!m_bAppletEnabled) {
            SafeLogInfo("Skipping LCD Update: Applet is currently disabled.");
            return UpdateStatus::Skipped;
        }

        if (m_LGLcdBitmap.hdr.Format == LGLCD_BMP_FORMAT_160x43x1) {
            std::memcpy(m_LGLcdBitmap.bmp_mono.pixels,
                        data, GetDisplayByteSize());
        } else {
            std::memcpy(m_LGLcdBitmap.bmp_qvga32.pixels,
                        data, GetDisplayByteSize());
        }

        using LGPriority = typename lgDevice::Priority;

        const auto priority = GetDisplayPriority();
        LGPriority nativePriority{ 0 };
        switch (priority) {
            case Priority::Idle:       nativePriority = LGLCD_PRIORITY_IDLE_NO_SHOW; break;
            case Priority::Background: nativePriority = LGLCD_PRIORITY_BACKGROUND;   break;
            case Priority::Foreground: nativePriority = LGLCD_PRIORITY_NORMAL;       break;
            case Priority::Alert:      nativePriority = LGLCD_PRIORITY_ALERT;        break;
            DEFAULT_UNREACHABLE;
        }

        const auto flags{ GetFlags() };
        const auto status = m_Device.UpdateBitmap(&(m_LGLcdBitmap.hdr),
                                                  nativePriority,
                                                  flags & Flags::PreferVSync);
        switch (status) {
            case LGLCD_UPDATE_STATUS_SUCCESS: {
                if (priority == Priority::Alert) {
                    SetDisplayPriority(Priority::Foreground);
                } else if (flags & Flags::ForcePriority) {
                    m_Device.SetForegroundMode(true);
                }
                return UpdateStatus::Success;
            }

            case LGLCD_UPDATE_STATUS_SKIPPED:
                return UpdateStatus::Skipped;

            case LGLCD_UPDATE_STATUS_DEVICE_DISCONNECTED:
                SafeLogWarning("LCD device disconnected.");
                m_Device.Close();
                return UpdateStatus::Failure;

            case LGLCD_UPDATE_STATUS_FAILURE:
                return UpdateStatus::Failure;

            DEFAULT_UNREACHABLE;
        }
    }

    //--------------------------------------------------------------------------

    void LogitechLCD::SetDisplayPriority(Priority priority) noexcept {
        ILCD::SetDisplayPriority(priority);
        if (m_Device && m_bAppletEnabled) {
            m_Device.SetForegroundMode(priority == Priority::Foreground ||
                                       priority == Priority::Alert);
        }
    }

    //--------------------------------------------------------------------------

    LogitechLCD::ButtonState LogitechLCD::GetButtons() const noexcept {
        if (!m_Device) {
            SafeLogTrace("Failed to get button state: No device currently initialised.");
            return ButtonState::None;
        }

        if (!m_bAppletEnabled) {
            SafeLogInfo("Failed to get button state: Applet is currently disabled.");
            return ButtonState::None;
        }

        using LGButtonState = typename lgDevice::ButtonState;

        LGButtonState dwButtons{ 0 };
        if (m_Device.ReadButtons(dwButtons)) {
            ButtonState buttonStates{ ButtonState::None };
            if (dwButtons & LGLCDBUTTON_BUTTON0) { buttonStates |= ButtonState::Button0; }
            if (dwButtons & LGLCDBUTTON_BUTTON1) { buttonStates |= ButtonState::Button1; }
            if (dwButtons & LGLCDBUTTON_BUTTON2) { buttonStates |= ButtonState::Button2; }
            if (dwButtons & LGLCDBUTTON_BUTTON3) { buttonStates |= ButtonState::Button3; }
            if (dwButtons & LGLCDBUTTON_BUTTON4) { buttonStates |= ButtonState::Button4; }
            if (dwButtons & LGLCDBUTTON_BUTTON5) { buttonStates |= ButtonState::Button5; }
            if (dwButtons & LGLCDBUTTON_BUTTON6) { buttonStates |= ButtonState::Button6; }
            if (dwButtons & LGLCDBUTTON_BUTTON7) { buttonStates |= ButtonState::Button7; }

            if (dwButtons & LGLCDBUTTON_LEFT)   { buttonStates |= ButtonState::Left; }
            if (dwButtons & LGLCDBUTTON_RIGHT)  { buttonStates |= ButtonState::Right; }
            if (dwButtons & LGLCDBUTTON_UP)     { buttonStates |= ButtonState::Up; }
            if (dwButtons & LGLCDBUTTON_DOWN)   { buttonStates |= ButtonState::Down; }
            if (dwButtons & LGLCDBUTTON_OK)     { buttonStates |= ButtonState::Ok; }
            if (dwButtons & LGLCDBUTTON_CANCEL) { buttonStates |= ButtonState::Cancel; }
            if (dwButtons & LGLCDBUTTON_MENU)   { buttonStates |= ButtonState::Menu; }

            return buttonStates;
        } else {
            return ButtonState::None;
        }
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnDeviceArrival(_In_ lgNotifyParam deviceType) noexcept {
        SafeLogInfo(m_Device ? "Device was connected; new device available." : "Device was connected; initiating device.");
        if (!m_Device) {
            SafeOnNotification(::LCD::Device::Notification::DeviceConnected);
            SafeLogInfo("Device was connected; initiating device.");
            switch (deviceType) {
                case LGLCD_DEVICE_BW:   SafeConnect(DeviceType::Monochrome); break;
                case LGLCD_DEVICE_QVGA: SafeConnect(DeviceType::Color);      break;
                default:
                    SafeLogDebug("Unrecognised device type \"{}\" connected.", deviceType);
                    break;
            }
        }
        return m_Device ? ERROR_SUCCESS : ERROR_APP_INIT_FAILURE;
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnDeviceRemoval(_In_ lgNotifyParam deviceType) noexcept {
        if (m_Device) {
            switch (deviceType) {
                case LGLCD_DEVICE_BW: {
                    if (GetDeviceType() == DeviceType::Monochrome) {
                        SafeLogInfo("Device was disconnected; resetting device.");
                        SafeOnNotification(::LCD::Device::Notification::DeviceDisconnected);
                        Disconnect();
                    }
                    break;
                }

                case LGLCD_DEVICE_QVGA: {
                    if (GetDeviceType() == DeviceType::Color) {
                        SafeLogInfo("Device was disconnected; resetting device.");
                        SafeOnNotification(::LCD::Device::Notification::DeviceDisconnected);
                        Disconnect();
                    }
                    break;
                }

                default:
                    SafeLogDebug("Unrecognised device type \"{}\" disconnected.", deviceType);
                    break;
            }
        }
        return ERROR_SUCCESS;
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnCloseConnection() noexcept {
        SafeLogInfo("Connection was closed; resetting device and connection.");
        Disconnect();
        if (m_Connection) { m_Connection.Disconnect(); }
        return ERROR_SUCCESS;
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnAppletDisabled() noexcept {
        SafeLogInfo("Applet was disabled; disabling updates.");
        SafeOnNotification(::LCD::Device::Notification::AppletDisabled);
        m_bAppletEnabled = false;
        return ERROR_SUCCESS;
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnAppletEnabled() noexcept {
        SafeLogInfo(m_Device ? "Applet was enabled; enabling updates." : "Applet was enabled; initiating device.");
        SafeOnNotification(::LCD::Device::Notification::AppletEnabled);
        m_bAppletEnabled = true;
        return ERROR_SUCCESS;
    }

    //--------------------------------------------------------------------------

    LogitechLCD::lgNotifyStatus LogitechLCD::OnTerminateApplet() noexcept {
        if (m_Device) {
            SafeLogInfo("Applet was terminated; resetting device.");
            Disconnect();
        }
        return ERROR_SUCCESS;
    }
} // namespace LCD::Logitech
