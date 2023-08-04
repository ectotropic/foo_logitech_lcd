#pragma once
#ifndef GUID_7A8E00D1_532B_4D54_9DA8_2791D8774A86
#define GUID_7A8E00D1_532B_4D54_9DA8_2791D8774A86
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
#include "Windows/Windows_Core.h"
//--------------------------------------

//--------------------------------------
//
#include <debugapi.h> // OutputDebugString, DebugBreak, IsDebuggerPresent
//--------------------------------------

//--------------------------------------
//
#include <cassert>
#include <cstring> // std::strerror
#include <utility> // std::exchange
//--------------------------------------

//--------------------------------------
//
#ifndef SPDLOG_API
#   ifndef SPDLOG_ACTIVE_LEVEL
#       if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#           define SPDLOG_ACTIVE_LEVEL  SPDLOG_LEVEL_DEBUG
#       else
#           define SPDLOG_ACTIVE_LEVEL  SPDLOG_LEVEL_WARN
#       endif
#   endif

#   if defined(UNICODE) || defined(_UNICODE)
#       define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#   endif

#   ifndef FMT_HEADER_ONLY
#       define FMT_HEADER_ONLY
#   endif

#   include <spdlog/spdlog.h>
#endif
//--------------------------------------

namespace Windows::detail {
    //**************************************************************************
    // ErrorStringT
    //**************************************************************************
    template <typename CharT>
    class ErrorStringT final {
    public:
        using charType = CharT;

    private:
        // No default construction or copy
        ErrorStringT()                               = delete;
        ErrorStringT(const ErrorStringT&)            = delete;
        ErrorStringT& operator=(const ErrorStringT&) = delete;

    public:
        explicit ErrorStringT(_In_ DWORD dwError) noexcept {
            ::Windows::FormatMessageFromError(dwError, m_ErrorString);
        }

        constexpr ErrorStringT(ErrorStringT&& other) noexcept :
            m_ErrorString{ std::exchange(other.m_ErrorString, nullptr) } {}

        constexpr ErrorStringT& operator=(ErrorStringT&& other) noexcept {
            m_ErrorString = std::exchange(other.m_ErrorString, nullptr);
            return *this;
        }

        ~ErrorStringT() noexcept {
            if (m_ErrorString) {
                ::Windows::FreeFormatMessageBuffer(m_ErrorString);
            }
        }

    public:
        constexpr const charType* const get() const noexcept { return m_ErrorString; }

    public:
        constexpr operator const charType* const() const noexcept { return m_ErrorString; }

        explicit constexpr operator bool() const noexcept { return m_ErrorString; }

    private:
        charType* m_ErrorString{ nullptr };
    }; // template <...> class ErrorStringT final

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    using ErrorStringA = ErrorStringT<CHAR>;
    using ErrorStringW = ErrorStringT<WCHAR>;
    using ErrorString  = ErrorStringT<TCHAR>;
} // namespace detail

//==============================================================================

namespace Windows {
    //**************************************************************************
    // Debug Message Helpers
    //**************************************************************************
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
    inline void WriteDebugMessage(_In_z_ LPCSTR szMsg) noexcept {
        if (::IsDebuggerPresent()) {
            ::OutputDebugStringA(szMsg);
            ::OutputDebugStringA("\r\n");
        }
    }

    //--------------------------------------------------------------------------

    inline void WriteDebugMessage(_In_z_ LPCWSTR szMsg) noexcept {
        if (::IsDebuggerPresent()) {
            ::OutputDebugStringW(szMsg);
            ::OutputDebugStringW(L"\r\n");
        }
    }
#else
    inline constexpr void WriteDebugMessage(_In_z_ LPCSTR ) noexcept {};
    inline constexpr void WriteDebugMessage(_In_z_ LPCWSTR) noexcept {};
#endif

    //**************************************************************************
    // Error String Helpers: errno
    //**************************************************************************
    inline decltype(auto) GetErrnoStringA(_In_ int errnum = errno) noexcept {
        return std::strerror(errnum);
    }

