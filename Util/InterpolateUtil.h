#pragma once
#ifndef GUID_EB745D91_602D_4699_9B55_3D5877F248CA
#define GUID_EB745D91_602D_4699_9B55_3D5877F248CA
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
#include "Util/MemoryUtil.h"
//--------------------------------------

//--------------------------------------
//
#include <memory>
#include <vector>
#include <type_traits>
#include <cassert>
//--------------------------------------

namespace util {
    //**************************************************************************
    //  lerp
    //**************************************************************************
    template <
        typename TypeT,
        typename InterpT = TypeT
    >
    inline [[nodiscard]]
    constexpr auto lerp(TypeT prev, TypeT next, InterpT interp) noexcept {
        return prev + (next - prev) * interp;
    }

    //------------------------------------------------------

    template <
        typename TypeT,
        typename InterpT = typename ::util::data_span<TypeT>::value_type
    >
    inline void lerp(::util::data_span<TypeT> curr,
                     ::util::data_span<const TypeT> prev,
                     ::util::data_span<const TypeT> next,
                     InterpT interp) noexcept {
        assert(std::size(curr) >= std::size(prev) &&
               std::size(curr) >= std::size(next));
        if (std::size(curr) < std::size(prev) ||
            std::size(curr) < std::size(next)) {
            for (auto& c : curr) { c = {}; }
        }
        auto it_prev{ std::cbegin(prev) };
        auto it_next{ std::cbegin(next) };
        for (auto& c : curr) {
            c = lerp(*it_prev, *it_next, interp);
            ++it_prev; ++it_next;
        }
    }

    //------------------------------------------------------

    template <
        typename TypeT,
        class AllocatorT = std::allocator<TypeT>,
        typename InterpT = typename std::vector<TypeT, AllocatorT>::value_type
    >
    inline [[nodiscard]]
    auto lerp(const std::vector<TypeT, AllocatorT>& prev,
              const std::vector<TypeT, AllocatorT>& next,
              InterpT interp) {
        std::vector<TypeT, AllocatorT> curr{};
        curr.resize(std::min(prev.size(), next.size()));
        lerp(::util::data_span<TypeT>{ curr },
             ::util::data_span<const TypeT>{ prev },
             ::util::data_span<const TypeT>{ next },
             interp);
        return curr;
    }

    //**************************************************************************
    // interpolated_value
    //**************************************************************************
    template <typename TypeT>
    class interpolated_value {
    private:
        using this_type = interpolated_value<TypeT>;

    public:
        using value_type      = TypeT;
        using size_type       = std::size_t;
        using reference       = TypeT&;
        using const_reference = const TypeT&;
        using pointer         = TypeT*;
        using const_pointer   = const TypeT*;

        struct pair_type final {
            value_type prev{ 0 };
            value_type next{ 0 };
        };

        struct reference_pair_type final {
            const_reference prev;
            reference       next;
        };

    public:
        interpolated_value()                   noexcept     = default;
        interpolated_value(const this_type& )  noexcept     = default;
        interpolated_value(      this_type&&)  noexcept     = default;

        this_type& operator=(const this_type& ) noexcept    = default;
        this_type& operator=(      this_type&&) noexcept    = default;

    public:
        constexpr interpolated_value(const value_type& prev,
                                     const value_type& next) noexcept :
            m_Prev{ prev },
            m_Next{ next } {}

        constexpr interpolated_value(const value_type& value) noexcept :
            interpolated_value{ value, value } {}

    public:
        void swap(this_type& other) noexcept {
            using std::swap;
            swap(m_Prev, other.m_Prev);
            swap(m_Next, other.m_Next);
        }

    public:
        constexpr void clear() noexcept { m_Next = m_Prev = 0; }
        constexpr void reset() noexcept { clear(); }
        constexpr void zero () noexcept { clear(); }

    public:
        void update(const value_type val) noexcept {
            using std::swap;
            swap(m_Prev, m_Next);
            m_Next = val;
        }

    public:
        constexpr auto prev() const noexcept { return m_Prev; }
        constexpr auto next()       noexcept { return m_Next; }
        constexpr auto next() const noexcept { return m_Next; }

        template <typename InterpT = value_type>
        constexpr auto curr(InterpT interp) const noexcept {
            return ::util::lerp(m_Prev, m_Next, interp);
        }

