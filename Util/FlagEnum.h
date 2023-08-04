#pragma once
#ifndef GUID_CD3D1833_1E86_419A_A183_5FFF7C2C90E2
#define GUID_CD3D1833_1E86_419A_A183_5FFF7C2C90E2
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
#include <type_traits>
#include <cassert>
//--------------------------------------

namespace util {
    //**************************************************************************
    // flag_enum
    //**************************************************************************
    template <typename EnumT>
    class flag_enum : public EnumT {
    private:
        using base_type = EnumT;
        using this_type = flag_enum<EnumT>;

    private:
        template <typename OtherT>
        using enable_if_can_construct_with =
            std::enable_if<std::is_constructible_v<this_type, OtherT>
                           || std::is_integral_v<OtherT>, bool>;

        template <typename OtherT>
        using enable_if_can_construct_with_t =
            typename enable_if_can_construct_with<OtherT>::type;

    public:
        using typename EnumT::enum_type;
        using typename EnumT::int_type;

        static constexpr int_type to_int(enum_type e) noexcept {
            return static_cast<int_type>(e);
        }

        static constexpr enum_type to_enum(int_type i) noexcept {
            return static_cast<enum_type>(i);
        }

    private:
        template <typename IntT, std::enable_if_t<std::is_integral_v<IntT>, bool> = true>
        explicit constexpr flag_enum(IntT val) noexcept :
            m_Val(static_cast<int_type>(val)) {
            assert(val == static_cast<IntT>(m_Val));
        }

    public:
        constexpr flag_enum() noexcept = default;

        constexpr flag_enum(const this_type& ) noexcept = default;
        constexpr flag_enum(      this_type&&) noexcept = default;

        constexpr flag_enum(enum_type val) noexcept :
            m_Val{ to_int(val) } {};

    public:
        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr this_type& operator=(OtherT other) noexcept {
            m_Val = this_type(other).m_Val;
            return *this;
        }

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr bool operator==(OtherT other) const noexcept {
            return m_Val == this_type(other).m_Val;
        }

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr bool operator!=(OtherT other) const noexcept {
            return m_Val != this_type(other).m_Val;
        }

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr this_type& operator|=(OtherT other) {
            m_Val |= this_type(other).m_Val;
            return *this;
        }

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr this_type& operator&=(OtherT other) {
            m_Val &= this_type(other).m_Val;
            return *this;
        }

        template <typename OtherT = this_type, enable_if_can_construct_with_t<OtherT> = true>
        constexpr this_type& operator^=(OtherT other) {
            m_Val ^= this_type(other).m_Val;
            return *this;
        }

        constexpr this_type operator~() const noexcept {
            return this_type(~m_Val);
        }

    public:
        constexpr auto to_enum() const noexcept {
            return this_type::to_enum(m_Val);
        }

        constexpr auto  to_int() const noexcept {
            return m_Val;
        }

        constexpr operator enum_type() const noexcept {
            return to_enum();
        }

        explicit constexpr operator int_type() const noexcept {
            return to_int();
        }

        explicit constexpr operator bool() const noexcept {
            return m_Val;
        }

    private:
        int_type m_Val{ 0 };
    }; // template <...> class flag_enum
} // namespace util

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//  Operators
#define FLAG_ENUM_OPERATORS(name) \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator|(OtherT lhs, name rhs) { return name{ lhs } |= rhs; } \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator&(OtherT lhs, name rhs) { return name{ lhs } &= rhs; } \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator^(OtherT lhs, name rhs) { return name{ lhs } ^= rhs; } \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator|(OtherT lhs, typename name::enum_type rhs) { return name{ lhs } |= rhs; } \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator&(OtherT lhs, typename name::enum_type rhs) { return name{ lhs } &= rhs; } \
    template <typename OtherT = name, std::enable_if_t<std::is_constructible_v<OtherT, name>, bool> = true> \
    constexpr name operator^(OtherT lhs, typename name::enum_type rhs) { return name{ lhs } ^= rhs; } \
    constexpr name operator~(name::enum_type val) { return ~name{ val }; }
//--------------------------------------

//--------------------------------------
//  Define Values
#define FLAG_ENUM_VALUES(v, ...)  v, __VA_ARGS__
//--------------------------------------

//--------------------------------------
//  Define Enumeration (no operators)
#define FLAG_ENUM_NO_OPS(name, ...) \
    struct name ## _impl_ { \
        enum enum_type { __VA_ARGS__, }; \
        using int_type = std::underlying_type_t<enum_type>; \
    }; \
    using name = ::util::flag_enum<name ## _impl_>;
//--------------------------------------

//--------------------------------------
//  Define Enumeration (with operators)
#define FLAG_ENUM(name, ...) \
    FLAG_ENUM_NO_OPS(name, __VA_ARGS__) \
    FLAG_ENUM_OPERATORS(name)
//--------------------------------------

#endif // GUID_CD3D1833_1E86_419A_A183_5FFF7C2C90E2