    //--------------------------------------------------------------------------

    inline decltype(auto) GetErrnoStringW(_In_ int errnum = errno) noexcept {
        return ::_wcserror(errnum);
    }

    //--------------------------------------------------------------------------

#if defined(UNICODE) || defined(_UNICODE)
#   define GetErrnoString GetErrnoStringW
#else
#   define GetErrnoString GetErrnoStringA
#endif

    //**************************************************************************
    // Error String Helpers: Windows Error Codes
    //**************************************************************************

    _Success_(return != 0)
    BOOL FormatMessageFromError(_In_ DWORD dwError,
                                _When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                LPSTR& lpBuffer,
                                _In_ DWORD cchBuffer = 0) noexcept;

    //--------------------------------------------------------------------------

    _Success_(return != 0)
    BOOL FormatMessageFromError(_In_ DWORD dwError,
                                _When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                LPWSTR& lpBuffer,
                                _In_ DWORD cchBuffer = 0) noexcept;

    //--------------------------------------------------------------------------

    inline decltype(auto) FormatMessageFromLastError(_When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                                     _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                                     LPSTR& lpBuffer,
                                                     _In_ DWORD cchBuffer = 0) noexcept {
        return FormatMessageFromError(::GetLastError(),
                                      lpBuffer, cchBuffer);
    }

    //--------------------------------------------------------------------------

    inline decltype(auto) FormatMessageFromLastError(_When_(cchBuffer == 0, _At_(&lpBuffer, _Outptr_result_z_))
                                                     _When_(cchBuffer != 0, _Out_writes_z_(cchBuffer))
                                                     LPWSTR& lpBuffer,
                                                     _In_ DWORD cchBuffer = 0) noexcept {
        return FormatMessageFromError(::GetLastError(),
                                      lpBuffer, cchBuffer);
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    inline auto FreeFormatMessageBuffer(_Notref_ _Frees_ptr_opt_ LPSTR& lpBuffer) noexcept {
        const auto res{ ::LocalFree(lpBuffer) == NULL };
        assert(res); //< Don't use complex asserts with functions used to implement those asserts!
        lpBuffer = NULL;
        return res;
    }

    //--------------------------------------------------------------------------

    inline auto FreeFormatMessageBuffer(_Notref_ _Frees_ptr_opt_ LPWSTR& lpBuffer) noexcept {
        const auto res{ ::LocalFree(lpBuffer) == NULL };
        assert(res); //< Don't use complex asserts with functions used to implement those asserts!
        lpBuffer = NULL;
        return res;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) FormatMessageFromErrorA(_In_ DWORD dwError = ::GetLastError()) noexcept {
        return ::Windows::detail::ErrorStringA{ dwError };
    }

    //--------------------------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) FormatMessageFromErrorW(_In_ DWORD dwError = ::GetLastError()) noexcept {
        return ::Windows::detail::ErrorStringW{ dwError };
    }

    //--------------------------------------------------------------------------

#if defined(UNICODE) || defined(_UNICODE)
    inline [[nodiscard]]
    decltype(auto) FormatMessageFromError(_In_ DWORD dwError = ::GetLastError()) noexcept {
        return ::Windows::FormatMessageFromErrorW(dwError);
    }
#else
    inline [[nodiscard]]
    decltype(auto) FormatMessageFromError(_In_ DWORD dwError = ::GetLastError()) noexcept {
        return ::Windows::FormatMessageFromErrorA(dwError);
    }
#endif

    //--------------------------------------------------------------------------

