#pragma once
#ifndef GUID_B9A272CC_A6CB_43C5_9096_25CCA86D59C0
#define GUID_B9A272CC_A6CB_43C5_9096_25CCA86D59C0
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
#include "Util/TypeTraits.h"
//--------------------------------------

//--------------------------------------
//
#include <cstdint>
#include <array>
#include <limits>
//--------------------------------------

namespace Color::detail {
    //**************************************************************************
    // ColorCommonT
    //**************************************************************************
    template <typename ComponentT, std::size_t CountT>
    class ColorCommonT : public std::array<ComponentT, CountT> {
    private:
        static_assert((CountT > 0) && (CountT <= 4),
                      "ColorCommonT unsupported component count");

    private:
        using this_type = ColorCommonT<ComponentT, CountT>;
        using base_type = std::array<ComponentT, CountT>;

    public:
        using component_type = base_type::value_type;

    public:
        inline static constexpr const auto Count     { CountT };
        inline static constexpr const auto ChannelMax{ std::numeric_limits<component_type>::max() };
        inline static constexpr const auto ChannelMin{ std::numeric_limits<component_type>::min() };

    protected: // construction
        template <typename ...ArgPackT>
        constexpr ColorCommonT(ArgPackT&& ...args) noexcept :
            base_type{ std::forward<ArgPackT>(args)... } {}

    public: // operators
        constexpr this_type& operator+=(const this_type& other) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] += other[i];
            }
            return *this;
        }

        constexpr this_type& operator-=(const this_type& other) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] -= other[i];
            }
            return *this;
        }

        constexpr this_type& operator*=(const this_type& other) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] *= other[i];
            }
            return *this;
        }

        constexpr this_type& operator/=(const this_type& other) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] /= other[i];
            }
            return *this;
        }

        constexpr this_type& operator+=(value_type value) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] += value;
            }
            return *this;
        }

        constexpr this_type& operator-=(value_type value) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] -= value;
            }
            return *this;
        }

        constexpr this_type& operator*=(value_type value) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] *= value;
            }
            return *this;
        }

        constexpr this_type& operator/=(value_type value) noexcept {
            for (size_type i = 0; i < size(); ++i) {
                (*this)[i] /= value;
            }
            return *this;
        }

        constexpr this_type operator+(const this_type& other) const noexcept {
            return this_type{ *this } += other;
        }

        constexpr this_type operator-(const this_type& other) const noexcept {
            return this_type{ *this } -= other;
        }

        constexpr this_type operator*(const this_type& other) const noexcept {
            return this_type{ *this } *= other;
        }

        constexpr this_type operator/(const this_type& other) const noexcept {
            return this_type{ *this } /= other;
        }

        constexpr this_type operator+(value_type other) const noexcept {
            return this_type{ *this } += other;
        }

        constexpr this_type operator-(value_type other) const noexcept {
            return this_type{ *this } -= other;
        }

        constexpr this_type operator*(value_type other) const noexcept {
            return this_type{ *this } *= other;
        }

        constexpr this_type operator/(value_type other) const noexcept {
            return this_type{ *this } /= other;
        }
    }; // template <...> class ColorCommonT
} // namespace Color::detail

//==============================================================================

namespace Color {
    //**************************************************************************
    // ColorT
    //**************************************************************************
    template <typename ComponentT, std::size_t CountT>
    class ColorT {};

    //==========================================================================

    template <typename ComponentT>
    class ColorT<ComponentT, 1> final :
        public ::Color::detail::ColorCommonT<ComponentT, 1> {
    private:
        using this_type = ColorT<ComponentT, 1>;
        using base_type = ColorCommonT<ComponentT, 1>;

    public:
        using typename base_type::component_type;

    public:
        ColorT() = default;

        constexpr ColorT(const this_type&  other) noexcept :
            base_type{ other } {}

        constexpr ColorT(      this_type&& other) noexcept :
            base_type{ std::forward<this_type>(other) } {}

        explicit constexpr ColorT(component_type _1) noexcept :
            base_type{ _1 } {}

        explicit constexpr ColorT(const component_type p[1]) noexcept :
            base_type{ p[0] } {}

        this_type& operator=(const this_type& ) = default;
        this_type& operator=(      this_type&&) = default;

    public: // setters
        constexpr void r(component_type val) noexcept { (*this)[0] = val; }

    public: // getters
        constexpr decltype(auto) r()       noexcept { return (*this)[0]; }
        constexpr decltype(auto) r() const noexcept { return (*this)[0]; }

    public: // conversion
        template <typename OtherComponentT>
        explicit operator ColorT<OtherComponentT, 1>() const noexcept {
            return color_cast<ColorT<OtherComponentT, 1>>(*this);
        }
    }; // template <...> class ColorT final

