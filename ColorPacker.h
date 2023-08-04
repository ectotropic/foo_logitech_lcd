#pragma once
#ifndef GUID_59E0235D_9729_48EB_991C_C6A5B119A087
#define GUID_59E0235D_9729_48EB_991C_C6A5B119A087
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
#include "ColorCast.h"
#include "ColorTraits.h"
//--------------------------------------

//--------------------------------------
//
#include <cstdint>
#include <limits>
//--------------------------------------

namespace Color {
    //**************************************************************************
    // PackedColorT
    //**************************************************************************
    template <typename PackedT, typename ComponentT>
    struct PackedColorT final {
    private:
        static_assert(sizeof(PackedT) >= (sizeof(ComponentT) * 4),
                      "PackedColorT sizeof(packed_type) is less than >= sizeof(component_type) * 4!");

    private:
        using this_type = PackedColorT<PackedT, ComponentT>;

    public:
        using packed_type    = ::util::remove_cvref_t<PackedT>;
        using component_type = ::util::remove_cvref_t<ComponentT>;
        using size_type      = ::util::unsigned_integral_type_t<packed_type>;

        using color3_type = ::Color::ColorT<component_type, 3>;
        using color4_type = ::Color::ColorT<component_type, 4>;

    private:
        inline static constexpr const packed_type _component_mask{
            static_cast<size_type>(std::numeric_limits<component_type>::max()) -
                static_cast<size_type>(std::numeric_limits<component_type>::min())
        };

        inline static /*constexpr*/ const packed_type _shift[4]{
            (sizeof(packed_type) - sizeof(component_type) * 1) * 8,
            (sizeof(packed_type) - sizeof(component_type) * 2) * 8,
            (sizeof(packed_type) - sizeof(component_type) * 3) * 8,
            (sizeof(packed_type) - sizeof(component_type) * 4) * 8
        };

        inline static /*constexpr*/ const packed_type _mask[4]{
            _component_mask << _shift[0],
            _component_mask << _shift[1],
            _component_mask << _shift[2],
            _component_mask << _shift[3]
        };

        template <size_type i>
        static constexpr auto PackChannel(component_type c) noexcept {
            return _mask[i] & (static_cast<packed_type>(c & _component_mask) << _shift[i]);
        }

        template <size_type i>
        static constexpr auto UnpackChannel(packed_type packed) noexcept {
            return static_cast<component_type>(_component_mask & ((packed & _mask[i]) >> _shift[i]));
        }

        //--------------------------------------------------
        // ChannelPackerT
        //--------------------------------------------------
        template <typename ChannelImplT>
        struct ChannelPackerT final : public ChannelImplT {
            static constexpr auto Pack  (component_type c) noexcept { return   PackChannel<index>(c); }
            static constexpr auto Unpack(packed_type    p) noexcept { return UnpackChannel<index>(p); }
        }; // template <...> struct ChannelPackerT final

        //--------------------------------------------------
        // UnpackDispatcherT
        //--------------------------------------------------
        template <typename PackerT, typename TargetT>
        struct UnpackDispatcherT {
            using packer_type       = PackerT;
            using target_color_type = ::util::color_type_t<TargetT, 4>;

            static constexpr target_color_type Unpack(packed_type packed) noexcept {
                color4_type unpacked{ };
                packer_type::Unpack(packed, unpacked);
                return color_cast<target_color_type>(unpacked);
            }
        }; // template <...> struct UnpackDispatcherT final

        //------------------------------

        template <typename PackerT, typename TargetT>
        struct UnpackDispatcherT<PackerT, ::Color::Color3T<TargetT>> {
            using packer_type       = PackerT;
            using target_color_type = ::util::color_type_t<TargetT, 3>;

            static constexpr target_color_type Unpack(packed_type packed) noexcept {
                color3_type unpacked{ };
                packer_type::Unpack(packed, unpacked);
                return color_cast<target_color_type>(unpacked);
            }
        }; // template <...> struct UnpackDispatcherT final

        //--------------------------------------------------
        // PackerT
        //--------------------------------------------------
        template <typename ImplT>
        struct PackerT final : public ImplT {
            static constexpr auto Pack(component_type r,
                                       component_type g,
                                       component_type b,
                                       component_type a) noexcept {
                return R::Pack(r) | G::Pack(g) | B::Pack(b) | A::Pack(a);
            }

            static constexpr auto Pack(const color4_type& col) noexcept {
                return Pack(col.r(), col.g(), col.b(), col.a());
            }

            template <typename TypeT>
            static constexpr auto Pack(const ::Color::ColorT<TypeT, 4>& col) noexcept {
                return Pack(::Color::color_cast<color4_type>(col));
            }

            static constexpr auto Pack(component_type r,
                                       component_type g,
                                       component_type b) noexcept {
                return R::Pack(r) | G::Pack(g) | B::Pack(b);
            }

