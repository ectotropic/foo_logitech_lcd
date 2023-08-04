#pragma once
#ifndef GUID_B92A61D0_2A8D_49E4_BD2F_696616426551
#define GUID_B92A61D0_2A8D_49E4_BD2F_696616426551
/*******************************************************************************
 *******************************************************************************
 * Copyright (c) 2009-2023 ectotropic (ectotropic@gmail.com,                   *
 *                                     https://github.com/ectotropic)          *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation, either version 2.1 of the License, or (at     *
 * option) any later version.                                                  *
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
//--------------------------------------

namespace util {
    //__________________________________________________________________________
    // remove_cvref:
    //      Added in C++20, but easy to define in earlier versions
#if defined(__cpp_lib_remove_cvref) && (__cpp_lib_remove_cvref >= 201711L)
    using std::remove_cvref;
    using std::remove_cvref_t;
#else
    template <typename TypeT>
    using remove_cvref =
        std::remove_cv<std::remove_reference_t<TypeT>>;

    //----------------------------------

    template <typename TypeT>
    using remove_cvref_t =
        typename remove_cvref<TypeT>::type;
#endif

    //__________________________________________________________________________
    // char_type:
    //  Character type of a static string.

    template <typename TypeT>
    struct char_type {
        using type = std::remove_all_extents_t<std::remove_reference_t<TypeT>>;
    };

    //----------------------------------

    template <typename TypeT>
    using char_type_t = typename ::util::char_type<TypeT>::type;

    //__________________________________________________________________________
    // comparison:
    //  Compare two values with the given operator
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_gt = std::integral_constant<bool, (LHS > RHS)>;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_ge = std::integral_constant<bool, (LHS >= RHS)>;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_lt = std::integral_constant<bool, (LHS < RHS)>;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_le = std::integral_constant<bool, (LHS <= RHS)>;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_eq = std::integral_constant<bool, (LHS == RHS)>;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    using is_ne = std::integral_constant<bool, (LHS != RHS)>;

    //----------------------------------

    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_gt_v = ::util::is_gt<TypeT, LHS, RHS>::value;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_ge_v = ::util::is_ge<TypeT, LHS, RHS>::value;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_lt_v = ::util::is_lt<TypeT, LHS, RHS>::value;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_le_v = ::util::is_le<TypeT, LHS, RHS>::value;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_eq_v = ::util::is_eq<TypeT, LHS, RHS>::value;
    template <typename TypeT, TypeT LHS, TypeT RHS>
    constexpr bool is_ne_v = ::util::is_ne<TypeT, LHS, RHS>::value;

    //__________________________________________________________________________
    // sizeof comparison:
    //  Compare the size of the given types using the specified operator
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_gt = ::util::is_gt<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_ge = ::util::is_ge<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_lt = ::util::is_lt<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_le = ::util::is_le<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_eq = ::util::is_eq<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;
    template <typename LHSTypeT, typename RHSTypeT>
    using sizeof_ne = ::util::is_ne<std::size_t, sizeof(LHSTypeT), sizeof(RHSTypeT)>;

    //----------------------------------

    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_gt_v = ::util::sizeof_gt<LHSTypeT, RHSTypeT>::value;
    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_ge_v = ::util::sizeof_ge<LHSTypeT, RHSTypeT>::value;
    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_lt_v = ::util::sizeof_lt<LHSTypeT, RHSTypeT>::value;
    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_le_v = ::util::sizeof_le<LHSTypeT, RHSTypeT>::value;
    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_eq_v = ::util::sizeof_eq<LHSTypeT, RHSTypeT>::value;
    template <typename LHSTypeT, typename RHSTypeT>
    constexpr bool sizeof_ne_v = ::util::sizeof_ne<LHSTypeT, RHSTypeT>::value;

    //__________________________________________________________________________
    // largest_type:
    //  Get the largest of two types
    template <typename LHSTypeT, typename RHSTypeT, typename EnableT = void>
    struct largest_type { using type = RHSTypeT; };

    //----------------------------------

    template <typename LHSTypeT, typename RHSTypeT>
    struct largest_type<
        LHSTypeT, RHSTypeT,
        std::enable_if_t<::util::sizeof_gt_v<LHSTypeT, RHSTypeT>>
    > { using type = LHSTypeT; };

    //----------------------------------
    //----------------------------------

    template <typename LHSTypeT, typename RHSTypeT>
    using largest_type_t =
        typename ::util::largest_type<LHSTypeT, RHSTypeT>::type;

    //__________________________________________________________________________
    // floating_point_type:
    //  Get a floating point type of equal or larger size to another type
    template <typename TypeT, typename EnableT = void>
    struct floating_point_type {
        static_assert(std::is_arithmetic_v<TypeT>,
                      "floating_point_type: Type must be an arithmetic type!");
    };

    //----------------------------------

    template <typename TypeT>
    struct floating_point_type<
        TypeT,
        std::enable_if_t<std::is_floating_point_v<TypeT>>
    > { using type = TypeT; };

    //----------------------------------

    // sizeof(double) < sizeof(TypeT)
    template <typename TypeT>
    struct floating_point_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                std::is_integral<TypeT>,
                ::util::sizeof_gt<TypeT, double>
            >
        >
    > { using type = long double; };

    //----------------------------------

    // sizeof(float) < sizeof(TypeT) < sizeof(long double)
    template <typename TypeT>
    struct floating_point_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                std::is_integral<TypeT>,
                ::util::sizeof_gt<TypeT, float>,
                ::util::sizeof_lt<TypeT, long double>
            >
        >
    > { using type = double; };

    //----------------------------------

    // sizeof(TypeT) <= sizeof(float)
    template <typename TypeT>
    struct floating_point_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                std::is_integral<TypeT>,
                ::util::sizeof_le<TypeT, float>
            >
        >
    > { using type = float; };

    //----------------------------------
    //----------------------------------

    template <typename TypeT>
    using floating_point_type_t =
        typename ::util::floating_point_type<TypeT>::type;

    //__________________________________________________________________________
    // integral_type:
    //  Get a floating point type of equal or larger size to another type
    template <typename TypeT, typename EnableT = void>
    struct integral_type {
        //static_assert(...);
    };

    //----------------------------------

    template <typename TypeT>
    struct integral_type<
        TypeT,
        std::enable_if_t<::util::sizeof_le_v<TypeT, std::int8_t>>
    > {
        using signed_type   = std::int8_t;
        using unsigned_type = std::uint8_t;
    };

    //----------------------------------

    template <typename TypeT>
    struct integral_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::sizeof_gt<TypeT, std::int8_t>,
                ::util::sizeof_le<TypeT, std::int16_t>
            >
        >
    > {
        using signed_type   = std::int16_t;
        using unsigned_type = std::uint16_t;
    };

    //----------------------------------

    template <typename TypeT>
    struct integral_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::sizeof_gt<TypeT, std::int16_t>,
                ::util::sizeof_le<TypeT, std::int32_t>
            >
        >
    > {
        using signed_type   = std::int32_t;
        using unsigned_type = std::uint32_t;
    };

    //----------------------------------

    template <typename TypeT>
    struct integral_type<
        TypeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::sizeof_gt<TypeT, std::int32_t>,
                ::util::sizeof_le<TypeT, std::int64_t>
            >
        >
    > {
        using signed_type   = std::int64_t;
        using unsigned_type = std::uint64_t;
    };

    //----------------------------------
    //----------------------------------

    template <typename TypeT>
    using signed_integral_type_t =
        typename ::util::integral_type<TypeT>::signed_type;

    //----------------------------------

    template <typename TypeT>
    using unsigned_integral_type_t =
        typename ::util::integral_type<TypeT>::unsigned_type;

    //__________________________________________________________________________
    // integral_type:
    //  Get a floating point type of equal or larger size to another type
    template <std::size_t SizeT, typename EnableT = void>
    struct integral_type_from_size {
        //static_assert(...);
    };

    //----------------------------------

    template <std::size_t SizeT>
    struct integral_type_from_size<
        SizeT,
        std::enable_if_t<
            ::util::is_le_v<std::size_t, SizeT, sizeof(std::int8_t)>
        >
    > {
        using signed_type   = std::int8_t;
        using unsigned_type = std::uint8_t;
    };

    //----------------------------------

    template <std::size_t SizeT>
    struct integral_type_from_size <
        SizeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::is_gt<std::size_t, SizeT, sizeof(std::int8_t)>,
                ::util::is_le<std::size_t, SizeT, sizeof(std::int16_t)>
            >
        >
    > {
        using signed_type   = std::int16_t;
        using unsigned_type = std::uint16_t;
    };

    //----------------------------------

    template <std::size_t SizeT>
    struct integral_type_from_size <
        SizeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::is_gt<std::size_t, SizeT, sizeof(std::int16_t)>,
                ::util::is_le<std::size_t, SizeT, sizeof(std::int32_t)>
            >
        >
    > {
        using signed_type   = std::int32_t;
        using unsigned_type = std::uint32_t;
    };

    //----------------------------------

    template <std::size_t SizeT>
    struct integral_type_from_size<
        SizeT,
        std::enable_if_t<
            std::conjunction_v<
                ::util::is_gt<std::size_t, SizeT, sizeof(std::int32_t)>,
                ::util::is_le<std::size_t, SizeT, sizeof(std::int64_t)>
            >
        >
    > {
        using signed_type   = std::int64_t;
        using unsigned_type = std::uint64_t;
    };

    //----------------------------------
    //----------------------------------

    template <std::size_t SizeT>
    using signed_integral_type_from_size_t =
        typename ::util::integral_type_from_size<SizeT>::signed_type;

    //----------------------------------

    template <std::size_t SizeT>
    using unsigned_integral_type_from_size_t =
        typename ::util::integral_type_from_size<SizeT>::unsigned_type;
} // namespace util

#endif // GUID_B92A61D0_2A8D_49E4_BD2F_696616426551
