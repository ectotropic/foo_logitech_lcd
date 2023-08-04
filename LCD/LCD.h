#pragma once
#ifndef GUID_C3EDF8BC_00B7_481F_99AA_FD14F2C030DD
#define GUID_C3EDF8BC_00B7_481F_99AA_FD14F2C030DD
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
#include "Util/FlagEnum.h"
#include "Util/Singleton.h"
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <memory>
#include <functional>
//--------------------------------------

namespace LCD {
    //**************************************************************************
    // Forward Declarations
    //**************************************************************************
    class ILCD;

    //**************************************************************************
    // Device
    //**************************************************************************
    struct Device final {

        //------------------------------

        struct Notification final {
            enum What {
                DeviceConnected = 0,
                DeviceDisconnected,
                AppletEnabled,
                AppletDisabled,

                Error = 0xFFFFFFFF,
            };

            //--------------------------

            using callback_type =
                std::function<void(Notification::What,
                                   void*,
                                   const ::LCD::ILCD&)>;
        }; // struct Notification final

        //------------------------------

        enum class Type {
            Unknown = 0,
            Monochrome,
            Color,

            Mono   = Monochrome,
            Colour = Color,
        }; // enum class Type

        //------------------------------

        struct Desc final {
            constexpr explicit operator bool() const noexcept {
                return (eType != Type::Unknown) &&
                    (iWidth > 0) && (iHeight > 0) &&
                    (iBitsPerPixel > 0);
            }

            Type eType        { Type::Unknown };
            int  iWidth       { 0 };
            int  iHeight      { 0 };
            int  iBitsPerPixel{ 0 };
            int  iButtonCount { 0 };
        }; // struct Desc final
    }; // struct Device final

    //----------------------------------

    enum class Priority {
        Idle = 0,
        Background,
        Foreground,
        Alert,

        NoShow = Idle,
    }; // enum class Priority

    //----------------------------------

    FLAG_ENUM(ButtonState,
              FLAG_ENUM_VALUES(_maskNumbered  = 0x00FF,
                               _maskNamed     = 0xFF00,
                               _shiftNumbered = 0,
                               _shiftNamed    = 16,

                               None = 0,

                               Button0 = 1 << (0 + _shiftNumbered),
                               Button1 = 1 << (1 + _shiftNumbered),
                               Button2 = 1 << (2 + _shiftNumbered),
                               Button3 = 1 << (3 + _shiftNumbered),
                               Button4 = 1 << (4 + _shiftNumbered),
                               Button5 = 1 << (5 + _shiftNumbered),
                               Button6 = 1 << (6 + _shiftNumbered),
                               Button7 = 1 << (7 + _shiftNumbered),

                               Left   = 1 << (0 + _shiftNamed),
                               Right  = 1 << (1 + _shiftNamed),
                               Up     = 1 << (2 + _shiftNamed),
                               Down   = 1 << (3 + _shiftNamed),
                               Ok     = 1 << (4 + _shiftNamed),
                               Cancel = 1 << (5 + _shiftNamed),
                               Menu   = 1 << (6 + _shiftNamed)));

    //**************************************************************************
    // ILCD
    //**************************************************************************
    class ILCD : public ::util::singleton<::LCD::ILCD> {
    public:
        using DeviceType           = ::LCD::Device::Type;
        using DeviceDesc           = ::LCD::Device::Desc;
        using ButtonState          = ::LCD::ButtonState;
        using Priority             = ::LCD::Priority;
        using Notification         = ::LCD::Device::Notification;
        using NotificationCode     = typename Notification::What;
        using NotificationCallback = typename Notification::callback_type;

        enum class UpdateStatus {
            Success = 0,
            Skipped,
            Failure,
        };

        FLAG_ENUM_NO_OPS(Flags,
                         FLAG_ENUM_VALUES(None = 0,
                                          ForcePriority = 1 << 0,
                                          PreferVSync = 1 << 1));
    protected:
        ILCD() noexcept = default;

    public:
        ILCD(const ILCD& ) = default;
        ILCD(      ILCD&&) = default;

        ILCD& operator=(const ILCD& ) = default;
        ILCD& operator=(      ILCD&&) = default;

        virtual ~ILCD() noexcept = default;

    public:
        bool Connect(DeviceType prefered,
                     void* pData = nullptr) {
            if (Connected()) {
                Disconnect(pData);
            }

            DeviceDesc desc{};
            if (OnConnect(desc, prefered, pData) && desc) {
                using std::swap;
                swap(m_Desc, desc);
                return true;
            } else {
                return false;
            }
        }

