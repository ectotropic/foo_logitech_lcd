#pragma once
#ifndef GUID_AD1A66A7_2111_4637_BD07_942386DD21BB
#define GUID_AD1A66A7_2111_4637_BD07_942386DD21BB
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
#include <utility>
//--------------------------------------

namespace util::detail {
    //**************************************************************************
    // scope_exit (implementation)
    //**************************************************************************
    template <typename TypeT>
    class scope_exit final {
    public:
        template <typename OtherTypeT = TypeT>
        scope_exit(OtherTypeT&& func) noexcept :
            fnExit_{ std::forward<OtherTypeT>(func) } {}

        ~scope_exit() noexcept {
            fnExit_();
        }

        scope_exit(const scope_exit& ) = delete; // No Copy
        scope_exit(      scope_exit&&) = delete; // No Move

        scope_exit& operator=(const scope_exit& ) = delete; // No Copy
        scope_exit& operator=(      scope_exit&&) = delete; // No Move

    private:
        TypeT fnExit_;
    }; // template <...> class scope_exit final
} // namespace util::detail

//==============================================================================

namespace util {
    //**************************************************************************
    // at_scope_exit
    //**************************************************************************
    template <typename FuncT>
    inline [[nodiscard]]
    decltype(auto) at_scope_exit(FuncT&& fn) noexcept {
        return ::util::detail::scope_exit<FuncT>{ std::forward<FuncT>(fn) };
    }
} // namespace util
#endif // GUID_AD1A66A7_2111_4637_BD07_942386DD21BB
