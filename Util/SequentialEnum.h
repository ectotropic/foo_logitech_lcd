#pragma once
#ifndef GUID_E544FEBD_E7A5_4A13_AAC0_1A8F52E0DB24
#define GUID_E544FEBD_E7A5_4A13_AAC0_1A8F52E0DB24
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
#include "Util/TypeTraits.h"
#include <cassert>
//--------------------------------------

namespace util {
    //**************************************************************************
    // sequential_enum: iterable enum which also supports named values
    //**************************************************************************
    template <typename EnumT>
    class sequential_enum : public EnumT {
    private:
        using base_type = EnumT;
        using this_type = sequential_enum<EnumT>;

    private:
        template <typename OtherT>
        using enable_if_can_construct_with =
            std::enable_if<std::is_constructible_v<this_type, OtherT>
                           || std::is_integral_v<OtherT>, bool>;

        template <typename OtherT>
        using enable_if_can_construct_with_t =
            typename enable_if_can_construct_with<OtherT>::type;

        template <typename LHSTypeT, typename RHSTypeT>
        using enable_if_same =
            std::enable_if<std::is_same_v<LHSTypeT, RHSTypeT>, bool>;

        template <typename LHSTypeT, typename RHSTypeT>
        using enable_if_same_t =
            typename enable_if_same<LHSTypeT, RHSTypeT>::type;

        template <typename TypeT>
        using enable_if_integral =
            std::enable_if<std::is_integral_v<TypeT>, bool>;

        template <typename TypeT>
        using enable_if_integral_t =
            typename enable_if_integral<TypeT>::type;

    public:
        using typename base_type::enum_type;
        using typename base_type::int_type;

        static constexpr int_type  to_int (enum_type e) noexcept { return  static_cast<int_type>(e); }
        static constexpr enum_type to_enum(int_type  i) noexcept { return static_cast<enum_type>(i); }

        static constexpr auto begin () noexcept { return this_type(base_type::_sequential_enum_begin); }
        static constexpr auto cbegin() noexcept { return this_type(base_type::_sequential_enum_begin); }
        static constexpr auto end   () noexcept { return this_type(base_type::_sequential_enum_end); }
        static constexpr auto cend  () noexcept { return this_type(base_type::_sequential_enum_end); }

        static constexpr auto count () { return end().to_int() - begin().to_int(); }

        static constexpr auto first () noexcept { return begin(); }
        static constexpr auto last  () noexcept { return --end(); }

        template <typename IntT>
        static constexpr auto valid(IntT i) noexcept {
            const this_type val{ static_cast<int_type>(i) };
            return i == static_cast<IntT>(val.m_Val) &&
                   (val >= first()) && (val <= last());
        }

    private:
        template <typename IntT, std::enable_if_t<std::is_integral_v<IntT>, bool> = true>
        explicit constexpr sequential_enum(IntT val) noexcept :
            m_Val(static_cast<int_type>(val)) {}

    public:
        constexpr sequential_enum() noexcept = default;

        constexpr sequential_enum(const this_type& ) noexcept = default;
        constexpr sequential_enum(      this_type&&) noexcept = default;

        constexpr sequential_enum(enum_type val) noexcept :
            m_Val(to_int(val)) {}

    public:
        constexpr this_type& operator=(const this_type& ) noexcept = default;
        constexpr this_type& operator=(      this_type&&) noexcept = default;

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr this_type& operator=(OtherT other) noexcept {
            m_Val = this_type(std::forward<OtherT>(other)).m_Val;
            return *this;
        }

