#pragma once
#ifndef GUID_F66ACDB7_769F_479D_BCF2_D2C43909E550
#define GUID_F66ACDB7_769F_479D_BCF2_D2C43909E550
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
#include <stdexcept>
#include <cassert>
#include <cstring> // memset/memcpy
//--------------------------------------

namespace util {
    //**************************************************************************
    // data_span
    //**************************************************************************
    template <typename TypeT>
    class data_span final {
    private:
        using this_type = data_span<TypeT>;

    public:
        using element_type           = TypeT;
        using value_type             = std::remove_cv_t<TypeT>;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = TypeT&;
        using const_reference        = const TypeT&;
        using pointer                = TypeT*;
        using const_pointer          = const TypeT*;
        using iterator               = TypeT*;
        using const_iterator         = const TypeT*;
        using reverse_iterator       = TypeT*;
        using const_reverse_iterator = const TypeT*;

    private:
        data_span(const data_span&)            = delete; // No Copy
        data_span& operator=(const data_span&) = delete; // No Copy

    public:
        data_span(pointer data, size_type size) noexcept :
            m_Data{ data },
            m_Size{ size } {}

        template <class ContainerT>
        data_span(ContainerT& src) noexcept :
            m_Data{ std::data(src) },
            m_Size{ std::size(src) } {}

        data_span(data_span&& other) noexcept :
            m_Data{ std::exchange(other.m_Data, nullptr) },
            m_Size{ std::exchange(other.m_Size, 0) } {}

        data_span& operator=(data_span&& other) noexcept {
            m_Data = std::exchange(other.m_Data, nullptr);
            m_Size = std::exchange(other.m_Size, 0);
            return *this;
        }

    public:
        constexpr auto  begin  ()       noexcept { assert(m_Size); return &m_Data[0]; }
        constexpr auto  begin  () const noexcept { assert(m_Size); return &m_Data[0]; }
        constexpr auto  cbegin () const noexcept { assert(m_Size); return &m_Data[0]; }

        constexpr auto  end    ()       noexcept { assert(m_Size); return &m_Data[m_Size]; }
        constexpr auto  end    () const noexcept { assert(m_Size); return &m_Data[m_Size]; }
        constexpr auto  cend   () const noexcept { assert(m_Size); return &m_Data[m_Size]; }

        constexpr auto  rbegin ()       noexcept { assert(m_Size); return &m_Data[m_Size - 1]; }
        constexpr auto  rbegin () const noexcept { assert(m_Size); return &m_Data[m_Size - 1]; }
        constexpr auto  crbegin() const noexcept { assert(m_Size); return &m_Data[m_Size - 1]; }

        constexpr auto  rend   ()       noexcept { assert(m_Size); return &m_Data[-1]; }
        constexpr auto  rend   () const noexcept { assert(m_Size); return &m_Data[-1]; }
        constexpr auto  crend  () const noexcept { assert(m_Size); return &m_Data[-1]; }

        constexpr auto& front  ()       noexcept { assert(m_Size); return m_Data[0]; }
        constexpr auto& front  () const noexcept { assert(m_Size); return m_Data[0]; }
        constexpr auto& back   ()       noexcept { assert(m_Size); return m_Data[m_Size - 1]; }
        constexpr auto& back   () const noexcept { assert(m_Size); return m_Data[m_Size - 1]; }

        constexpr auto  data   ()       noexcept { return m_Data; }
        constexpr auto  data   () const noexcept { return m_Data; }

    public:
        constexpr auto size      () const noexcept { return m_Size; }
        constexpr auto size_bytes() const noexcept { return m_Size * sizeof(value_type); }
        constexpr auto empty     () const noexcept { return m_Size == 0; }

    public:
        explicit constexpr operator bool() const noexcept { return m_Data && m_Size; }

        constexpr auto& operator[](size_type pos)       noexcept { return m_Data[pos]; }
        constexpr auto& operator[](size_type pos) const noexcept { return m_Data[pos]; }

        constexpr auto& at(size_type pos) {
            if (pos >= size()) { throw std::out_of_range{}; }
            return m_Data[pos];
        }

        constexpr auto& at(size_type pos) const {
            if (pos >= size()) { throw std::out_of_range{}; }
            return m_Data[pos];
        }

    private:
        pointer   m_Data{ nullptr };
        size_type m_Size{ 0 };
    }; // template <...> class data_span

