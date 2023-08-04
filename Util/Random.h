#pragma once
#ifndef GUID_B5401D2F_71A6_4C36_A26C_C0119C3913A2
#define GUID_B5401D2F_71A6_4C36_A26C_C0119C3913A2
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
#include <random>
#include <limits>
//--------------------------------------

namespace util {
    //**************************************************************************
    // random_number_generator
    //**************************************************************************
    template <
        typename ValueTypeT,
        class DistributionT,
        class GeneratorT    = std::mt19937,
        class DeviceT       = std::random_device
    >
    class random_number_generator final {
    public:
        using value_type        = ValueTypeT;
        using device_type       = DeviceT;
        using generator_type    = GeneratorT;
        using distribution_type = DistributionT;
        using result_type       = typename distribution_type::result_type;
        using param_type        = typename distribution_type::param_type;
        using limits_type       = std::numeric_limits<result_type>;

    public:
        random_number_generator() noexcept = default;

        random_number_generator(const random_number_generator& ) noexcept = default;
        random_number_generator(      random_number_generator&&) noexcept = default;

        random_number_generator& operator=(const random_number_generator& ) noexcept = default;
        random_number_generator& operator=(      random_number_generator&&) noexcept = default;

        explicit random_number_generator(result_type a,
                                         result_type b = limits_type::max()) :
            m_Distribution{ a,b } {}

        explicit random_number_generator(const param_type& params) :
            m_Distribution{ params } {}

        decltype(auto) get() {
            return m_Distribution(m_Generator);
        }

        decltype(auto) get(const param_type& params) {
            return m_Distribution(m_Generator, params);
        }

        decltype(auto) get(result_type a,
                           result_type b = limits_type::max()) {
            return get(param_type{ a, b });
        }

    private:
        inline static device_type m_Device{};

    private:
        generator_type    m_Generator   { m_Device() };
        distribution_type m_Distribution{};
    }; // template <...> class random_number_generator final
} // namespace util

#endif // GUID_B5401D2F_71A6_4C36_A26C_C0119C3913A2