    //==========================================================================

    template <typename ComponentT>
    class ColorT<ComponentT, 2> final :
        public ::Color::detail::ColorCommonT<ComponentT, 2> {
    private:
        using this_type = ColorT<ComponentT, 2>;
        using base_type = ColorCommonT<ComponentT, 2>;

    public:
        using typename base_type::component_type;

    public:
        ColorT() = default;

        constexpr ColorT(const this_type&  other) : base_type{ other } {}
        constexpr ColorT(      this_type&& other) : base_type{ std::forward<this_type>(other) } {}

        constexpr ColorT(component_type _1, component_type _2) :
            base_type{ _1, _2 } {}

        explicit constexpr ColorT(const component_type p[2]) :
            base_type{ p[0], p[1] } {}

        this_type& operator=(const this_type& ) = default;
        this_type& operator=(      this_type&&) = default;

    public: // setters
        constexpr void r(component_type val) noexcept { (*this)[0] = val; }
        constexpr void g(component_type val) noexcept { (*this)[1] = val; }

    public: // getters
        constexpr decltype(auto) r()       noexcept { return (*this)[0]; }
        constexpr decltype(auto) r() const noexcept { return (*this)[0]; }
        constexpr decltype(auto) g()       noexcept { return (*this)[1]; }
        constexpr decltype(auto) g() const noexcept { return (*this)[1]; }

    public: // conversion
        template <typename OtherComponentT>
        explicit operator ColorT<OtherComponentT, 2>() const noexcept {
            return color_cast<ColorT<OtherComponentT, 2>>(*this);
        }
    }; // template <...> class ColorT final

    //==========================================================================

    template <typename ComponentT>
    class ColorT<ComponentT, 3> final :
        public ::Color::detail::ColorCommonT<ComponentT, 3> {
    private:
        using this_type = ColorT<ComponentT, 3>;
        using base_type = ColorCommonT<ComponentT, 3>;

    public:
        using typename base_type::component_type;

    public:
        ColorT() = default;

        constexpr ColorT(const this_type&  other) noexcept : base_type{ other } {}
        constexpr ColorT(      this_type&& other) noexcept : base_type{ std::forward<this_type>(other) } {}

        constexpr ColorT(component_type _1, component_type _2, component_type _3) noexcept :
            base_type{ _1, _2, _3 } {}

        explicit constexpr ColorT(const component_type p[3]) noexcept :
            base_type{ p[0], p[1], p[2] } {}

        this_type& operator=(const this_type& ) = default;
        this_type& operator=(      this_type&&) = default;

    public: // setters
        constexpr void r(component_type val) noexcept { (*this)[0] = val; }
        constexpr void g(component_type val) noexcept { (*this)[1] = val; }
        constexpr void b(component_type val) noexcept { (*this)[2] = val; }

    public: // getters
        constexpr decltype(auto) r()       noexcept { return (*this)[0]; }
        constexpr decltype(auto) r() const noexcept { return (*this)[0]; }
        constexpr decltype(auto) g()       noexcept { return (*this)[1]; }
        constexpr decltype(auto) g() const noexcept { return (*this)[1]; }
        constexpr decltype(auto) b()       noexcept { return (*this)[2]; }
        constexpr decltype(auto) b() const noexcept { return (*this)[2]; }

    public: // conversion
        template <typename OtherComponentT>
        explicit operator ColorT<OtherComponentT, 3>() const noexcept {
            return color_cast<ColorT<OtherComponentT, 3>>(*this);
        }
    }; // template <...> class ColorT final

    //==========================================================================

