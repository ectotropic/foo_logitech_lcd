#pragma once
#ifndef GUID_19B0B5C0_E502_49D9_90AA_9525BFFE32E5
#define GUID_19B0B5C0_E502_49D9_90AA_9525BFFE32E5
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
#include <cmath>
#include "Util/TypeTraits.h"
//--------------------------------------

namespace Audio::dB {
    /**************************************************************************
     * dBFromWavedata
     * --------------
     *
     * Given wave data from an audio source, determine the "dB SPL" as used for
     * VU Meters and similar applications.
     *
     * Decibel Sound Pressure Level (dB SPL):
     *      `20 * log10(value/ref)`
     *
     *   Where:
     *      value:  Root mean square of all values
     *      ref:    "20 micro-pascals in air"
     *
     * Decibels relative to full scale (dBFS):
     *    - Defined for digital systems where an absolute
     *      peak is known ahead of time.
     *    - Defined "such that the RMS value of a full-scale
     *      sine wave is designated 0 dB FS"
     *    - "ref" is then the RMS of a sine wave
     *      which can works out to be `1/sqrt(2)`
     *
     *  REF:
     *      https://en.wikipedia.org/wiki/Decibel
     *      https://en.wikipedia.org/wiki/DBFS
     *      https://en.wikipedia.org/wiki/Root_mean_square
     **************************************************************************/
    template <
        typename SampleT,
        typename DecibelT = SampleT,
        typename SizeT    = std::size_t>
    inline auto dBFromWavedata(const SampleT* source,
                               SizeT sourceTotalCount,
                               SizeT stride) noexcept {
        using dB_t = ::util::remove_cvref_t<DecibelT>;

        if (source == nullptr || sourceTotalCount <= 0 ||
            stride <= 0 || stride > sourceTotalCount) {
            return dB_t{ 0 };
        }

        const auto sourceCount{ sourceTotalCount / stride };
        dB_t dB{ 0 };
        for (SizeT s = 0; s < sourceCount; s += stride) {
            const auto samp = static_cast<dB_t>(source[s]);
            dB += samp * samp;
        }

        if (dB != 0) {
            dB = std::sqrt(dB / static_cast<dB_t>(sourceCount));
            static const auto fReference{ std::sqrt(static_cast<dB_t>(2.)) };
            dB = std::log10(dB * fReference) * static_cast<dB_t>(20.);
        }

        return dB;
    }
} // namespace Audio::dB

#endif // GUID_19B0B5C0_E502_49D9_90AA_9525BFFE32E5