        constexpr auto get()       noexcept { return reference_pair_type{ m_Prev, m_Next }; }
        constexpr auto get() const noexcept { return           pair_type{ m_Prev, m_Next }; }

    protected:
        value_type m_Prev{ 0 };
        value_type m_Next{ 0 };
    }; // template <...> class interpolated_value

    //******************************************************************************
    // interpolated_buffer
    //******************************************************************************
    template <
        typename TypeT,
        class AllocatorT = std::allocator<TypeT>
    >
    class interpolated_buffer {
    private:
        using this_type = interpolated_buffer<TypeT, AllocatorT>;

    public:
        using buffer_type     = std::vector<TypeT, AllocatorT>;
        using value_type      = typename buffer_type::value_type;
        using size_type       = typename buffer_type::size_type;
        using allocator_type  = typename buffer_type::allocator_type;
        using reference       = typename buffer_type::reference;
        using const_reference = typename buffer_type::const_reference;
        using pointer         = typename buffer_type::pointer;
        using const_pointer   = typename buffer_type::const_pointer;
        using span_type       = ::util::data_span<value_type>;
        using const_span_type = ::util::data_span<const value_type>;

        struct pair_type final {
            value_type prev{ 0 };
            value_type next{ 0 };
        }; // struct pair_type final

        struct reference_pair_type final {
            const_reference prev;
            reference       next;
        }; // struct reference_pair_type final

    public:
        interpolated_buffer()                   noexcept    = default;
        interpolated_buffer(const this_type& )              = default;
        interpolated_buffer(      this_type&&)  noexcept    = default;

        this_type& operator=(const this_type& )             = default;
        this_type& operator=(      this_type&&) noexcept    = default;

    public:
        interpolated_buffer(const buffer_type& prev,
                            const buffer_type& next) :
            m_Prev{ prev },
            m_Next{ next } {}

        interpolated_buffer(buffer_type&& prev,
                            buffer_type&& next) noexcept :
            m_Prev{ std::forward<buffer_type>(prev) },
            m_Next{ std::forward<buffer_type>(next) } {}

        explicit interpolated_buffer(const buffer_type& buffer) :
            interpolated_buffer{ buffer, buffer } {}

        explicit interpolated_buffer(buffer_type&& buffer) :
            m_Prev{ std::forward<buffer_type>(buffer) },
            m_Next{ m_Prev } {}

        explicit interpolated_buffer(size_type num) {
            resize(num);
        }

    public:
        decltype(auto) size    ()  const noexcept { assert(valid()); return m_Prev.size(); }
        decltype(auto) capacity()  const noexcept { assert(valid()); return m_Prev.capacity(); }
        decltype(auto) empty   ()  const noexcept { assert(valid()); return m_Prev.empty(); }

        auto total_size    ()  const noexcept { assert(valid()); return m_Prev.size() + m_Next.size(); }
        auto total_capacity()  const noexcept { assert(valid()); return m_Prev.capacity() + m_Next.capacity(); }

    public:
        void swap(this_type& other) noexcept {
            assert(valid() && other.valid());
            using std::swap;
            swap(m_Prev, other.m_Prev);
            swap(m_Next, other.m_Next);
        }

        void shrink_to_fit() {
            assert(valid());
            if (empty()) { return; }
            m_Prev.shrink_to_fit();
            m_Next.shrink_to_fit();
        }

        void clear() noexcept {
            assert(valid());
            if (empty()) { return; }
            m_Prev.clear();
            m_Next.clear();
        }

        void reset() {
            clear();
            shrink_to_fit();
        }

        void zero() noexcept {
            assert(valid());
            if (empty()) { return; }
            ::util::zero(m_Prev);
            ::util::zero(m_Next);
        }

        //--------------------------------------------------
        // `resize` && `reserve`:
        // ----------------------
        //
        // Want to ensure that both buffers **always**
        // have the same size, even on error.
        //
        // Simplest solution is to do the reallocation
        // in temporary variables, as is done here. This
        // should be no different in performance than
        // simply resizing the buffers, but ensures both
        // buffers maintain the same size at all times.
        //
        // Unfortunately if an error does occur the
        // buffers will be left empty, but avoiding this
        // requires more complex code which is likely
        // overkill for a worst case scenario which is
        // likely to be fatal anyway.
        //
        // NOTE: Assumes `move` is `noexcept`
        void resize(size_type num) {
            assert(valid());
            if (num == m_Prev.size()) { return; }
            auto prev{ std::move(m_Prev) };
            auto next{ std::move(m_Next) };
            prev.resize(num);
            next.resize(num);
            m_Prev = std::move(prev);
            m_Next = std::move(next);
        }

