#pragma once
#ifndef GUID_111922C8_DC8D_45D2_AB41_176A3B436B7E
#define GUID_111922C8_DC8D_45D2_AB41_176A3B436B7E
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
#include "foobar/foobar_core_util.h"
#include "foobar/foobar_string_util.h"
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

#   if defined(_WIN32) && (defined(UNICODE) || defined(_UNICODE))
#       define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#   endif

#   ifndef FMT_HEADER_ONLY
#       define FMT_HEADER_ONLY
#   endif

#   include <spdlog/spdlog.h>
#endif
//--------------------------------------

//--------------------------------------
//
#include <array>
#include <string>
#include <utility>
#include <type_traits>
//--------------------------------------

namespace foobar::Config::detail {
    //**************************************************************************
    // ConfigTypeT
    //**************************************************************************

    //----------------------------------
    // Default
    template <class TypeT, class EnableT = void>
    struct ConfigTypeT {
        using value_type   = TypeT;
        using storage_type = typename value_type;
        using config_type  = cfg_int_t<TypeT>;
    };

    //----------------------------------
    // cfg_int_t pass through
    template <class TypeT>
    struct ConfigTypeT<cfg_int_t<TypeT>> {
        using value_type   = TypeT;
        using storage_type = typename value_type;
        using config_type  = cfg_int_t<TypeT>;
    };

    //----------------------------------
    // cfg_struct_t pass through
    template <class TypeT>
    struct ConfigTypeT<cfg_struct_t<TypeT>> {
        using value_type   = TypeT;
        using storage_type = typename value_type;
        using config_type  = cfg_struct_t<TypeT>;
    };

    //----------------------------------
    // cfg_string pass through
    template <>
    struct ConfigTypeT<cfg_string> {
        using value_type   = pfc::string8;
        using storage_type = typename value_type;
        using config_type  = cfg_string;
    };

    //----------------------------------
    // enum
    template <class TypeT>
    struct ConfigTypeT<TypeT, std::enable_if_t<std::is_enum_v<TypeT>>> {
        using value_type   = TypeT;
        using storage_type = std::underlying_type_t<TypeT>;
        using config_type  = cfg_int_t<storage_type>;
    };

    //----------------------------------
    // class wrapped enum
    template <class TypeT>
    struct ConfigTypeT<
        TypeT,
        std::void_t<
            decltype(TypeT::to_int(std::declval<typename TypeT::enum_type>()))
        >
    > : public ConfigTypeT <typename TypeT::enum_type> {};

    //----------------------------------
    // std::basic_string
    template <class CharT, class TraitsT, class AllocT>
    struct ConfigTypeT<std::basic_string<CharT, TraitsT, AllocT>> {
        using value_type = std::basic_string<CharT, TraitsT, AllocT>;
        using storage_type = typename value_type;
        using config_type = cfg_string;
    };

    //**************************************************************************
    // ConfigSerialiseT
    //**************************************************************************

    //----------------------------------
    // Default
    template <
        typename ConfigT,
        typename TypeT,
        class EnableT = void
    >
    struct ConfigSerialiseT {
        using value_type   = TypeT;
        using config_type  = ConfigT;
        using default_type = value_type;

