#pragma once
#ifndef GUID_96C98F80_9B97_4154_BFA6_5479C1094A31
#define GUID_96C98F80_9B97_4154_BFA6_5479C1094A31
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
#include <cerrno>
#include <cstring>
#include <cwchar>

#include <type_traits>
//--------------------------------------

namespace Windows::detail {
    //**************************************************************************
    // StrToFltT: StringToFloat Helper
    //**************************************************************************
    template <typename FloatT>
    struct StrToFltT final {};

    //------------------------------------------------------

    // StrToFltT: `float` specialisation
    template <>
    struct StrToFltT<float> final {
        static inline float StringToFloat(_In_z_ LPCSTR szFloat,
                                          _Out_ LPSTR* pEnd) noexcept {
            return std::strtof(szFloat, pEnd);
        }

        static inline float StringToFloat(_In_z_ LPCWSTR szFloat,
                                          _Out_ LPWSTR* pEnd) noexcept {
            return std::wcstof(szFloat, pEnd);
        }
    }; // template <...> struct StrToFltT<float> final

    //------------------------------------------------------

    // StrToFltT: `double` specialisation
    template <>
    struct StrToFltT<double> final {
        static inline double StringToFloat(_In_z_ LPCSTR szFloat,
                                           _Out_ LPSTR* pEnd) noexcept {
            return std::strtod(szFloat, pEnd);
        }

        static inline double StringToFloat(_In_z_ LPCWSTR szFloat,
                                           _Out_ LPWSTR* pEnd) noexcept {
            return std::wcstod(szFloat, pEnd);
        }
    }; // template <...> struct StrToFltT<double> final

    //**************************************************************************
    // StrToIntT: StringToInt Helper
    //**************************************************************************
    template <typename IntT, typename EnableT = bool>
    struct StrToIntT {};

    //------------------------------------------------------

    // StrToFltT: `LONG` specialisation
    template <typename IntT>
    struct StrToIntT<
        IntT,
        std::enable_if_t<
            (sizeof(IntT) < sizeof(LONG)) || std::is_same_v<IntT, LONG>,
            bool
        >
    > {
        static inline IntT StringToInt(_In_z_ LPCSTR szInt,
                                        _Out_ LPSTR* pEnd,
                                        _In_ INT base) noexcept {
            return static_cast<IntT>(std::strtol(szInt, pEnd, base));
        }

        static inline IntT StringToInt(_In_z_ LPCWSTR szInt,
                                        _Out_ LPWSTR* pEnd,
                                        _In_ INT base) noexcept {
            return static_cast<IntT>(std::wcstol(szInt, pEnd, base));
        }
    };

    //------------------------------------------------------

    // StrToFltT: `LONGLONG` specialisation
    template <typename IntT>
    struct StrToIntT<
        IntT,
        std::enable_if_t<
            std::is_same_v<IntT, LONGLONG> || std::is_same_v<IntT, ULONG>,
            bool
        >
    > {
        static inline IntT StringToInt(_In_z_ LPCSTR szInt,
                                        _Out_ LPSTR* pEnd,
                                        _In_ INT base) noexcept {
            return static_cast<IntT>(std::strtoll(szInt, pEnd, base));
        }

        static inline IntT StringToInt(_In_z_ LPCWSTR szInt,
                                        _Out_ LPWSTR* pEnd,
                                        _In_ INT base) noexcept {
            return static_cast<IntT>(std::wcstoll(szInt, pEnd, base));
        }
    };

} // namespace Windows::detail

//==============================================================================