    // Due to the automatic util management used for the
    // error message strings, FormatMessageFromError returns
    // an opaque object which, although convertible to string,
    // can cause issues with overloaded functions. These aliases
    // make it easier to use, but **must** be pre-processor
    // definitions or the values will be invalid.
#   define GetErrorStringA(...) static_cast<LPCSTR> (::Windows::FormatMessageFromErrorA(__VA_ARGS__))
#   define GetErrorStringW(...) static_cast<LPWCSTR>(::Windows::FormatMessageFromErrorW(__VA_ARGS__))

#if defined(UNICODE) || defined(_UNICODE)
#   define GetErrorString GetErrorStringW
#else
#   define GetErrorString GetErrorStringA
#endif
} // namespace Windows

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//  NOTE: The use of `try...catch` is
//        for code analysis.
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define DETAIL_WinAPIAssert_Message(_, msg, ...) msg

#   define DETAIL_WinAPIAssert_DoAppend(_, _1, ...)         _1, ## __VA_ARGS__
#   define DETAIL_WinAPIAssert_Append(_1, ...)              DETAIL_WinAPIAssert_DoAppend(__VA_ARGS__, _1)

#   define DETAIL_WinAPIAssert_LogError(code, msg, ...) \
            const auto lastErrorMessage_{ ::Windows::FormatMessageFromErrorA(code) }; \
            try {\
                SPDLOG_ERROR(\
                    "Assertion Failed (Windows): `" ## msg ## "` (GetLastError: \"{}\" [{}])", \
                    DETAIL_WinAPIAssert_Append(lastErrorMessage_.get(), ## __VA_ARGS__),\
                    code \
                ); \
            } catch(...) {} \
            assert(code && false);

#   define DETAIL_WinAPIAssert(expr, msg, ...) \
        do { \
            if (expr) { break; } \
            const auto lastError_{ ::GetLastError() }; \
            DETAIL_WinAPIAssert_LogError(lastError_, msg, ## __VA_ARGS__) \
        } while(0)

#   define DETAIL_WinAPIAssertErrorSuccess(msg, ...) \
        do { \
            const auto lastError_{ ::GetLastError() }; \
            if (lastError_ == ERROR_SUCCESS) { break; } \
            DETAIL_WinAPIAssert_LogError(lastError_, msg, ## __VA_ARGS__) \
        } while(0)

#   define DETAIL_WinAPIAssertHResult(hr, msg, ...) \
        do { \
            const HRESULT hResultCode_{ hr }; \
            if (hResultCode_ == S_OK) { break; } \
            DETAIL_WinAPIAssert_LogError(static_cast<DWORD>(hResultCode_), msg, ## __VA_ARGS__) \
        } while(0)

#   define WinAPIAssert(expr, ...)          \
        DETAIL_WinAPIAssert(expr, DETAIL_WinAPIAssert_Message(expr, ## __VA_ARGS__, #expr), ## __VA_ARGS__)

#   define WinAPIAssertErrorSuccess(...)    \
        DETAIL_WinAPIAssertErrorSuccess(DETAIL_WinAPIAssert_Message(_, ## __VA_ARGS__, "GetLastError() == ERROR_SUCCESS"), ## __VA_ARGS__)

#   define WinAPIAssertHResult(hr, ...)     \
        DETAIL_WinAPIAssertHResult(hr, DETAIL_WinAPIAssert_Message(hr, ## __VA_ARGS__, "HRESULT == S_OK"), ## __VA_ARGS__)

#   define WinAPIVerify                     WinAPIAssert
#   define WinAPIVerifyHResult              WinAPIAssertHResult
#else // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#   define WinAPIAssert(...)                ((void)0)
#   define WinAPIAssertErrorSuccess(...)    ((void)0)
#   define WinAPIAssertHResult(...)         ((void)0)
#   define WinAPIVerify(expr, ...)          ((void)(expr))
#   define WinAPIVerifyHResult(expr, ...)   ((void)(expr))
#endif // #if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
//--------------------------------------

//--------------------------------------
//
#define WinAPIAssertHR  WinAPIAssertHResult
#define WinAPIVerifyHR  WinAPIVerifyHResult
//--------------------------------------

#endif // GUID_7A8E00D1_532B_4D54_9DA8_2791D8774A86