        static void cfg_save(config_type& dst,
                             const value_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_load(value_type& dst,
                             const config_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_default(value_type& dst,
                                const default_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
    }; // template <...> struct ConfigSerialiseT

    //----------------------------------
    // cfg_int_t
    template <typename TypeT>
    struct ConfigSerialiseT<cfg_int_t<TypeT>, TypeT> {
        using value_type   = TypeT;
        using config_type  = cfg_int_t<value_type>;
        using default_type = value_type;

        static void cfg_save(config_type& dst,
                             const value_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_load(value_type& dst,
                             const config_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_default(value_type& dst,
                                const default_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
    }; // template <...> struct ConfigSerialiseT

    //----------------------------------
    // cfg_struct_t
    template <typename TypeT>
    struct ConfigSerialiseT<cfg_struct_t<TypeT>, TypeT> {
        using value_type   = TypeT;
        using config_type  = cfg_struct_t<value_type>;
        using default_type = value_type;

        static void cfg_save(config_type& dst,
                             const value_type& src)
            noexcept_if(dst.set(src)) {
            dst.set(src);
        }
        static void cfg_load(value_type& dst,
                             const config_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_default(value_type& dst,
                                const default_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
    }; // template <...> struct ConfigSerialiseT

    //----------------------------------
    // class wrapped enum
    template <typename TypeT>
    struct ConfigSerialiseT<
        cfg_int_t<std::underlying_type_t<typename TypeT::enum_type>>,
        TypeT,
        std::void_t<
            decltype(TypeT::to_int(std::declval<typename TypeT::enum_type>()))
        >
    > {
        using value_type      = TypeT;
        using enum_type       = typename value_type::enum_type;
        using underlying_type = std::underlying_type_t<enum_type>;
        using config_type     = cfg_int_t<underlying_type>;
        using default_type    = TypeT;

        static void cfg_save(config_type& dst,
                             const value_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
        static void cfg_load(value_type& dst,
                             const config_type& src)
            noexcept_if(dst = value_type::to_enum(src)) {
            dst = value_type::to_enum(src);
        }
        static void cfg_default(value_type& dst,
                                const default_type& src)
            noexcept_if(dst = src) {
            dst = src;
        }
    }; // template <...> struct ConfigSerialiseT

    //----------------------------------
    // std::wstring
    template <>
    struct ConfigSerialiseT<pfc::string8, std::wstring> {
        using value_type = std::wstring;
        using config_type = cfg_string;
        using default_type = pfc::string8;

        static void cfg_save(config_type& dst,
                             const value_type& src) noexcept(false) {
            using converter_type = pfc::stringcvt::string_utf8_from_wide;
            const converter_type converted{ src.c_str(), src.size() };
            dst.set_string(converted.get_ptr(), converted.length());
        }

        static void cfg_load(value_type& dst,
                             const config_type& src) noexcept(false) {
            using converter_type = pfc::stringcvt::string_wide_from_utf8;
            const converter_type converted{ src.c_str(), src.length() };
            dst.assign(converted.get_ptr(), converted.length());
        }

        static void cfg_default(value_type& dst,
                                const default_type& src) noexcept(false) {
            using converter_type = pfc::stringcvt::string_wide_from_utf8;
            const converter_type converted{ src.c_str(), src.length() };
            dst.assign(converted.get_ptr(), converted.length());
        }
    }; // template <...> struct ConfigSerialiseT

    template <>
    struct ConfigSerialiseT<cfg_string, std::wstring> :
        public ConfigSerialiseT<pfc::string8, std::wstring> {};

    //----------------------------------
    // std::string
    template <>
    struct ConfigSerialiseT<pfc::string8, std::string> {
        using value_type   = std::string;
        using config_type  = cfg_string;
        using default_type = pfc::string8;

        static void cfg_save(config_type& dst,
                             const value_type& src) noexcept(false) {
            dst.set_string(src.c_str(), src.size());
        }
        static void cfg_load(value_type& dst,
                             const config_type& src) noexcept(false) {
            dst.assign(src.c_str());
        }
        static void cfg_default(value_type& dst,
                                const default_type& src) noexcept(false) {
            dst.assign(src.c_str());
        }
    }; // template <...> struct ConfigSerialiseT

    template <>
    struct ConfigSerialiseT<cfg_string, std::string> :
        public ConfigSerialiseT<pfc::string8, std::string> {};

    //----------------------------------
    // class with cfg_save/cfg_load
    template <typename ConfigT, typename TypeT>
    struct ConfigSerialiseT<
        typename ConfigT,
        typename TypeT,
        std::void_t<
            decltype(std::declval<ConfigT>().cfg_save(std::declval<TypeT>()))
        >
    > final {
        using value_type   = TypeT;
        using config_type  = ConfigT;
        using default_type = value_type;

        static void cfg_save(config_type& dst,
                             const value_type& src)
            noexcept_if(dst.cfg_save(src)) {
            dst.cfg_save(src);
        }

        static void cfg_load(value_type& dst,
                             const config_type& src)
            noexcept_if(src.cfg_load(dst)) {
            src.cfg_load(dst);
        }

        static void cfg_default(value_type& dst,
                                const config_type& src)
            noexcept_if(src.cfg_default(dst)) {
            src.cfg_default(dst);
        }
    }; // template <...> struct ConfigSerialiseT
} // namespace foobar::Config::detail

//==============================================================================

namespace foobar::Config {
    //**************************************************************************
    // foobar cfg type helpers
    //**************************************************************************

    //----------------------------------
    // Dispatcher
    template <class TypeT>
    using cfg_type =
        typename foobar::Config::detail::ConfigTypeT<TypeT>;

    //----------------------------------
    // Dispatcher Helper
    template <class TypeT>
    using cfg_type_t =
        typename foobar::Config::detail::ConfigTypeT<TypeT>::config_type;

    //**************************************************************************
    // Serialisation Helpers
    //**************************************************************************

    //----------------------------------
    // Dispatcher (cfg_save)
    template <typename ConfigT, typename TypeT>
    void cfg_save(ConfigT& dst, const TypeT& src)
        noexcept_if(foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_save(dst, src)) {
        foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_save(dst, src);
    }

    //----------------------------------
    // Dispatcher (cfg_load)
    template <typename ConfigT, typename TypeT>
    void cfg_load(TypeT& dst, const ConfigT& src)
        noexcept_if(foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_load(dst, src)) {
        foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_load(dst, src);
    }

    //----------------------------------
    // Dispatcher (cfg_default)
    template <typename ConfigT, typename TypeT>
    void cfg_default(TypeT& dst, const ConfigT& src)
        noexcept_if(foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_default(dst, src)) {
        foobar::Config::detail::ConfigSerialiseT<ConfigT, TypeT>::cfg_default(dst, src);
    }
} // namespace foobar::Config

#endif // GUID_111922C8_DC8D_45D2_AB41_176A3B436B7E