        bool SafeConnect(DeviceType prefered,
                         void* pData = nullptr) noexcept {
            try {
                return Connect(prefered, pData);
            } catch (...) {
                SafeLogError("Exception thrown during ILCD::Connect");
                assert(false);
                return false;
            }
        }

        bool Disconnect(void* pData = nullptr) noexcept {
            const auto status = OnDisconnect(pData);
            m_Desc = DeviceDesc{};
            return status;
        }

    protected:
        virtual bool OnConnect(DeviceDesc& desc,
                               DeviceType prefered,
                               void* data) = 0;

        virtual bool OnDisconnect(void* pData) noexcept = 0;

    public:
        virtual UpdateStatus Update(void* pData) = 0;

        // Get current state of all buttons
        virtual ButtonState GetButtons() const noexcept {
            return ButtonState::None;
        }

        // Get current state of buttons that
        // have changed since the given state
        virtual ButtonState GetButtons(ButtonState lastState) const noexcept {
            return GetButtons() & ~lastState;
        }

        virtual void SetFlags(Flags flags) noexcept {
            m_Flags = flags;
        }

        virtual void SetDisplayPriority(Priority priority) noexcept {
            m_Priority = priority;
        }

    public:
        void AddFlags   (Flags flags) noexcept;
        void RemoveFlags(Flags flags) noexcept;

        void SetPreferVSync(bool bPreferVSync) noexcept {
            if (bPreferVSync) {
                AddFlags(Flags::PreferVSync);
            } else {
                RemoveFlags(Flags::PreferVSync);
            }
        }

        void SetForceDisplayPriority(bool bForcePriority) noexcept {
            if (bForcePriority) {
                AddFlags(Flags::ForcePriority);
            } else {
                RemoveFlags(Flags::ForcePriority);
            }
        }

        void SetNotificationCallback(NotificationCallback pCallback) {
            m_NotificationCallback = pCallback;
        }

    public:
        constexpr auto GetDeviceDesc  () const noexcept { return m_Desc; }
        constexpr auto GetWidth       () const noexcept { return m_Desc.iWidth; }
        constexpr auto GetHeight      () const noexcept { return m_Desc.iHeight; }
        constexpr auto GetBitsPerPixel() const noexcept { return m_Desc.iBitsPerPixel; }
        constexpr auto GetButtonCount () const noexcept { return m_Desc.iButtonCount; }
        constexpr auto GetDeviceType  () const noexcept { return m_Desc.eType; }

        constexpr auto GetDisplayByteSize() const noexcept {
            return GetWidth() * GetHeight() * (GetBitsPerPixel() / 8);
        }

        constexpr auto GetDisplayPriority() const noexcept { return m_Priority; }
        constexpr auto GetFlags          () const noexcept { return m_Flags; }

        constexpr bool Connected() const noexcept {
            return static_cast<bool>(m_Desc);
        }

        explicit constexpr operator bool() const noexcept {
            return Connected();
        }

    protected:
        void OnNotification(NotificationCode what,
                            void* pData = nullptr) const {
            if (m_NotificationCallback) {
                m_NotificationCallback(what, pData, *this);
            }
        }

        void SafeOnNotification(NotificationCode what,
                                void* pData = nullptr) const noexcept {
            try {
                OnNotification(what, pData);
            } catch (...) {
                SafeLogError("Exception thrown during ILCD::OnNotification");
                assert(false);
            }
        }

    private: //Data
        DeviceDesc           m_Desc                { };
        Priority             m_Priority            { Priority::Foreground };
        Flags                m_Flags               { Flags::None };
        NotificationCallback m_NotificationCallback{ nullptr };
    }; // class ILCD

    //--------------------------------------------------------------------------

    FLAG_ENUM_OPERATORS(::LCD::ILCD::Flags);

    //--------------------------------------------------------------------------

    inline void ::LCD::ILCD::AddFlags(Flags flags) noexcept {
        SetFlags(GetFlags() | flags);
    }

    inline void ::LCD::ILCD::RemoveFlags(Flags flags) noexcept {
        SetFlags(GetFlags() & ~flags);
    }
} // namespace LCD

#endif //GUID_C3EDF8BC_00B7_481F_99AA_FD14F2C030DD
