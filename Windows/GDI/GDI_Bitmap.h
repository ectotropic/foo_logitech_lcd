#pragma once
#ifndef GUID_9D46070A_EE8B_4716_8580_1F3C8B59F580
#define GUID_9D46070A_EE8B_4716_8580_1F3C8B59F580
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
#include "Windows/GDI/GDI.h"
//--------------------------------------

//--------------------------------------
//
#include <cstring> // memcpy, memcmp
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // BitmapInfoHeader
    //**************************************************************************
    struct BitmapInfoHeader : public BITMAPINFOHEADER {
    public:
        BitmapInfoHeader(BitmapInfoHeader&& other)            noexcept = default;
        BitmapInfoHeader& operator=(BitmapInfoHeader&& other) noexcept = default;

    public:
        BitmapInfoHeader() noexcept {
            ZeroMemory(this, sizeof(BitmapInfoHeader));
            biSize   = sizeof(BITMAPINFOHEADER);
            biPlanes = 1; //< Docs: "This value must be set to 1."
        }

        BitmapInfoHeader(const BitmapInfoHeader* other) noexcept {
            if (other) {
                std::memcpy(this, other, sizeof(BitmapInfoHeader));
            }
        }

        BitmapInfoHeader(const BitmapInfoHeader& other) noexcept :
            BitmapInfoHeader{ &other } {}

    public:
        BitmapInfoHeader& operator=(const BitmapInfoHeader* other) noexcept {
            if (other) { std::memcpy(this, other, sizeof(BitmapInfoHeader)); }
            return *this;
        }

        BitmapInfoHeader& operator=(const BitmapInfoHeader& other) noexcept {
            return *this = &other;
        }

        bool operator==(const BitmapInfoHeader* other) noexcept {
            return other && std::memcmp(this, other, sizeof(BitmapInfoHeader)) == 0;
        }

        auto operator==(const BitmapInfoHeader& other) noexcept {
            return *this == &other;
        }

        auto operator!=(const BitmapInfoHeader* other) noexcept {
            return !(*this == other);
        }

        auto operator!=(const BitmapInfoHeader& other) noexcept {
            return *this != &other;
        }

    public:
        operator       BITMAPINFOHEADER* ()       noexcept { return this; }
        operator const BITMAPINFOHEADER* () const noexcept { return this; }
    };
} // namespace Windows::GDI

//==============================================================================

namespace Windows::GDI::detail {
    //**************************************************************************
    // RGBQuadT
    //**************************************************************************
    template <UINT ColorCountT>
    struct RGBQuadT {
    private:
        using thisStruct = RGBQuadT<ColorCountT>;

    public:
        static constexpr const auto ColorCount{ ColorCountT };

    public:
        RGBQuadT() noexcept {
            ZeroMemory(this, sizeof(thisStruct));
        }

        RGBQuadT(const thisStruct* other) noexcept {
            if (other) { std::memcpy(this, other, sizeof(thisStruct)); }
        }

        RGBQuadT(const thisStruct& other) noexcept :
            RGBQuadT{ &other } {}

        RGBQuadT& operator=(const thisStruct* other) noexcept {
            if (other) { std::memcpy(this, other, sizeof(thisStruct)); }
            return *this;
        }

    public:
        RGBQuadT& operator=(const thisStruct& other) noexcept {
            return *this = &other;
        }

        bool operator==(const thisStruct* other) noexcept {
            return other && std::memcmp(this, other, sizeof(thisStruct)) == 0;
        }
        auto operator==(const thisStruct& other) noexcept { return *this == &other; }
        auto operator!=(const thisStruct* other) noexcept { return !(*this == other); }
        auto operator!=(const thisStruct& other) noexcept { return *this != &other; }

        const auto& operator [](int i) const noexcept {
            return bmiColors[i];
        }

        auto& operator [](int i) noexcept {
            return bmiColors[i];
        }

    public:
        operator       RGBQUAD*()       noexcept { return bmiColors; }
        operator const RGBQUAD*() const noexcept { return bmiColors; }

    public:
        RGBQUAD bmiColors[ColorCount]{};
    }; // template <...> struct RGBQuadT

    //**************************************************************************
    // BitmapInfoT
    //**************************************************************************
    template <UINT ColorCountT>
    struct BitmapInfoT {
    private:
        using thisStruct = BitmapInfoT<ColorCountT>;
        using rgbStruct  = RGBQuadT<ColorCountT>;

    public:
        static constexpr const auto ColorCount{ ColorCountT };

    public:
        BitmapInfoT() noexcept = default;

        BitmapInfoT(_In_opt_ const BitmapInfoHeader* otherHeader,
                    _In_opt_ const rgbStruct* otherColors) noexcept :
            bmiHeader{ otherHeader ? otherHeader : BitmapInfoHeader{} },
            bmiColors{ otherColors ? otherColors : rgbStruct{} } {}

        BitmapInfoT(_In_opt_ const thisStruct* other) noexcept :
            BitmapInfoT{
                other ? &other->bmiHeader : nullptr,
                other ? &other->bmiColors : nullptr } {}

        BitmapInfoT(const thisStruct& other) noexcept :
            BitmapInfoT{ &other } {}

    public:
        thisStruct& operator=(const thisStruct* other) noexcept {
            if (other) {
                bmiHeader = other->bmiHeader;
                bmiColors = other->bmiColors;
            }
            return *this;
        }

        thisStruct& operator=(const thisStruct& other) noexcept {
            return *this = &other;
        }

        bool operator==(const thisStruct* other) noexcept {
            return other &&
                bmiHeader == other->bmiHeader &&
                bmiColors == other->bmiColors;
        }

        auto operator==(const thisStruct& other) noexcept {
            return *this == &other;
        }

        auto operator!=(const thisStruct* other) noexcept {
            return !(*this == other);
        }

        auto operator!=(const thisStruct& other) noexcept {
            return *this != &other;
        }

    public:
        operator       BitmapInfoHeader* ()       noexcept { return &bmiHeader; }
        operator const BitmapInfoHeader* () const noexcept { return &bmiHeader; }

        operator       BITMAPINFOHEADER* ()       noexcept { return &bmiHeader; }
        operator const BITMAPINFOHEADER* () const noexcept { return &bmiHeader; }

        operator       rgbStruct* ()       noexcept { return &bmiColors; }
        operator const rgbStruct* () const noexcept { return &bmiColors; }

        operator       RGBQUAD* ()       noexcept { return bmiColors; }
        operator const RGBQUAD* () const noexcept { return bmiColors; }

        operator BITMAPINFO* () noexcept {
            return reinterpret_cast<BITMAPINFO*>(this);
        }

        operator const BITMAPINFO* () const noexcept {
            return reinterpret_cast<const BITMAPINFO*>(this);
        }

    public:
        BitmapInfoHeader bmiHeader{};
        rgbStruct        bmiColors{};
    }; // template <...> struct BitmapInfoT
} // namespace Windows::GDI::detail

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // Types
    //**************************************************************************
    using RGBQuad    = detail::RGBQuadT<1>;
    using BitmapInfo = detail::BitmapInfoT<1>;

    using RGBQuad256    = detail::RGBQuadT<256>;
    using BitmapInfo256 = detail::BitmapInfoT<256>;
} // namespace Windows::GDI

#endif // GUID_9D46070A_EE8B_4716_8580_1F3C8B59F580