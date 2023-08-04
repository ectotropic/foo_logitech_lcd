#pragma once
#ifndef GUID_D6809C04_1AAE_4B8D_B96B_CE9E4585B74D
#define GUID_D6809C04_1AAE_4B8D_B96B_CE9E4585B74D
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
#include <array>
#include <cmath>
//--------------------------------------

namespace util {
    //**************************************************************************
    // trig_cache
    //**************************************************************************
    template <
        typename FloatT,
        std::size_t CacheEntriesT
    >
    class trig_cache final {
    public:
        using float_type = FloatT;

        struct sincos_type {
            constexpr auto sin() const noexcept { return sine;   }
            constexpr auto cos() const noexcept { return cosine; }

            float_type sine  { 0 };
            float_type cosine{ 0 };
        };

        using cache_type = std::array<sincos_type, CacheEntriesT>;
        using size_type  = typename cache_type::size_type;

    public:
        static constexpr const size_type cache_count{ CacheEntriesT };

        static constexpr const float_type         pi{ static_cast<float_type>(3.1415926535897932384626433832795) };
        static constexpr const float_type     two_pi{ static_cast<float_type>(2.) * pi };
        static constexpr const float_type    deg2rad{ pi / static_cast<float_type>(180.) };
        static constexpr const float_type    rad2deg{ static_cast<float_type>(180.) / pi };
        static constexpr const float_type theta_diff{ two_pi / static_cast<float_type>(cache_count) };

    public:
        trig_cache() noexcept {
            float_type fTheta = 0;
            for (auto& c : m_Cache) {
                c = { std::sin(fTheta), std::cos(fTheta) };
                fTheta += theta_diff;
            }
        }

    public:
        constexpr decltype(auto) sincos(size_type i) const noexcept {
            assert(i < cache_count);
            return m_Cache[i];
        }

        constexpr sincos_type sincos(size_type index,
                                     size_type count) const noexcept {
            if (count == cache_count) {
                return sincos(index);
            } else {
                const auto fCacheCount = static_cast<float_type>(cache_count);
                const auto fCount      = static_cast<float_type>(count);
                const auto fIndex      = static_cast<float_type>(index);
                const auto fCacheIndex = fCacheCount * (fIndex / fCount);
                const auto fLowerIndex = std::floor(fCacheIndex);
                const auto fFract      = fCacheIndex - fLowerIndex;
                const auto lowerIndex  = static_cast<size_type>(fLowerIndex);
                      auto upperIndex  = lowerIndex + 1;

                if (upperIndex >= cache_count) {
                    upperIndex = 0;
                }

                const auto lower = sincos(lowerIndex);
                const auto upper = sincos(upperIndex);
                return sincos_type{
                    lower.sine   + (upper.sine   - lower.sine)   * fFract,
                    lower.cosine + (upper.cosine - lower.cosine) * fFract,
                };
            }
        }

        constexpr auto sin(size_type i) const noexcept { return sincos(i).sine; }
        constexpr auto cos(size_type i) const noexcept { return sincos(i).cosine; }

        constexpr auto sin(size_type i, size_type c) const noexcept { return sincos(i, c).sine; }
        constexpr auto cos(size_type i, size_type c) const noexcept { return sincos(i, c).cosine; }

    private:
        cache_type m_Cache;
    }; // template <...> class trig_cache final
} // namespace util

#endif // GUID_D6809C04_1AAE_4B8D_B96B_CE9E4585B74D
