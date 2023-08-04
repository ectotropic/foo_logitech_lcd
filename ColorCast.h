#pragma once
#ifndef GUID_702B51A6_0C16_4E48_A70B_733AD62D366F
#define GUID_702B51A6_0C16_4E48_A70B_733AD62D366F
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
#include "ColorTraits.h"
#include <limits>
#include "Util/TypeTraits.h"
//--------------------------------------

namespace Color::detail {
    //**************************************************************************
    // ColorValueFloatCastT
    //**************************************************************************
    template <
        typename ValueT,        // Color value type
        typename FloatT,        // Intermediate type
        typename EnableT = void // SFINAE
    >
    struct ColorValueFloatCastT final {
        static_assert(std::is_arithmetic_v<ValueT>, "ColorValueFloatCastT: Type must be an arithmetic type!");
    }; // template<...> struct ColorValueFloatCastT final

    //--------------------------------------------------------------------------
    // Specialisation: is_floating_point_v<ValueT>
    //--------------------------------------------------------------------------
    template <typename ValueT, typename FloatT>
    struct ColorValueFloatCastT<
        ValueT,
        FloatT,
        std::enable_if_t<std::is_floating_point_v<ValueT>>
    > final {
    public:
        using value_type = ValueT;
        using float_type = FloatT;

        static constexpr float_type to_itermediate(value_type val) noexcept {
            return static_cast<float_type>(val);
        }

        static constexpr value_type from_itermediate(float_type val) noexcept {
            return static_cast<value_type>(val);
        }
    }; // template<...> struct ColorValueFloatCastT final [[Specialisation]]

    //--------------------------------------------------------------------------
    // Specialisation: is_integral<ValueT> && is_signed<ValueT>
    //--------------------------------------------------------------------------
    template <typename ValueT, typename FloatT>
    struct ColorValueFloatCastT<
        ValueT,
        FloatT,
        std::enable_if_t<
            std::conjunction_v<
                std::is_integral<ValueT>,
                std::is_signed<ValueT>
            >
        >
    > final {
    private:
        inline static constexpr const auto max{ std::numeric_limits<ValueT>::max() };
        inline static constexpr const auto min{ std::numeric_limits<ValueT>::min() };

        inline static constexpr const auto maxf{ static_cast<FloatT>(+1) };
        inline static constexpr const auto minf{ static_cast<FloatT>(-1) };

    public:
        using value_type = ValueT;
        using float_type = FloatT;

        static constexpr float_type to_itermediate(value_type val) noexcept {
            if (val >= 0) {
                return static_cast<float_type>(val) / static_cast<float_type>(max);
            } else {
                return static_cast<float_type>(val) / static_cast<float_type>(min);
            }
        }

        static constexpr value_type from_itermediate(float_type val) noexcept {
            if (val >= maxf) {
                return max;
            } else if (val <= minf) {
                return min;
            } else if (val >= 0) {
                return static_cast<value_type>(val * static_cast<float_type>(max));
            } else {
                return static_cast<value_type>(val * -static_cast<float_type>(min));
            }
        }
    }; // template<...> struct ColorValueFloatCastT final [[Specialisation]]

    //--------------------------------------------------------------------------
    // Specialisation: is_integral<ValueT> && is_unsigned<ValueT>
    //--------------------------------------------------------------------------
    template <typename ValueT, typename FloatT>
    struct ColorValueFloatCastT<
        ValueT,
        FloatT,
        std::enable_if_t<
            std::conjunction_v<
                std::is_integral<ValueT>,
                std::is_unsigned<ValueT>
            >
        >
    > final {
    private:
        inline static constexpr const auto max{ std::numeric_limits<ValueT>::max() };
        inline static constexpr const auto min{ std::numeric_limits<ValueT>::min() };

        inline static constexpr const auto maxf{ static_cast<FloatT>(1) };
        inline static constexpr const auto minf{ static_cast<FloatT>(0) };

    public:
        using value_type = ValueT;
        using float_type = FloatT;

        static constexpr float_type to_itermediate(value_type val) noexcept {
            return static_cast<float_type>(val) / static_cast<float_type>(max);
        }

        static constexpr value_type from_itermediate(float_type val) noexcept {
            if (val >= maxf) {
                return max;
            } else if (val <= minf) {
                return min;
            } else {
                gsl_suppress(26467) // C26467: Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value.
                return static_cast<value_type>(val * static_cast<float_type>(max));
            }
        }
    }; // template<...> struct ColorValueFloatCastT final [[Specialisation]]

    //**************************************************************************
    // ColorCastImplT
    //**************************************************************************
    template <typename TargetT>
    struct ColorCastImplT {
        template <typename SourceT, std::size_t CountT>
        static constexpr auto color_cast(const ::Color::ColorT<SourceT, CountT>& source) noexcept {
            using source_type       = ::Color::ColorT<SourceT, CountT>;
            using target_type       = ::Color::ColorT<TargetT, CountT>;
            using target_value_type = typename target_type::value_type;
            using size_type         = typename target_type::size_type;

            target_type target{};
            for (size_type i = 0; i < CountT; ++i) {
                target[i] = ::Color::color_value_cast<target_value_type>(source[i]);
            }
            return target;
        }
    };

    //----------------------------------

    template <typename TargetT, std::size_t TargetCountT>
    struct ColorCastImplT<::Color::ColorT<TargetT, TargetCountT>> {
        template <typename SourceT, std::size_t SourceCountT>
        static constexpr auto color_cast(const ::Color::ColorT<SourceT, SourceCountT>& source) noexcept {
            using source_type       = ::Color::ColorT<SourceT, SourceCountT>;
            using target_type       = ::Color::ColorT<TargetT, TargetCountT>;
            using target_value_type = typename target_type::value_type;
            using size_type         = typename target_type::size_type;

            target_type target{};
            const auto count{ std::min(source.size(), target.size())};
            size_type i = 0;
            while (i < count) {
                target[i] = ::Color::color_value_cast<target_value_type>(source[i]);
                ++i;
            }
            while (i < target.size()) { target[i] = 0; ++i; }
            return target;
        }
    };
} // namespace Color::detail

//==============================================================================

namespace Color {
    //**************************************************************************
    // color_value_cast
    //**************************************************************************
    template <typename TargetT, typename SourceT>
    inline constexpr auto color_value_cast(SourceT val) noexcept {
        using largest_t = ::util::largest_type_t<TargetT, SourceT>;
        using float_t   = ::util::floating_point_type_t<largest_t>;

        using target_helper = ::Color::detail::ColorValueFloatCastT<TargetT, float_t>;
        using source_helper = ::Color::detail::ColorValueFloatCastT<SourceT, float_t>;

        const auto inter_val{ source_helper::to_itermediate(val) };
        return target_helper::from_itermediate(inter_val);
    }

    //**************************************************************************
    // color_cast
    //**************************************************************************
    template <
        typename TargetT,
        typename SourceT,
        std::size_t CountT
    >
    inline constexpr decltype(auto) color_cast(const ::Color::ColorT<SourceT, CountT>& source) noexcept {
        return ::Color::detail::ColorCastImplT<TargetT>::color_cast(source);
    }
} // namespace Color

#endif // GUID_702B51A6_0C16_4E48_A70B_733AD62D366F