        // See notes for `resize`
        void reserve(size_type num) {
            assert(valid());
            if (num <= m_Prev.capacity()) { return; }
            auto prev{ std::move(m_Prev) };
            auto next{ std::move(m_Next) };
            prev.reserve(num);
            next.reserve(num);
            m_Prev = std::move(prev);
            m_Next = std::move(next);
        }
        //--------------------------------------------------

    public:
        [[nodiscard]]
        auto data_for_update() noexcept {
            assert(valid());
            using std::swap;
            swap(m_Prev, m_Next);
            return span_type{ m_Next };
        }

        void update(const buffer_type& data) noexcept {
            assert(valid());
            using std::swap;
            swap(m_Prev, m_Next);
            ::util::copy_min(m_Next, data);
        }

        void update(buffer_type&& data) noexcept {
            assert(valid());
            using std::swap;
            swap(m_Prev, m_Next);
            if (data.size() == m_Next.size()) {
                m_Next = std::forward<buffer_type>(data);
            } else {
                ::util::copy_min_n(m_Next,
                                     ::util::data(data),
                                     ::util::size(data));
            }
        }

        void update(const_span_type data) noexcept {
            assert(valid());
            using std::swap;
            swap(m_Prev, m_Next);
            ::util::copy_min_n(m_Next,
                                 ::util::data(data),
                                 ::util::size(data));
        }

    public:
        bool valid() const noexcept {
            return m_Prev.size() == m_Next.size();
        }

        explicit operator bool() const noexcept {
            assert(valid());
            return !empty();
        }

    public:
        constexpr auto& prev()  const noexcept { assert(valid()); return m_Prev; }
        constexpr auto& next()        noexcept { assert(valid()); return m_Next; }
        constexpr auto& next()  const noexcept { assert(valid()); return m_Next; }

        decltype(auto)  prev(size_type pos) const noexcept { assert(valid()); return m_Prev[pos]; }
        decltype(auto)  next(size_type pos)       noexcept { assert(valid()); return m_Next[pos]; }
        decltype(auto)  next(size_type pos) const noexcept { assert(valid()); return m_Next[pos]; }

        decltype(auto)  prev_at(size_type pos) const noexcept { assert(valid()); return m_Prev.at(pos); }
        decltype(auto)  next_at(size_type pos)       noexcept { assert(valid()); return m_Next.at(pos); }
        decltype(auto)  next_at(size_type pos) const noexcept { assert(valid()); return m_Next.at(pos); }

        template <typename InterpT = value_type>
        decltype(auto) curr(InterpT interp) const {
            assert(valid());
            return ::util::lerp(m_Prev, m_Next, interp);
        }

        template <typename InterpT = value_type>
        decltype(auto) curr(size_type pos,
                            InterpT interp) const noexcept {
            assert(valid());
            return ::util::lerp(m_Prev[pos], m_Next[pos], interp);
        }

        template <typename InterpT = value_type>
        decltype(auto) curr_at(size_type pos,
                               InterpT interp) const noexcept {
            assert(valid());
            return ::util::lerp(m_Prev.at(pos), m_Next.at(pos), interp);
        }

        auto at(size_type pos) {
            assert(valid());
            return reference_pair_type{ m_Prev.at(pos), m_Next.at(pos) };
        }

        auto at(size_type pos) const {
            assert(valid());
            return pair_type{ m_Prev.at(pos), m_Next.at(pos) };
        }

        auto operator[](size_type pos) noexcept {
            assert(valid());
            return reference_pair_type{ m_Prev[pos], m_Next[pos] };
        }

        auto operator[](size_type pos) const noexcept {
            assert(valid());
            return pair_type{ m_Prev[pos], m_Next[pos] };
        }

    protected:
        buffer_type m_Prev{};
        buffer_type m_Next{};
    }; // template <...> class interpolated_buffer
} // namespace util

#endif // GUID_EB745D91_602D_4699_9B55_3D5877F248CA
