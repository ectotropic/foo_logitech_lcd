#pragma once
#ifndef GUID_8F643DDD_2A2B_418F_8E89_E7138F348374
#define GUID_8F643DDD_2A2B_418F_8E89_E7138F348374
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
#include "Windows/GDIPlus/GDIPlus.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
#include <type_traits>
//--------------------------------------

namespace Windows::GDIPlus::detail {
    //**************************************************************************
    // LastStatusT
    //**************************************************************************
    template <typename ObjectT>
    struct LastStatusT final {
        using status = ::Gdiplus::Status;

        static status LastStatus(const ObjectT& obj) noexcept {
            try {
                return obj.GetLastStatus();
            } catch (...) {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }

        static status LastStatus(const ObjectT* obj) noexcept {
            if (obj) {
                return obj->GetLastStatus();
            } else {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }
    }; // struct LastStatusT final

    //--------------------------------------------------------------------------
    // LastStatusT: Specialisation for `std::unique_ptr`
    //--------------------------------------------------------------------------
    template <typename ObjectT>
    struct LastStatusT<std::unique_ptr<ObjectT>> final {
        using status = ::Gdiplus::Status;

        static status LastStatus(const std::unique_ptr<ObjectT>& obj) noexcept {
            try {
                return obj->GetLastStatus();
            } catch (...) {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }

        static status LastStatus(const std::unique_ptr<ObjectT>* obj) noexcept {
            if (obj) {
                return LastStatus(*obj);
            } else {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }
    }; // struct LastStatusT<...> final

    //--------------------------------------------------------------------------
    // LastStatusT: Specialisation for `std::shared_ptr`
    //--------------------------------------------------------------------------
    template <typename ObjectT>
    struct LastStatusT<std::shared_ptr<ObjectT>> final {
        using status = ::Gdiplus::Status;

        static status LastStatus(const std::shared_ptr<ObjectT>& obj) noexcept {
            try {
                return obj->GetLastStatus();
            } catch (...) {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }

        static status LastStatus(const std::shared_ptr<ObjectT>* obj) noexcept {
            if (obj) {
                return LastStatus(*obj);
            } else {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }
    }; // struct LastStatusT<...> final

    //--------------------------------------------------------------------------
    // LastStatusT: Specialisation for `::Gdiplus::Status`
    //--------------------------------------------------------------------------
    template <>
    struct LastStatusT<::Gdiplus::Status> final {
        static auto LastStatus(const ::Gdiplus::Status& status) noexcept {
            return status;
        }

        static auto LastStatus(const ::Gdiplus::Status* status) noexcept {
            if (status) {
                return *status;
            } else {
                assert(false);
                return ::Gdiplus::InvalidParameter;
            }
        }
    }; // struct LastStatusT<...> final
} // namespace Windows::GDIPlus::detail

//==============================================================================

namespace Windows::GDIPlus {
    //**************************************************************************
    // GetStatusStringA
    //**************************************************************************
    inline const char* const GetStatusStringA(::Gdiplus::Status status) noexcept {
        switch (status) {
        case ::Gdiplus::Ok:                         return "Ok";
        case ::Gdiplus::GenericError:               return "GenericError";
        case ::Gdiplus::InvalidParameter:           return "InvalidParameter";
        case ::Gdiplus::OutOfMemory:                return "OutOfMemory";
        case ::Gdiplus::ObjectBusy:                 return "ObjectBusy";
        case ::Gdiplus::InsufficientBuffer:         return "InsufficientBuffer";
        case ::Gdiplus::NotImplemented:             return "NotImplemented";
        case ::Gdiplus::Win32Error:                 return "Win32Error";
        case ::Gdiplus::WrongState:                 return "WrongState";
        case ::Gdiplus::Aborted:                    return "Aborted";
        case ::Gdiplus::FileNotFound:               return "FileNotFound";
        case ::Gdiplus::ValueOverflow:				return "ValueOverflow";
        case ::Gdiplus::AccessDenied:               return "AccessDenied";
        case ::Gdiplus::UnknownImageFormat:			return "UnknownImageFormat";
        case ::Gdiplus::FontFamilyNotFound:			return "FontFamilyNotFound";
        case ::Gdiplus::FontStyleNotFound:			return "FontStyleNotFound";
        case ::Gdiplus::NotTrueTypeFont:            return "NotTrueTypeFont";
        case ::Gdiplus::UnsupportedGdiplusVersion:  return "UnsupportedGdiplusVersion";
        case ::Gdiplus::PropertyNotFound:           return "PropertyNotFound";
        case ::Gdiplus::PropertyNotSupported:       return "PropertyNotSupported";
#if (GDIPVER >= 0x0110)
        case ::Gdiplus::ProfileNotFound:            return "ProfileNotFound";
#endif //(GDIPVER >= 0x0110)
        default:
            assert(false);
            return "Unknown/NotAvailable";
        }
    }

    //**************************************************************************
    // GetStatusStringW
    //**************************************************************************
    inline const wchar_t* const GetStatusStringW(::Gdiplus::Status status) noexcept {
        switch (status) {
        case ::Gdiplus::Ok:                         return L"Ok";
        case ::Gdiplus::GenericError:               return L"GenericError";
        case ::Gdiplus::InvalidParameter:           return L"InvalidParameter";
        case ::Gdiplus::OutOfMemory:                return L"OutOfMemory";
        case ::Gdiplus::ObjectBusy:                 return L"ObjectBusy";
        case ::Gdiplus::InsufficientBuffer:         return L"InsufficientBuffer";
        case ::Gdiplus::NotImplemented:             return L"NotImplemented";
        case ::Gdiplus::Win32Error:                 return L"Win32Error";
        case ::Gdiplus::WrongState:                 return L"WrongState";
        case ::Gdiplus::Aborted:                    return L"Aborted";
        case ::Gdiplus::FileNotFound:               return L"FileNotFound";
        case ::Gdiplus::ValueOverflow:				return L"ValueOverflow";
        case ::Gdiplus::AccessDenied:               return L"AccessDenied";
        case ::Gdiplus::UnknownImageFormat:			return L"UnknownImageFormat";
        case ::Gdiplus::FontFamilyNotFound:			return L"FontFamilyNotFound";
        case ::Gdiplus::FontStyleNotFound:			return L"FontStyleNotFound";
        case ::Gdiplus::NotTrueTypeFont:            return L"NotTrueTypeFont";
        case ::Gdiplus::UnsupportedGdiplusVersion:  return L"UnsupportedGdiplusVersion";
        case ::Gdiplus::PropertyNotFound:           return L"PropertyNotFound";
        case ::Gdiplus::PropertyNotSupported:       return L"PropertyNotSupported";
#if (GDIPVER >= 0x0110)
        case ::Gdiplus::ProfileNotFound:            return L"ProfileNotFound";
#endif //(GDIPVER >= 0x0110)
        default:
            assert(false);
            return L"Unknown/NotAvailable";
        }
    }

    //**************************************************************************
    // GetStatusString
    //**************************************************************************
#if defined(UNICODE) || defined(_UNICODE)
    inline decltype(auto) GetStatusString(::Gdiplus::Status status) noexcept {
        return GetStatusStringW(status);
    }
#else
    inline decltype(auto) GetStatusString(::Gdiplus::Status status) noexcept {
        return GetStatusStringA(status);
    }
#endif

    //**************************************************************************
    // GetLastStatus
    //**************************************************************************
    template <typename ObjectT>
    inline decltype(auto) GetLastStatus(ObjectT& obj) noexcept {
        using object_type = std::remove_cv_t<ObjectT>;
        using impl_type   = ::Windows::GDIPlus::detail::LastStatusT<object_type>;
        return impl_type::LastStatus(obj);
    }

    //--------------------------------------------------------------------------

    template <typename ObjectT>
    inline decltype(auto) GetLastStatus(ObjectT* obj) noexcept {
        using object_type = std::remove_cv_t<ObjectT>;
        using impl_type = ::Windows::GDIPlus::detail::LastStatusT<object_type>;
        return impl_type::LastStatus(obj);
    }
}

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define DETAIL_WinGDIPlusAssert_Append       DETAIL_WinAPIAssert_Append
#   define DETAIL_WinGDIPlusAssert_Message      DETAIL_WinAPIAssert_Message

#   define DETAIL_WinGDIPlusAssertStatus(expr, msg, ...) \
        do { \
            const auto gdiStatus_ = ::Windows::GDIPlus::GetLastStatus(expr); \
            if (gdiStatus_ == ::Gdiplus::Ok) { break; } \
            const auto lastErrorMessage_{ ::Windows::GDIPlus::GetStatusStringA(gdiStatus_) }; \
            try {\
                SPDLOG_ERROR(\
                    "Assertion Failed (Windows): `" ## msg ## "` (GDI+ Status: \"{}\" [{}])", \
                    DETAIL_WinGDIPlusAssert_Append(lastErrorMessage_, ## __VA_ARGS__),\
                    static_cast<std::underlying_type_t<::Gdiplus::Status>>(gdiStatus_) \
                ); \
            } catch(...) {} \
            assert(gdiStatus_ == ::Gdiplus::Ok); \
        } while(0)

#   define WinGDIPlusAssertStatus(expr, ...) \
        DETAIL_WinGDIPlusAssertStatus(expr, DETAIL_WinGDIPlusAssert_Message(expr, ## __VA_ARGS__, #expr), ## __VA_ARGS__)

#   define WinGDIPlusVerifyStatus             WinGDIPlusAssertStatus
#else // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define WinGDIPlusAssertStatus(...)        ((void)0)
#   define WinGDIPlusVerifyStatus(expr, ...)  ((void)(expr))
#endif // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
//--------------------------------------

//--------------------------------------
//
#define WinGDIPlusAssert    WinAPIAssert
#define WinGDIPlusVerify    WinAPIVerify
//--------------------------------------

#endif // GUID_8F643DDD_2A2B_418F_8E89_E7138F348374