    template <typename ComponentT>
    class ColorT<ComponentT, 4> final :
        public ::Color::detail::ColorCommonT<ComponentT, 4> {
    private:
        using this_type = ColorT<ComponentT, 4>;
        using base_type = ::Color::detail::ColorCommonT<ComponentT, 4>;

    public:
        using typename base_type::component_type;

    public:
        ~ColorT() = default;

        ColorT() = default;

        constexpr ColorT(const this_type&  other) noexcept : base_type{ other } {}
        constexpr ColorT(      this_type&& other) noexcept : base_type{ std::forward<this_type>(other) } {}

        constexpr ColorT(component_type _1, component_type _2,
                         component_type _3, component_type _4) noexcept :
            base_type{ _1, _2, _3, _4 } {}

        explicit constexpr ColorT(const component_type p[4]) noexcept :
            base_type{ p[0], p[1], p[2], p[3] } {}

        this_type& operator=(const this_type& ) = default;
        this_type& operator=(      this_type&&) = default;

    public: // setters
        constexpr void r(component_type val) noexcept { (*this)[0] = val; }
        constexpr void g(component_type val) noexcept { (*this)[1] = val; }
        constexpr void b(component_type val) noexcept { (*this)[2] = val; }
        constexpr void a(component_type val) noexcept { (*this)[3] = val; }

    public: // getters
        constexpr decltype(auto) r()       noexcept { return (*this)[0]; }
        constexpr decltype(auto) r() const noexcept { return (*this)[0]; }
        constexpr decltype(auto) g()       noexcept { return (*this)[1]; }
        constexpr decltype(auto) g() const noexcept { return (*this)[1]; }
        constexpr decltype(auto) b()       noexcept { return (*this)[2]; }
        constexpr decltype(auto) b() const noexcept { return (*this)[2]; }
        constexpr decltype(auto) a()       noexcept { return (*this)[3]; }
        constexpr decltype(auto) a() const noexcept { return (*this)[3]; }

    public: // conversion
        template <typename OtherComponentT>
        explicit operator ColorT<OtherComponentT, 4>() const noexcept {
            return color_cast<ColorT<OtherComponentT, 4>>(*this);
        }
    }; // template <...> class ColorT final

    //**************************************************************************
    // ColorPaletteT
    //**************************************************************************
    template <typename ColorT>
    struct ColorPaletteT {
        using color_type = ::util::remove_cvref_t<ColorT>;

        color_type    Primary{ 0 };
        color_type  Secondary{ 0 };
        color_type Background{ 0 };
    }; // template <...> class ColorPaletteT

    //-------------------------------------------------------------------------

    template <typename ColorT, std::size_t CountT>
    struct ColorPaletteT<::Color::ColorT<ColorT, CountT>> {
        using color_type = ::Color::ColorT<ColorT, CountT>;

        color_type    Primary{ };
        color_type  Secondary{ };
        color_type Background{ };
    };

    //**************************************************************************
    // Specialisations
    //**************************************************************************
    template <typename TypeT>
    using Color1T = ::Color::ColorT<TypeT, 1>;

    template <typename TypeT>
    using Color2T = ::Color::ColorT<TypeT, 2>;

    template <typename TypeT>
    using Color3T = ::Color::ColorT<TypeT, 3>;

    template <typename TypeT>
    using Color4T = ::Color::ColorT<TypeT, 4>;

    // Color
    using Color3ub = ::Color::Color3T<std::uint8_t>;
    using Color3f  = ::Color::Color3T<float>;
    using Color4ub = ::Color::Color4T<std::uint8_t>;
    using Color4f  = ::Color::Color4T<float>;

    // ColorPalette
    using ColorPalette    = ::Color::ColorPaletteT<std::uint32_t>;
    using ColorPalette3ub = ::Color::ColorPaletteT<Color3ub>;
    using ColorPalette4ub = ::Color::ColorPaletteT<Color4ub>;
    using ColorPalette3f  = ::Color::ColorPaletteT<Color3f>;
    using ColorPalette4f  = ::Color::ColorPaletteT<Color4f>;
} // namespace Color

#endif // GUID_B9A272CC_A6CB_43C5_9096_25CCA86D59C0