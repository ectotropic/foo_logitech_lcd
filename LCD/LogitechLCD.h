#pragma once
#ifndef GUID_E784DC56_4A78_46B0_BFD6_DB4AF03442BC
#define GUID_E784DC56_4A78_46B0_BFD6_DB4AF03442BC
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
#include "LCD.h"
//--------------------------------------

//--------------------------------------
//
#include "LogitechAPI.h"
//--------------------------------------

namespace LCD::Logitech {
    //**************************************************************************
    // LogitechLCD
    //**************************************************************************
    class LogitechLCD final :
        public ::LCD::ILCD,
        public ::LCD::Logitech::NotifyHandler {
    private:
        using thisClass = LogitechLCD;
        using baseClass = ::LCD::ILCD;

    public:
        using lgNotifyHandler = ::LCD::Logitech::NotifyHandler;
        using lgAPI           = ::LCD::Logitech::API;
        using lgConnection    = ::LCD::Logitech::Connection;
        using lgDevice        = ::LCD::Logitech::Device;

        using lgNotifyStatus = typename lgNotifyHandler::Status;
        using lgNotifyParam  = typename lgNotifyHandler::Param;
        using lgBitmapDesc   = typename lgDevice::BitmapDesc;

    public:
        template <typename... ArgPackT>
        static void initialise(ArgPackT&& ...args) {
            baseClass::initialise<thisClass>(std::forward<ArgPackT>(args)...);
        }

    public:
        LogitechLCD(singleton_constructor_tag /*tag*/,
                    const char* szName,
                    bool bAutoStartable = false,
                    bool bPersistent = false) noexcept;

        LogitechLCD(singleton_constructor_tag /*tag*/,
                    const wchar_t* szName,
                    bool bAutoStartable = false,
                    bool bPersistent = false) noexcept;

        LogitechLCD(const LogitechLCD& ) = default;
        LogitechLCD(      LogitechLCD&&) = default;

        LogitechLCD& operator=(const LogitechLCD& ) = default;
        LogitechLCD& operator=(      LogitechLCD&&) = default;

    public:
        constexpr bool APIInitialised() const noexcept { return static_cast<bool>(m_API); }
        constexpr bool APIConnected  () const noexcept { return static_cast<bool>(m_Connection); }
        constexpr bool DeviceOpened  () const noexcept { return static_cast<bool>(m_Device); }

    public: // ::LCD::ILCD
        virtual bool         OnConnect         (DeviceDesc& desc,
                                                DeviceType prefered,
                                                void* data)                        override;
        virtual bool         OnDisconnect      (void* data)               noexcept override;
        virtual UpdateStatus Update            (void* pData)                       override;
        virtual ButtonState  GetButtons        ()                   const noexcept override;
        virtual void         SetDisplayPriority(Priority priority)        noexcept override;

    protected: // ::LCD::Logitech::NotifyHandler
        virtual lgNotifyStatus OnDeviceArrival  (_In_ lgNotifyParam /*deviceType*/) noexcept override;
        virtual lgNotifyStatus OnDeviceRemoval  (_In_ lgNotifyParam /*deviceType*/) noexcept override;
        virtual lgNotifyStatus OnCloseConnection()                                  noexcept override;
        virtual lgNotifyStatus OnAppletDisabled ()                                  noexcept override;
        virtual lgNotifyStatus OnAppletEnabled  ()                                  noexcept override;
        virtual lgNotifyStatus OnTerminateApplet()                                  noexcept override;

    private: //Methods
        bool ConnectToType(DeviceDesc& desc,
                           DeviceType type) noexcept;

    private: //Data
        lgBitmapDesc m_LGLcdBitmap   { 0 };
        lgAPI        m_API           { };
        lgConnection m_Connection    { };
        lgDevice     m_Device        { };
        bool         m_bAppletEnabled{ true };
    }; // class LogitechLCD final
} // namespace LCD::Logitech

#endif //LG_LCD_WRAPPER_HPP
