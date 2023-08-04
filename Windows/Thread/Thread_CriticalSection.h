#pragma once
#ifndef GUID_EF60A1DE_BBDE_43EB_8698_E6C5235002A6
#define GUID_EF60A1DE_BBDE_43EB_8698_E6C5235002A6
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

//--------------------------------------
//
#include <synchapi.h>
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#include <type_traits>
#include <utility>
//--------------------------------------

namespace Windows::Thread {
    //**************************************************************************
    // ScopedAquireCriticalSection
    //**************************************************************************
    class ScopedAquireCriticalSection final {
    public:
        _Acquires_lock_(*this->m_lpCriticalSection)
        _Post_same_lock_(*this->m_lpCriticalSection, *lpCriticalSection)
        ScopedAquireCriticalSection(_In_ LPCRITICAL_SECTION lpCriticalSection) noexcept :
            m_lpCriticalSection{ lpCriticalSection } {
            if (m_lpCriticalSection) {
                ::EnterCriticalSection(m_lpCriticalSection);
            }
        }

        _Releases_lock_(*m_lpCriticalSection)
        ~ScopedAquireCriticalSection() noexcept {
            Release();
        }

        ScopedAquireCriticalSection(const ScopedAquireCriticalSection& ) = delete; // No Copy
        ScopedAquireCriticalSection(      ScopedAquireCriticalSection&&) = delete; // No Move
        ScopedAquireCriticalSection& operator=(const ScopedAquireCriticalSection& ) = delete; // No Copy
        ScopedAquireCriticalSection& operator=(      ScopedAquireCriticalSection&&) = delete; // No Move

        _Releases_lock_(*m_lpCriticalSection)
        void Release() noexcept {
            gsl_suppress(26115) // C26115: Failing to release lock '...' in function '...'.
            if (IsLocked()) {
                ::LeaveCriticalSection(m_lpCriticalSection);
                m_lpCriticalSection = NULL;
            }
        }

        bool IsLocked() const throw() {
            return (m_lpCriticalSection != NULL);
        }

    private:
        LPCRITICAL_SECTION m_lpCriticalSection{ NULL };
    };

    //**************************************************************************
    // CriticalSection
    //**************************************************************************
    class CriticalSection final {
    private:
        static CRITICAL_SECTION GetEmptyCriticalSection() noexcept {
            CRITICAL_SECTION cs;
            ZeroMemory(&cs, sizeof(CRITICAL_SECTION));
            return cs;
        }

    public:
        CriticalSection() noexcept :
            m_bInitialized{true} {
            ::InitializeCriticalSection(&m_CriticalSection);
        }

        CriticalSection(_In_ DWORD dwSpinCount) noexcept :
            m_bInitialized{ true } {
            // [DOC]: "This function always succeeds and returns a non-zero value."
            gsl_suppress(6031) // C6031: Return value ignored: '...'.
            ::InitializeCriticalSectionAndSpinCount(&m_CriticalSection,
                                                    dwSpinCount);
        }

        CriticalSection(_In_ DWORD dwSpinCount,
                        _In_ DWORD dwFlags) noexcept {
            // [DOC]: "If the function succeeds, the return value is non-zero."
            const auto res = ::InitializeCriticalSectionEx(&m_CriticalSection,
                                                           dwSpinCount,
                                                           dwFlags);
            m_bInitialized = (res != 0);
            WinAPIAssert(m_bInitialized);
        }

        CriticalSection(_In_ CriticalSection&& other) noexcept :
            m_bInitialized   { std::exchange(other.m_bInitialized, false) },
            m_CriticalSection{ std::exchange(other.m_CriticalSection,
                                             GetEmptyCriticalSection()) } {}

        CriticalSection& operator=(_In_ CriticalSection&& other) noexcept {
            Destroy();
            m_bInitialized    = std::exchange(other.m_bInitialized, false);
            m_CriticalSection = std::exchange(other.m_CriticalSection,
                                              GetEmptyCriticalSection());
            return *this;
        }

        ~CriticalSection() noexcept {
            Destroy();
        }

        CriticalSection(const CriticalSection&) = delete; // No Copy
        CriticalSection& operator=(const CriticalSection&) = delete; // No Copy

    public:
        void Destroy() noexcept {
            if (std::exchange(m_bInitialized, false)) {
                ::DeleteCriticalSection(&m_CriticalSection);
                m_CriticalSection = GetEmptyCriticalSection();
            }
        }

        decltype(auto) SetSpinCount(_In_ DWORD dwSpinCount) noexcept {
            // [DOC]: "The function returns the previous spin count for the critical section."
            return ::SetCriticalSectionSpinCount(&m_CriticalSection,
                                                 dwSpinCount);
        }

        _Acquires_lock_(this->m_CriticalSection)
        void Lock() noexcept {
            assert(m_bInitialized);
            ::EnterCriticalSection(&m_CriticalSection);
        }

        _Releases_lock_(this->m_CriticalSection)
        void Unlock() noexcept {
            assert(m_bInitialized);
            ::LeaveCriticalSection(&m_CriticalSection);
        }

        _Acquires_lock_(*return.m_lpCriticalSection)
        _Post_same_lock_(*return.m_lpCriticalSection, m_CriticalSection)
        [[nodiscard]]
        decltype(auto) ScopedLock() noexcept {
            assert(m_bInitialized);
            return ::Windows::Thread::ScopedAquireCriticalSection{ &m_CriticalSection };
        }