    public:
        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator==(OtherT other) const noexcept {
            return m_Val == this_type(other).m_Val;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator!=(OtherT other) const noexcept {
            return m_Val != this_type(other).m_Val;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator<(OtherT other) const noexcept {
            return m_Val < this_type(other).m_Val;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator<=(OtherT other) const noexcept {
            return m_Val <= this_type(other).m_Val;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator>(OtherT other) const noexcept {
            return m_Val > this_type(other).m_Val;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr bool operator>=(OtherT other) const noexcept {
            return m_Val >= this_type(other).m_Val;
        }

        constexpr sequential_enum& operator++() noexcept {
            ++m_Val;
            return *this;
        }
        constexpr sequential_enum& operator--() noexcept {
            --m_Val;
            return *this;
        }

        constexpr sequential_enum& operator++(int) noexcept {
            auto oldVal = m_Val;
            ++m_Val;
            return oldVal;
        }

        constexpr sequential_enum& operator--(int) noexcept {
            auto oldVal = m_Val;
            --m_Val;
            return oldVal;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr sequential_enum& operator-=(OtherT val) noexcept {
            m_Val -= this_type(val).m_Val;
            return *this;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr sequential_enum& operator+=(OtherT val) noexcept {
            m_Val += this_type(val).m_Val;
            return *this;
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr sequential_enum operator-(OtherT val) const noexcept {
            return this_type(m_Val) -= this_type(val);
        }

        template <
            typename OtherT = this_type,
            enable_if_can_construct_with_t<OtherT> = true
        >
        constexpr sequential_enum operator+(OtherT val) const noexcept {
            return this_type(m_Val) += this_type(val);
        }

    public:
        // Iterator support (type is it's own iterator)
        constexpr const sequential_enum& operator*() const noexcept { return *this; }

    public:
        constexpr auto to_enum() const noexcept { return to_enum(m_Val); }
        constexpr auto to_int () const noexcept { return m_Val; }

    public:
        //--------------------------------------------------
        // Having these methods templated means the compiler
        // will ignore them unless they are called, so
        // sequential enums that do not provide string names
        // will not cause compilation errors.
        template <typename DummyT = this_type, enable_if_same_t<DummyT, this_type> = true>
        auto to_string() const noexcept {
            return base_type::_to_string(to_int());
        }

        template <typename DummyT = this_type, enable_if_same_t<DummyT, this_type> = true>
        static auto to_string(DummyT e) noexcept {
            return e.to_string();
        }

        template <typename DummyT = int_type, enable_if_same_t<DummyT, int_type> = true>
        static auto to_string(DummyT i) noexcept {
            return base_type::_to_string(i);
        }

        template <typename DummyT = enum_type, enable_if_same_t<DummyT, enum_type> = true>
        static auto to_string(DummyT e) noexcept {
            return to_string(to_int(e));
        }
        //--------------------------------------------------

    public:
        constexpr operator enum_type() const noexcept { return to_enum(); }

        explicit constexpr operator int_type() const noexcept { return to_int(); }

    private:
        int_type m_Val{ _sequential_enum_begin };
    };
} // namespace util

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#define DETAIL_SEQUENTIAL_ENUM_FIRST(e, v, ...) \
            _sequential_enum_begin = v, e = _sequential_enum_begin

#define SEQUENTIAL_ENUM_FIRST(e, ...) \
            DETAIL_SEQUENTIAL_ENUM_FIRST(e, __VA_ARGS__, 0)

#define SEQUENTIAL_ENUM_VALUES(v, ...)  v, __VA_ARGS__

#define SEQUENTIAL_ENUM(name, e, ...) \
    class name ## _impl_ { \
    public: \
        enum enum_type { \
            e, \
            _sequential_enum_end, \
        }; \
        using int_type = std::underlying_type_t<enum_type>; \
    }; \
    using name = ::util::sequential_enum<name ## _impl_>;

//--------------------------------------
//
#define DETAIL_SEQUENTIAL_NAMED_ENUM_GET_FIRST(f, ...) f
#define DETAIL_DEFINE_SEQUENTIAL_NAMED_ENUM(name, e, s, s2, ...) \
    class name ## _impl_ { \
    public: \
        enum enum_type { \
            e, \
            _sequential_enum_end, \
        }; \
        using int_type  = std::underlying_type_t<enum_type>; \
        using char_type = ::util::char_type_t<decltype(DETAIL_SEQUENTIAL_NAMED_ENUM_GET_FIRST ## s)>; \
    protected: \
        static auto _to_string(int_type i) noexcept { \
            constexpr const auto _ibegin = static_cast<int_type>(_sequential_enum_begin); \
            constexpr const auto _iend   = static_cast<int_type>(_sequential_enum_end); \
            constexpr const auto _count  = _iend - _ibegin; \
            static const char_type* s_szStrings[_count]{ s2, ## __VA_ARGS__ }; \
            assert(i >= _ibegin && i < _iend); \
            return s_szStrings[i]; \
        } \
    }; \
    using name = ::util::sequential_enum<name ## _impl_>; \

#define SEQUENTIAL_NAMED_ENUM_VALUES  \
            SEQUENTIAL_ENUM_VALUES
#define SEQUENTIAL_NAMED_ENUM_STRINGS(s, ...) \
            s, ## __VA_ARGS__
#define SEQUENTIAL_NAMED_ENUM(name, e, s) \
            DETAIL_DEFINE_SEQUENTIAL_NAMED_ENUM(name, e, (s), s)

#endif // GUID_E544FEBD_E7A5_4A13_AAC0_1A8F52E0DB24