    //**************************************************************************
    // size/byte_size
    //**************************************************************************
    template <class ContainerT>
    inline auto size(const ContainerT& container) noexcept
        -> decltype(std::size(container)) {
        return std::size(container);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline auto byte_size(const ContainerT& container) noexcept
        -> decltype(::util::size(container)) {
        return ::util::size(container) * sizeof(ContainerT::value_type);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline auto byte_size_offset(const ContainerT& container,
                                 std::size_t offset) noexcept
        -> decltype(::util::size(container)) {
        assert(offset < ::util::size(container));
        return (::util::size(container) - offset) * sizeof(ContainerT::value_type);
    }

    //**************************************************************************
    // empty
    //**************************************************************************
    template <class ContainerT>
    inline auto empty(const ContainerT& container) noexcept
        -> decltype(std::empty(container)) {
        return std::empty(container);
    }

    //**************************************************************************
    // data
    //**************************************************************************
    template <class ContainerT>
    inline auto data(ContainerT& container)
        noexcept(noexcept(std::data(container)))
        -> decltype(std::data(container)) {
        return std::data(container);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline auto data(const ContainerT& container)
        noexcept(noexcept(std::data(container)))
        -> decltype(std::data(container)) {
        return std::data(container);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline auto data_offset(ContainerT& container,
                            std::size_t offset)
        noexcept(noexcept(::util::data(container)))
        -> decltype(std::data(container)) {
        assert((offset > 0) && offset < ::util::size(container));
        return ::util::data(container) + offset;
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline auto data_offset(const ContainerT& container,
                            std::size_t offset)
        noexcept(noexcept(::util::data(container)))
        -> decltype(std::data(container)) {
        assert(offset < ::util::size(container));
        return ::util::data(container) + offset;
    }

    //**************************************************************************
    // zero
    //**************************************************************************
    inline void zero(void* mem, std::size_t bytes) noexcept {
        std::memset(mem, 0, bytes);
    }

    //------------------------------------------------------

    template <class ValueT>
    inline void zero_n(ValueT* mem, std::size_t count) noexcept {
        ::util::zero(mem, count * sizeof(ValueT));
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void zero(ContainerT& container) noexcept {
        ::util::zero(::util::data(container),
                       ::util::byte_size(container));
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void zero_offset(ContainerT& container,
                            std::size_t offset) noexcept {
        assert(offset < ::util::size(container));
        ::util::zero(::util::data_offset(container, offset),
                       ::util::byte_size_offset(container, offset));
    }

    //------------------------------------------------------

    template <class ValueT>
    inline void zero_offset_n(ValueT* mem,
                              std::size_t count,
                              std::size_t offset) noexcept {
        assert(offset < count);
        ::util::zero_n(mem + offset, (count - offset));
    }

    //**************************************************************************
    // copy
    //**************************************************************************
    inline void copy(void* dst,
                     const void* src,
                     std::size_t bytes) noexcept {
        std::memcpy(dst, src, bytes);
    }

    //------------------------------------------------------

    template <class ValueT>
    inline void copy_n(ValueT* dst,
                       const ValueT* src,
                       std::size_t count) noexcept {
        ::util::copy(dst, src, count * sizeof(ValueT));
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void copy(ContainerT& dst,
                     const typename ContainerT::value_type* src,
                     std::size_t bytes) noexcept {
        assert(::util::byte_size(dst) >= bytes);
        ::util::copy(::util::data(dst), src, bytes);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void copy_n(ContainerT& dst,
                       const typename ContainerT::value_type* src,
                       std::size_t count) noexcept {
        assert(std::size(dst) >= count);
        ::util::copy_n(::util::data(dst), src, count);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void copy_min_n(ContainerT& dst,
                           const typename ContainerT::value_type* src,
                           std::size_t count) noexcept {
        if (::util::size(dst) > count) {
            ::util::copy_n(dst, src, count);
            ::util::zero_offset(dst, count);
        } else {
            ::util::copy_n(dst, src, ::util::size(dst));
        }
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void copy_min(ContainerT& dst,
                         const typename ContainerT::value_type* src,
                         std::size_t bytes) noexcept {
        const auto count{ bytes / sizeof(typename ContainerT::value_type) };
        ::util::copy_min_n(dst, src, count);
    }

    //------------------------------------------------------

    template <class ContainerT>
    inline void copy_min(ContainerT& dst,
                         const ContainerT& src) noexcept {
        const auto srcCount{ ::util::size(src) };
        const auto dstCount{ ::util::size(dst) };
        if (dstCount > srcCount) {
            ::util::copy_n(dst, ::util::data(src), srcCount);
            ::util::zero_offset(dst, srcCount);
        } else {
            ::util::copy_n(dst, ::util::data(src), dstCount);
        }
    }
} // namespace util

#endif // GUID_F66ACDB7_769F_479D_BCF2_D2C43909E550
