#pragma once
#ifndef GUID_4D977C8C_C3A9_4C01_A2B4_647D126835F4
#define GUID_4D977C8C_C3A9_4C01_A2B4_647D126835F4
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
#include <memory>
#include <utility>
#include <stdexcept>
#include <type_traits>
//--------------------------------------

namespace util {
    //**************************************************************************
    // sequential_enum: iterable enum which also supports named values
    //**************************************************************************
    template <
        class TypeT,
        class PointerT = std::shared_ptr<TypeT>
    >
    class singleton : public std::enable_shared_from_this<TypeT> {
    private:
        using this_type = singleton<TypeT, PointerT>;

    public:
        using singleton_type = TypeT;
        using pointer_type   = PointerT;

    private:
        template <typename TagT>
        struct _singleton_constructor_tag {};

    protected:
        // std::make_shared can't access non-public constructors, so instead
        // use a protected type as an argument for a public constructor
        using singleton_constructor_tag =
            _singleton_constructor_tag<singleton_type>;

    private: // Traits helpers
        //------------------------------
        // can_construct
        template <typename OtherT>
        using enable_if_can_construct =
            std::enable_if<std::is_constructible_v<OtherT, singleton_constructor_tag>, bool>;

        template <typename OtherT>
        using enable_if_can_construct_t =
            typename enable_if_can_construct<OtherT>::type;

        //------------------------------
        // can_not_construct
        template <typename OtherT>
        using enable_if_can_not_construct =
            std::enable_if<std::negation_v<std::is_constructible<OtherT, singleton_constructor_tag>>, bool>;

        template <typename OtherT>
        using enable_if_can_not_construct_t =
            typename enable_if_can_not_construct<OtherT>::type;

        //------------------------------
        // can_copy
        template <typename OtherT>
        using enable_if_can_copy =
            std::enable_if<std::is_copy_constructible_v<OtherT>, bool>;

        template <typename OtherT>
        using enable_if_can_copy_t =
            typename enable_if_can_copy<OtherT>::type;

        //------------------------------
        // can_not_copy
        template <typename OtherT>
        using enable_if_can_not_copy =
            std::enable_if<std::negation_v<std::is_copy_constructible<OtherT>>, bool>;

        template <typename OtherT>
        using enable_if_can_not_copy_t =
            typename enable_if_can_not_copy<OtherT>::type;

    public:
        //------------------------------
        // expired
        static auto expired() noexcept {
            return !static_cast<bool>(s_pSingleton);
        }
        //------------------------------

        //------------------------------
        // instance_peek
        template <
            typename DummyT = pointer_type,
            enable_if_can_copy_t<DummyT> = true
        >
        static auto instance_peek() noexcept {
            // NOTE: always returns a copy to avoid
            // the caller ever being able to release
            // or reset the singleton
            return pointer_type{ s_pSingleton };
        }

        template <
            typename DummyT = pointer_type,
            enable_if_can_not_copy_t<DummyT> = true
        >
        static auto instance_peek() noexcept {
            return s_pSingleton ? &(*s_pSingleton) : nullptr;
        }
        //------------------------------

        //------------------------------
        // instance
        template <
            typename DummyT = singleton_type,
            enable_if_can_construct_t<DummyT> = true
        >
        static decltype(auto) instance() {
            if (!s_pSingleton) { this_type::initialise<singleton_type>(); }
            return instance_peek();
        }

        template <
            typename DummyT = singleton_type,
            enable_if_can_not_construct_t<DummyT> = true
        >
        static decltype(auto) instance() {
            if (!s_pSingleton) {
                throw std::logic_error{ "Singleton not yet initialised" };
            }
            return instance_peek();
        }
        //------------------------------

        //------------------------------
        // destroy
        static void destroy() noexcept {
            s_pSingleton.reset();
        }
        //------------------------------

    protected:
        template <class OtherT, class... ArgsT>
        static void initialise(ArgsT&&... args) {
            s_pSingleton = std::make_shared<OtherT>(singleton_constructor_tag{},
                                                    std::forward<ArgsT>(args)...);
        }

        template <class OtherT, class... ArgsT>
        static void reset(std::shared_ptr<OtherT>&& pSingleton) {
            s_pSingleton = std::forward<pointer_type>(pSingleton);
        }

    protected:
        singleton() noexcept = default;

    private:
        inline static pointer_type s_pSingleton{};
    }; // template <...> class singleton
} // namespace util

#endif // GUID_4D977C8C_C3A9_4C01_A2B4_647D126835F4
