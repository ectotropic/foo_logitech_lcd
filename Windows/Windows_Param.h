#pragma once
#ifndef GUID_28AFDFC6_D2F1_4B67_8C86_9B33D284A240
#define GUID_28AFDFC6_D2F1_4B67_8C86_9B33D284A240
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
#include "Windows/Windows_Core.h"
//--------------------------------------

namespace Windows::detail {
    //**************************************************************************
    // ClassNameParam
    //**************************************************************************
    struct ClassNameParam final {
        ClassNameParam() = delete;

        constexpr ClassNameParam(_In_z_ LPCTSTR szName) noexcept :
            szClassName{ szName } {}

        ClassNameParam(ATOM atomName) noexcept :
            szClassName{ reinterpret_cast<LPCTSTR>(atomName) } {}

        constexpr operator LPCTSTR() const noexcept {
            return szClassName;
        }

        LPCTSTR szClassName{ NULL };
    }; // struct ClassNameParam final

    //**************************************************************************
    // ResourceNameParam
    //**************************************************************************
    struct ResourceNameParam final {
        ResourceNameParam() = delete;

        constexpr ResourceNameParam(_In_z_ LPCTSTR szName) noexcept :
            szResourceName{ szName } {}

        constexpr ResourceNameParam(WORD name) noexcept :
            szResourceName{ MAKEINTRESOURCE(name) } {}

        constexpr operator LPCTSTR() const noexcept {
            return szResourceName;
        }

        LPCTSTR szResourceName{ NULL };
    }; // struct ResourceNameParam final

    //**************************************************************************
    // ParamT
    // ------
    //
    // Accept function parameters by reference or pointer.
    //**************************************************************************
    template <typename TypeT>
    struct ParamT final {
        using param_type           = TypeT;
        using pointer_type         = TypeT*;
        using const_pointer_type   = const TypeT*;
        using reference_type       = TypeT&;
        using const_reference_type = const TypeT&;

        ParamT() = delete;

        constexpr ParamT(TypeT* param) noexcept :
            lpParam{ param } {}

        constexpr ParamT(reference_type param) noexcept :
            lpParam{ &param } {}

        constexpr explicit operator bool() const noexcept {
            return lpParam != nullptr;
        }

        constexpr operator const_pointer_type() const noexcept {
            assert(lpParam);
            return lpParam;
        }

        constexpr operator pointer_type() noexcept {
            assert(lpParam);
            return lpParam;
        }

        constexpr const_reference_type operator*() const noexcept {
            assert(lpParam);
            return *lpParam;
        }

        constexpr reference_type operator*() noexcept {
            assert(lpParam);
            return *lpParam;
        }

        constexpr const_pointer_type operator->() const noexcept {
            assert(lpParam);
            return lpParam;
        }

        constexpr pointer_type operator->() noexcept {
            assert(lpParam);
            return lpParam;
        }

        pointer_type lpParam{ nullptr };
    }; // struct ParamT final
} // namespace Windows::detail

#endif // #ifndef GUID_28AFDFC6_D2F1_4B67_8C86_9B33D284A240