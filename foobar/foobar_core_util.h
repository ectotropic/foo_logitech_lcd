#pragma once
#ifndef GUID_0A832AA3_7455_4C00_BB6C_B04C401707B5
#define GUID_0A832AA3_7455_4C00_BB6C_B04C401707B5
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
#include "foobar/foobar_sdk.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/TypeTraits.h"
//--------------------------------------

namespace foobar::detail {
    //**************************************************************************
    // make_list_impl
    //**************************************************************************
    template <typename ElementT>
    class make_list_impl final {
    public:
        using element_type = ::util::remove_cvref_t<ElementT>;
        using list_type    = ::pfc::list_t<element_type>;

    protected:
        static void make_list(list_type& p_list,
                              const element_type& p_element) {
            p_list.add_item(p_element);
        }

        template <typename ...ArgPackT>
        static void make_list(list_type& p_list,
                              const element_type& p_element,
                              ArgPackT... p_args) {
            make_list(p_list, p_element);
            make_list(p_list, p_args...);
        }

    public:
        static auto make_list(const element_type& p_element) {
            list_type elem_list{};
            elem_list.prealloc(1);
            make_list(elem_list, p_element);
            return elem_list;
        }

        template <typename ...ArgPackT>
        static auto make_list(const element_type& p_element,
                              ArgPackT... p_args) {
            list_type elem_list{};
            elem_list.prealloc(sizeof...(ArgPackT) + 1);
            make_list(elem_list, p_element, p_args...);
            return elem_list;
        }
    }; // template<...> class make_list_impl final
} // namespace foobar::detail

//==============================================================================

namespace foobar {
    //**************************************************************************
    // abort_callback_param
    //**************************************************************************
    struct abort_callback_param final {
        abort_callback_param() noexcept = default;

        abort_callback_param(abort_callback& p_callback) noexcept :
            m_callback{ &p_callback } {}

        abort_callback_param(abort_callback* p_callback) noexcept :
            m_callback{ p_callback } {}

        abort_callback_param(const abort_callback_param& other) noexcept :
            m_callback{ other.m_callback } {}

        abort_callback_param(abort_callback_param&& other) noexcept :
            m_callback{ std::exchange(other.m_callback, nullptr) } {}

        abort_callback_param& operator=(const abort_callback_param& other) noexcept {
            m_callback = other.m_callback;
            return *this;
        }

        abort_callback_param& operator=(abort_callback_param&& other) noexcept {
            m_callback = std::exchange(other.m_callback, nullptr);
            return *this;
        }

        constexpr operator abort_callback& () noexcept {
            return m_callback ? *m_callback : m_dummy;
        }

        constexpr operator abort_callback* () noexcept {
            return m_callback ? m_callback : &m_dummy;
        }

        ::abort_callback_dummy m_dummy{};
        ::abort_callback*      m_callback{ nullptr };
    }; // struct abort_callback_param final

    //**************************************************************************
    // make_list
    //**************************************************************************
    template <typename ElementT>
    inline [[nodiscard]]
    decltype(auto) make_list(ElementT p_element) {
        using impl_type =
            foobar::detail::make_list_impl<ElementT>;
        return impl_type::make_list(p_element);
    }

    //-------------------------------------------------------------------------

    template <typename ElementT, typename ...ArgPackT>
    inline [[nodiscard]]
    decltype(auto) make_list(ElementT p_element, ArgPackT... p_args) {
        using impl_type =
            foobar::detail::make_list_impl<ElementT>;
        return impl_type::make_list(p_element, p_args...);
    }

    //**************************************************************************
    // Playback helpers
    //**************************************************************************
    inline [[nodiscard]]
    auto is_playing(const ::playback_control::ptr& p_ctrl) {
        return p_ctrl.is_valid() && p_ctrl->is_playing();
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    auto is_paused(const ::playback_control::ptr& p_ctrl) {
        return p_ctrl.is_valid() && p_ctrl->is_paused();
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    auto is_stopped(const ::playback_control::ptr& p_ctrl) {
        return !(is_playing(p_ctrl) || is_paused(p_ctrl));
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    auto get_now_playing(const ::playback_control::ptr& p_ctrl) {
        ::metadb_handle::ptr p_metadb{};
        if (p_ctrl.is_valid()) {
            if (!is_stopped(p_ctrl)) {
                if (!p_ctrl->get_now_playing(p_metadb)) {
                    p_metadb.reset();
                }
            }
        }
        return p_metadb;
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) is_playing() {
        return is_playing(::playback_control::get());
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) is_paused() {
        return is_paused(::playback_control::get());
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) is_stopped() {
        return is_stopped(::playback_control::get());
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) get_now_playing() {
        return get_now_playing(::playback_control::get());
    }
} // namespace foobar

#endif // GUID_0A832AA3_7455_4C00_BB6C_B04C401707B5