        explicit operator bool() const noexcept { return m_bInitialized; }

    private:
        bool m_bInitialized{ false };
        CRITICAL_SECTION m_CriticalSection{ GetEmptyCriticalSection() };
    }; // class CriticalSection

    //**************************************************************************
    // CriticalSectionProtectedVariableT:
    // ----------------------------------
    //
    // Ensures direct access to a specific variable is enclosed in a lock.
    //
    // NOTE:
    //  - Storing a reference/pointer to the protected variable breaks the
    //    protection!
    //
    //**************************************************************************
    template <typename TypeT>
    class CriticalSectionProtectedVariableT final {
    private:
        using thisClass = CriticalSectionProtectedVariableT<TypeT>;

    private:
        //------------------------------
        // Helper Type Traits
        template <
            typename TypeT,
            typename EnableT = void
        >
        struct is_exchangable :
            public std::false_type {};

        template <typename TypeT>
        struct is_exchangable <
            TypeT,
            std::void_t<
                decltype(std::exchange(std::declval<TypeT&>(),
                                       std::declval<TypeT&&>()))
            >
        > : public std::true_type {};

        template <typename TypeT>
        static constexpr bool is_exchangable_v =
            is_exchangable<TypeT>::value;
        //------------------------------

    public:
        using valueType = TypeT;

    public: // Construction [conditional]
        //------------------------------
        // Default Construction
        template<
            bool EnableT = std::is_default_constructible_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        CriticalSectionProtectedVariableT()
            noexcept(noexcept(std::is_nothrow_default_constructible_v<valueType>)) :
            m_Variable{ } {}
        //------------------------------

        //------------------------------
        // Value Construction
        template<
            bool EnableT = std::is_move_constructible_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        CriticalSectionProtectedVariableT(_In_ valueType&& value)
            noexcept(noexcept(std::is_nothrow_move_constructible_v<valueType>)) :
            m_Variable{ std::forward<valueType>(value) } {}
        //------------------------------

        //------------------------------
        // Move Construction
        template<
            bool EnableT = std::is_move_constructible_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        CriticalSectionProtectedVariableT(_In_ CriticalSectionProtectedVariableT&& other) noexcept :
            m_Variable       { std::move(other.m_Variable) },
            m_CriticalSection{ std::move(other.m_CriticalSection) } {}
        //------------------------------

        //------------------------------
        // No copy operations
        CriticalSectionProtectedVariableT(const CriticalSectionProtectedVariableT&) = delete; // No Copy
        //------------------------------

    public: // Operators [conditional]
        //------------------------------
        template<
            bool EnableT = std::is_move_assignable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        CriticalSectionProtectedVariableT& operator=(_In_ CriticalSectionProtectedVariableT&& other) noexcept {
            m_Variable        = std::move(other.m_Variable);
            m_CriticalSection = std::move(other.m_CriticalSection);
            return *this;
        }
        //------------------------------

        //------------------------------
        // No copy operations
        CriticalSectionProtectedVariableT& operator=(const CriticalSectionProtectedVariableT& ) = delete; // No Copy
        //------------------------------

    public:
        //------------------------------
        // Get
        [[nodiscard]]
        auto Get() const noexcept {
            const auto scopedLock{ m_CriticalSection.ScopedLock() };
            return m_Variable;
        }
        //------------------------------

        //------------------------------
        // Set
        template<
            bool EnableT = std::is_copy_assignable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        void Set(const valueType& value)
            noexcept(noexcept(std::is_nothrow_copy_assignable_v<valueType>)) {
            const auto scopedLock{ m_CriticalSection.ScopedLock() };
            m_Variable = value;
        }

        template<
            bool EnableT = std::is_move_assignable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        void Set(valueType&& value)
            noexcept(noexcept(std::is_nothrow_move_assignable_v<valueType>)) {
            const auto scopedLock{ m_CriticalSection.ScopedLock() };
            m_Variable = std::forward<valueType>(value);
        }
        //------------------------------

        //------------------------------
        // Exchange
        template<
            bool EnableT = is_exchangable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        auto Exchange(valueType&& value)
            noexcept(noexcept(std::exchange(m_Variable, value))) {
            const auto scopedLock{ m_CriticalSection.ScopedLock() };
            return std::exchange(m_Variable,
                                 std::forward<valueType>(value));
        }
        //------------------------------

        //------------------------------
        // swap
        template<
            bool EnableT = std::is_swappable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        void swap(CriticalSectionProtectedVariableT& other)
            noexcept(noexcept(std::is_nothrow_swappable_v<valueType>)) {
            using std::swap;
            swap(m_Variable,        other.m_Variable);
            swap(m_CriticalSection, other.m_CriticalSection);
        }

        template<
            bool EnableT = std::is_swappable_v<valueType>,
            std::enable_if_t<EnableT, bool> = true
        >
        void swap(valueType& other)
            noexcept(noexcept(std::is_nothrow_swappable_v<valueType>)) {
            using std::swap;
            swap(m_Variable, other.m_Variable);
        }
        //------------------------------

    private:
        valueType m_Variable;
        mutable CriticalSection m_CriticalSection{};
    }; // template<...> class CriticalSectionProtectedVariableT final
} // namespace Windows::Thread

#endif // GUID_EF60A1DE_BBDE_43EB_8698_E6C5235002A6