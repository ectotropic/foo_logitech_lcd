#pragma once
#ifndef GUID_9D40FD96_93D8_46F0_95EC_9AC45B0B077B
#define GUID_9D40FD96_93D8_46F0_95EC_9AC45B0B077B
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
#include "ColorCast.h"
#include "Util/TypeTraits.h"
//--------------------------------------

namespace Color {
    //**************************************************************************
    // ColorValueBlend
    //**************************************************************************
    template <
        typename LHSColorT,
        typename RHSColorT,
        typename BlendT
    >
    inline constexpr decltype(auto) ColorValueBlend(LHSColorT lhs,
                                                    RHSColorT rhs,
                                                    BlendT    blend) noexcept {
        using largest_t = ::util::largest_type_t<LHSColorT, RHSColorT>;
        using float_t   = ::util::floating_point_type_t<largest_t>;

        const auto lhsf  { ::Color::color_value_cast<float_t>(lhs) };
        const auto rhsf  { ::Color::color_value_cast<float_t>(rhs) };
        const auto blendf{ ::Color::color_value_cast<float_t>(blend) };
        return ::Color::color_value_cast<largest_t>(lhsf + (rhsf - lhsf) * blendf);
    }

    //**************************************************************************
    // ColorBlend
    //**************************************************************************
    template <
        typename LHSTypeT,
        typename RHSTypeT,
        std::size_t CountT,
        typename BlendT
    >
    inline constexpr auto ColorBlend(const ::Color::ColorT<LHSTypeT, CountT>& c0,
                                     const ::Color::ColorT<RHSTypeT, CountT>& c1,
                                     BlendT blend) noexcept {
        using blend_color_value_t = ::util::largest_type_t<LHSTypeT, RHSTypeT>;
        using blend_color_t       = ::Color::ColorT<blend_color_value_t, CountT>;
        blend_color_t result{};
        for (std::size_t i = 0; i < c0.size(); ++i) {
            result[i] = ::Color::ColorValueBlend(c0[i], c1[i], blend);
        }
        return result;
    }

    //**************************************************************************
    // ColorValueAlphaBlend
    //**************************************************************************
    template <
        typename TargetT,
        typename SourceT,
        typename AlphaT
    >
    inline constexpr decltype(auto) ColorValueAlphaBlend(TargetT target,
                                                         SourceT source,
                                                         AlphaT  alpha) noexcept {
        using largest_t = ::util::largest_type_t<TargetT, SourceT>;
        using float_t   = ::util::floating_point_type_t<largest_t>;

        constexpr const auto onef { static_cast<float_t>(1) };
        constexpr const auto zerof{ static_cast<float_t>(0) };

        const auto targetf { ::Color::color_value_cast<float_t>(target) };
        const auto sourcef { ::Color::color_value_cast<float_t>(source) };
        auto alphaf{ ::Color::color_value_cast<float_t>(alpha) };
        alphaf = std::max(std::min(alphaf, onef), zerof);
        return ::Color::color_value_cast<largest_t>(sourcef * alphaf + targetf * (onef - alphaf));
    }

    //**************************************************************************
    // ColorAlphaBlend
    //**************************************************************************
    template <
        typename TargetT,
        typename SourceT
    >
    inline constexpr auto ColorAlphaBlend(const ::Color::ColorT<TargetT, 4>& target,
                                          const ::Color::ColorT<SourceT, 4>& source) noexcept {
        using blend_color_value_t = ::util::largest_type_t<TargetT, SourceT>;
        using blend_color_t       = ::Color::ColorT<blend_color_value_t, 4>;
        return blend_color_t{
            ::Color::ColorValueAlphaBlend<blend_color_value_t>(target.r(), source.r(), source.a()),
            ::Color::ColorValueAlphaBlend<blend_color_value_t>(target.g(), source.g(), source.a()),
            ::Color::ColorValueAlphaBlend<blend_color_value_t>(target.b(), source.b(), source.a()),
            ::Color::ColorValueAlphaBlend<blend_color_value_t>(target.a(), source.a(), source.a()),
        };
    }

    //**************************************************************************
    // ColorAlphaBlend
    //**************************************************************************
    template <
        typename TargetT,
        typename SourceT,
        std::size_t CountT,
        typename AlphaT
    >
    inline constexpr auto ColorAlphaBlend(const ::Color::ColorT<TargetT, CountT>& target,
                                          const ::Color::ColorT<SourceT, CountT>& source,
                                          AlphaT alpha) noexcept {
        using blend_color_value_t = ::util::largest_type_t<TargetT, SourceT>;
        using blend_color_t       = ::Color::ColorT<blend_color_value_t, CountT>;
        blend_color_t result{};
        for (std::size_t i = 0; i < target.size(); ++i) {
            result[i] = ::Color::ColorValueAlphaBlend<blend_color_value_t>(target[i], source[i], alpha);
        }
        return result;
    }
} // namespace Color

#endif // GUID_9D40FD96_93D8_46F0_95EC_9AC45B0B077B