namespace Windows {
    //**************************************************************************
    // StringCompare
    //**************************************************************************
    inline auto StringCompare(_In_z_ LPCSTR sz1,
                              _In_z_ LPCSTR sz2) noexcept {
        return std::strcmp(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringCompare(_In_z_ LPCWSTR sz1,
                              _In_z_ LPCWSTR sz2) noexcept {
        return std::wcscmp(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringCompare(__in_ecount_z(cch) LPCSTR sz1,
                              __in_ecount_z(cch) LPCSTR sz2,
                              _In_ SIZE_T cch) noexcept {
        return std::strncmp(sz1, sz2, cch);
    }

    //------------------------------------------------------

    inline auto StringCompare(__in_ecount_z(cch) LPCWSTR sz1,
                              __in_ecount_z(cch) LPCWSTR sz2,
                              _In_ SIZE_T cch) noexcept {
        return std::wcsncmp(sz1, sz2, cch);
    }

    //**************************************************************************
    // StringICompare
    //**************************************************************************
    inline auto StringICompare(_In_z_ LPCSTR sz1,
                               _In_z_ LPCSTR sz2) noexcept {
        return ::_stricmp(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringICompare(_In_z_ LPCWSTR sz1,
                               _In_z_ LPCWSTR sz2) noexcept {
        return ::_wcsicmp(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringICompare(__in_ecount_z(cch) LPCSTR sz1,
                               __in_ecount_z(cch) LPCSTR sz2,
                               _In_ SIZE_T cch) noexcept {
        return ::_strnicmp(sz1, sz2, cch);
    }

    //------------------------------------------------------

    inline auto StringICompare(__in_ecount_z(cch) LPCWSTR sz1,
                               __in_ecount_z(cch) LPCWSTR sz2,
                               _In_ SIZE_T cch) noexcept {
        return ::_wcsnicmp(sz1, sz2, cch);
    }

    //**************************************************************************
    // StringCopy
    //**************************************************************************
    inline auto StringCopy(_Out_writes_z_(_String_length_(sz2) + 1) LPSTR sz1,
                           _In_z_ LPCSTR sz2) noexcept {
        return std::strcpy(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringCopy(_Out_writes_z_(_String_length_(sz2) + 1) LPWSTR sz1,
                           _In_z_ LPCWSTR sz2) noexcept {
        return std::wcscpy(sz1, sz2);
    }

    //------------------------------------------------------

    inline auto StringCopy(_Out_writes_(cch) _Post_maybez_ LPSTR sz1,
                           _In_z_ LPCSTR sz2,
                           _In_ SIZE_T cch) noexcept {
        return std::strncpy(sz1, sz2, cch);
    }

    //------------------------------------------------------

    inline auto StringCopy(_Out_writes_(cch) _Post_maybez_ LPWSTR sz1,
                           _In_reads_or_z_(cch)  LPCWSTR sz2,
                           _In_ SIZE_T cch) noexcept {
        return std::wcsncpy(sz1, sz2, cch);
    }

    //**************************************************************************
    // StringLength
    //**************************************************************************
    inline auto StringLength(_In_z_ LPCSTR szString) noexcept {
        return std::strlen(szString);
    }

    //------------------------------------------------------

    inline auto StringLength(_In_z_ LPCWSTR szString) noexcept {
        return std::wcslen(szString);
    }


    //**************************************************************************
    // StringToFloat
    //**************************************************************************
    inline auto StringToFloat(_In_z_ LPCSTR szFloat,
                              _Out_ LPSTR* pEnd) noexcept {
        using helper_type = ::Windows::detail::StrToFltT<float>;
        return helper_type::StringToFloat(szFloat, pEnd);
    }

    //------------------------------------------------------

    inline auto StringToFloat(_In_z_ LPCWSTR szFloat,
                              _Out_ LPWSTR* pEnd) noexcept {
        using helper_type = ::Windows::detail::StrToFltT<float>;
        return helper_type::StringToFloat(szFloat, pEnd);
    }

    //------------------------------------------------------


    inline auto StringToDouble(_In_z_ LPCSTR szFloat,
                               _Out_ LPSTR* pEnd) noexcept {
        using helper_type = ::Windows::detail::StrToFltT<double>;
        return helper_type::StringToFloat(szFloat, pEnd);
    }

    //------------------------------------------------------

    inline auto StringToDouble(_In_z_ LPCWSTR szFloat,
                               _Out_ LPWSTR* pEnd) noexcept {
        using helper_type = ::Windows::detail::StrToFltT<double>;
        return helper_type::StringToFloat(szFloat, pEnd);
    }

    //------------------------------------------------------

    template <typename CharT, typename FloatT>
    inline BOOL StringToFloat(_In_z_ FAR const CharT* szFloat,
                              _Out_ FloatT& fValue) noexcept {
        using helper_type = ::Windows::detail::StrToFltT<FloatT>;
        ::_set_errno(0);
        FAR CharT* pEnd{ nullptr };
        fValue = helper_type::StringToFloat(szFloat, &pEnd);
        errno_t err;
        ::_get_errno(&err);
        if (err == ERANGE || !pEnd || *pEnd != '\0') {
            ::SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        return TRUE;
    }

    //**************************************************************************
    // StringToInt
    //**************************************************************************
    inline auto StringToLong(_In_z_ LPCSTR szInt,
                             _Out_ LPSTR* pEnd,
                             _In_ INT base = 10) noexcept {
        using helper_type = ::Windows::detail::StrToIntT<LONG>;
        return helper_type::StringToInt(szInt, pEnd, base);
    }

    //------------------------------------------------------

    inline auto StringToLong(_In_z_ LPCWSTR szInt,
                             _Out_ LPWSTR* pEnd,
                             _In_ INT base = 10) noexcept {
        using helper_type = ::Windows::detail::StrToIntT<LONG>;
        return helper_type::StringToInt(szInt, pEnd, base);
    }

    //------------------------------------------------------

    inline auto StringToLongLong(_In_z_ LPCSTR szInt,
                                 _Out_ LPSTR* pEnd,
                                 _In_ INT base = 10) noexcept {
        using helper_type = ::Windows::detail::StrToIntT<LONGLONG>;
        return helper_type::StringToInt(szInt, pEnd, base);
    }

    //------------------------------------------------------

    inline auto StringToLongLong(_In_z_ LPCWSTR szInt,
                                 _Out_ LPWSTR* pEnd,
                                 _In_ INT base = 10) noexcept {
        using helper_type = ::Windows::detail::StrToIntT<LONGLONG>;
        return helper_type::StringToInt(szInt, pEnd, base);
    }

    //------------------------------------------------------

    template <typename CharT, typename IntT>
    inline BOOL StringToInt(_In_z_ FAR const CharT* szInt,
                            _Out_ IntT& nValue,
                            _In_ INT base = 10) noexcept {
        using helper_type = ::Windows::detail::StrToIntT<IntT>;
        ::_set_errno(0);
        FAR CharT* pEnd{ nullptr };
        nValue = helper_type::StringToInt(szInt, &pEnd);
        errno_t err;
        ::_get_errno(&err);
        if (err == ERANGE || !pEnd || *pEnd != '\0') {
            ::SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        return TRUE;
    }
} // namespace Windows

#endif // GUID_96C98F80_9B97_4154_BFA6_5479C1094A31