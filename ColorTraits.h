#pragma once
#ifndef GUID_2606B8F4_8C28_4FE9_8B08_AD578BF4F311
#define GUID_2606B8F4_8C28_4FE9_8B08_AD578BF4F311
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
#include "Color.h"
#include "Util/TypeTraits.h"
//--------------------------------------

namespace util {
    //**************************************************************************
    // color_value_type
    //**************************************************************************
    template <typename TypeT>
    struct color_value_type {
        using type = ::util::remove_cvref_t<TypeT>;
    };

    //----------------------------------

    template <typename TypeT, std::size_t CountT>
    struct color_value_type<::Color::ColorT<TypeT, CountT>> {
        using type = ::util::remove_cvref_t<TypeT>;
    };

    //----------------------------------

    template <typename TypeT>
    using color_value_type_t = typename ::util::color_value_type<TypeT>::type;

    //**************************************************************************
    // color_type
    //**************************************************************************
    template <typename TypeT, std::size_t CountT>
    struct color_type {
        using type = ::Color::ColorT<TypeT, CountT>;
    };

    //----------------------------------

    template <typename TypeT, std::size_t Count1T, std::size_t Count2T>
    struct color_type<::Color::ColorT<TypeT, Count1T>, Count2T> {
        static_assert(Count1T == Count2T, "color_type trait counts do not match");
        using type = ::Color::ColorT<TypeT, Count1T>;
    };

    //----------------------------------

    template <typename TypeT, std::size_t CountT>
    using color_type_t =
        typename ::util::color_type<::util::remove_cvref_t<TypeT>, CountT>::type;

    //**************************************************************************
    // is_color_type
    //**************************************************************************
    template <typename TypeT>
    struct is_color_type : public std::false_type {};

    //----------------------------------

    template <typename TypeT, std::size_t CountT>
    struct is_color_type<::Color::ColorT<TypeT, CountT>> : public std::true_type {};

    //----------------------------------

    template <typename TypeT>
    constexpr bool is_color_type_v =
        ::util::is_color_type<::util::remove_cvref_t<TypeT>>::value;

    //**************************************************************************
    // color_component_count
    //**************************************************************************
    template <typename TypeT>
    struct color_component_count {
        static constexpr std::size_t value{0};
    };

    //----------------------------------

    template <typename TypeT, std::size_t CountT>
    struct color_component_count<::Color::ColorT<TypeT, CountT>> {
        static constexpr std::size_t value{ CountT };
    };

    //----------------------------------

    template <typename TypeT>
    constexpr auto color_component_count_v =
        ::util::color_component_count<::util::remove_cvref_t<TypeT>>::value;
} // namespace util

#endif // GUID_2606B8F4_8C28_4FE9_8B08_AD578BF4F311