            static constexpr auto Pack(const color3_type& col) noexcept {
                return Pack(col.r(), col.g(), col.b());
            }

            template <typename TypeT>
            static constexpr auto Pack(const ::Color::ColorT<TypeT, 3>& col) noexcept {
                return Pack(::Color::color_cast<color3_type>(col));
            }

            static constexpr void Unpack(packed_type packed,
                                         component_type& r,
                                         component_type& g,
                                         component_type& b,
                                         component_type& a) noexcept {
                r = R::Unpack(packed); g = G::Unpack(packed);
                b = B::Unpack(packed); a = A::Unpack(packed);
            }

            static constexpr void Unpack(packed_type packed,
                                         color4_type& unpacked) noexcept {
                Unpack(packed,
                       unpacked.r(), unpacked.g(),
                       unpacked.b(), unpacked.a());
            }

            static constexpr void Unpack(packed_type packed,
                                         component_type& r,
                                         component_type& g,
                                         component_type& b) noexcept {
                r = R::Unpack(packed); g = G::Unpack(packed);
                b = B::Unpack(packed);
            }

            static constexpr void Unpack(packed_type packed,
                                         color3_type& unpacked) noexcept {
                Unpack(packed, unpacked.r(), unpacked.g(), unpacked.b());
            }

            template <typename TargetT>
            static constexpr decltype(auto) Unpack(packed_type packed) noexcept {
                return UnpackDispatcherT<PackerT<ImplT>, TargetT>::Unpack(packed);
            }

            template <typename TargetT>
            static constexpr void Unpack(packed_type packed,
                                         ::Color::Color3T<TargetT>& unpacked) noexcept {
                color3_type temp{};
                Unpack(packed, temp);
                unpacked = color_cast<::Color::Color3T<TargetT>>(temp);
            }

            template <typename TargetT>
            static constexpr void Unpack(packed_type packed,
                                         ::Color::Color4T<TargetT>& unpacked) noexcept {
                color4_type temp{};
                Unpack(packed, temp);
                unpacked = color_cast<::Color::Color4T<TargetT>>(temp);
            }

            using Red   = ChannelPackerT<typename ImplT::Red>;
            using Green = ChannelPackerT<typename ImplT::Green>;
            using Blue  = ChannelPackerT<typename ImplT::Blue>;
            using Alpha = ChannelPackerT<typename ImplT::Alpha>;
            using R = Red;
            using G = Green;
            using B = Blue;
            using A = Alpha;
        }; // template <...> struct PackerT final

    public:
        //------------------------------
        // RGBAImpl
        //------------------------------
        struct RGBAImpl {
        protected:
            struct Red   { inline static constexpr size_type index{ 0 }; };
            struct Green { inline static constexpr size_type index{ 1 }; };
            struct Blue  { inline static constexpr size_type index{ 2 }; };
            struct Alpha { inline static constexpr size_type index{ 3 }; };
        }; // struct RGBAImpl

        //------------------------------
        // BGRAImpl
        //------------------------------
        struct BGRAImpl {
        protected:
            struct Blue  { inline static constexpr size_type index{ 0 }; };
            struct Green { inline static constexpr size_type index{ 1 }; };
            struct Red   { inline static constexpr size_type index{ 2 }; };
            struct Alpha { inline static constexpr size_type index{ 3 }; };
        }; // struct BGRAImpl

        //------------------------------
        // ARGBImpl
        //------------------------------
        struct ARGBImpl {
        protected:
            struct Alpha { inline static constexpr size_type index{ 0 }; };
            struct Red   { inline static constexpr size_type index{ 1 }; };
            struct Green { inline static constexpr size_type index{ 2 }; };
            struct Blue  { inline static constexpr size_type index{ 3 }; };
        }; // struct ARGBImpl

        //------------------------------
        // ABGRImpl
        //------------------------------
        struct ABGRImpl {
        protected:
            struct Alpha { inline static constexpr size_type index{ 0 }; };
            struct Blue  { inline static constexpr size_type index{ 1 }; };
            struct Green { inline static constexpr size_type index{ 2 }; };
            struct Red   { inline static constexpr size_type index{ 3 }; };
        }; // struct ABGRImpl

    public:
        using RGBA = PackerT<RGBAImpl>;
        using BGRA = PackerT<BGRAImpl>;
        using ARGB = PackerT<ARGBImpl>;
        using ABGR = PackerT<ABGRImpl>;
        using RGB  = ARGB;
        using BGR  = ABGR;
    }; // template <...> struct PackedColorT final

    //**************************************************************************
    // PackedColor32ui
    //**************************************************************************
    using PackedColor32ui = ::Color::PackedColorT<std::uint32_t, std::uint8_t>;
} // namespace Color

#endif // GUID_59E0235D_9729_48EB_991C_C6A5B119A087