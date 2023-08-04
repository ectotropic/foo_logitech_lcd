#pragma once
#ifndef GUID_6C78CCAA_008D_4FC8_9D8E_198AA26ACBE9
#define GUID_6C78CCAA_008D_4FC8_9D8E_198AA26ACBE9
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
#include "foobar/foobar_core_util.h"
//--------------------------------------

//--------------------------------------
//
#include <type_traits>
//--------------------------------------

namespace foobar::visualisation {
    //**************************************************************************
    // Visualisation Types
    //**************************************************************************

    using manager_t     = ::visualisation_manager;
    using manager_ptr_t = typename foobar::visualisation::manager_t::ptr;

    using stream_v1_t     = ::visualisation_stream;
    using stream_v1_ptr_t = typename foobar::visualisation::stream_v1_t::ptr;
    using stream_v2_t     = ::visualisation_stream_v2;
    using stream_v2_ptr_t = typename foobar::visualisation::stream_v2_t::ptr;
    using stream_v3_t     = ::visualisation_stream_v3;
    using stream_v3_ptr_t = typename foobar::visualisation::stream_v3_t::ptr;

    using stream_t     = typename foobar::visualisation::stream_v1_t;
    using stream_ptr_t = typename foobar::visualisation::stream_t::ptr;

    using data_t = ::audio_chunk_impl;

    //**************************************************************************
    // Visualisation Helpers
    //**************************************************************************
    template <
        typename StreamT,
        std::enable_if_t<
            std::is_base_of_v<foobar::visualisation::stream_v1_t, StreamT>,
            bool
        > = true
    >
    inline bool create_stream(::service_ptr_t<StreamT>& p_stream,
                              unsigned p_flags = foobar::visualisation::manager_t::KStreamFlagNewFFT) {
        p_stream.reset();
        stream_v1_ptr_t p_stream_v1{};
        const auto p_mgr{ foobar::visualisation::manager_t::get() };
        assert(p_mgr.is_valid()); if (!p_mgr.is_valid()) { return false; }
        p_mgr->create_stream(p_stream_v1, p_flags);
        assert(p_stream_v1.is_valid());
        if (p_stream_v1.is_valid()) {
            p_stream_v1->service_query_t(p_stream);
        }
        return p_stream.is_valid();
    }

    //------------------------------------------------------

    template <
        typename StreamT,
        std::enable_if_t<
            std::is_base_of_v<foobar::visualisation::stream_v1_t, StreamT>,
            bool
        > = true
    >
    inline auto create_stream(unsigned p_flags = foobar::visualisation::manager_t::KStreamFlagNewFFT) {
        service_ptr_t<StreamT> p_stream_v1{};
        if (!foobar::visualisation::create_stream(p_stream_v1, p_flags)) {
            p_stream_v1.reset();
        }
        return p_stream_v1;
    }

    //------------------------------------------------------

    inline decltype(auto) create_v1_stream(unsigned p_flags = foobar::visualisation::manager_t::KStreamFlagNewFFT) {
        return foobar::visualisation::create_stream<foobar::visualisation::stream_v1_t>(p_flags);
    }

    //------------------------------------------------------

    inline decltype(auto) create_v2_stream(unsigned p_flags = foobar::visualisation::manager_t::KStreamFlagNewFFT) {
        return foobar::visualisation::create_stream<foobar::visualisation::stream_v2_t>(p_flags);
    }

    //------------------------------------------------------

    inline decltype(auto) create_v3_stream(unsigned p_flags = foobar::visualisation::manager_t::KStreamFlagNewFFT) {
        return foobar::visualisation::create_stream<foobar::visualisation::stream_v3_t>(p_flags);
    }

    //------------------------------------------------------

    inline auto get_audio_data(foobar::visualisation::stream_ptr_t p_stream,
                               double p_offset,
                               double p_requested_length) {
        if (!p_stream.is_valid()) { return audio_chunk_impl{}; }
        ::audio_chunk_impl chunk{};
        if (!p_stream->get_chunk_absolute(chunk, p_offset,
                                          p_requested_length)) {
            chunk.reset();
        }
        return chunk;
    }

    //------------------------------------------------------

    inline auto get_audio_data(foobar::visualisation::stream_ptr_t p_stream,
                               double p_requested_length) {
        double absolute_time{ 0 };
        if (p_stream->get_absolute_time(absolute_time)) {
            return get_audio_data(p_stream, absolute_time,
                                  p_requested_length);
        } else {
            return foobar::visualisation::data_t{};
        }
    }

    //------------------------------------------------------

    template<typename ContainerT>
    inline auto get_audio_data(ContainerT& p_out,
                               stream_ptr_t p_stream,
                               double p_offset,
                               double p_requested_length) {
        const auto chunk{
            foobar::visualisation::get_audio_data(p_stream, p_offset,
                                                  p_requested_length)
        };
        if (chunk.is_valid()) {
            p_out.resize(chunk.get_data_size());
            std::memcpy(std::data(p_out),
                        chunk.get_data(),
                        chunk.get_data_size());
            return chunk.get_channel_count();
        }
        return 0;
    }

    //------------------------------------------------------

    template<typename ContainerT>
    inline auto get_audio_data(ContainerT& p_out,
                               stream_ptr_t p_stream,
                               double p_requested_length) {
        double absolute_time{ 0 };
        if (p_stream->get_absolute_time(absolute_time)) {
            return foobar::visualisation::get_audio_data(p_out, p_stream,
                                                         absolute_time,
                                                         p_requested_length);
        } else {
            return 0;
        }
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline auto get_spectrum_data(stream_ptr_t p_stream,
                                  double p_offset,
                                  unsigned p_fft_size) {
        if (!p_stream.is_valid()) { return audio_chunk_impl{}; }
        audio_chunk_impl chunk{};
        if (!p_stream->get_spectrum_absolute(chunk, p_offset, p_fft_size)) {
            chunk.reset();
        }
        return chunk;
    }

    //------------------------------------------------------

    inline auto get_spectrum_data(stream_ptr_t p_stream,
                                  unsigned p_fft_size) {
        double absolute_time{ 0 };
        if (p_stream->get_absolute_time(absolute_time)) {
            return foobar::visualisation::get_spectrum_data(p_stream,
                                                            absolute_time,
                                                            p_fft_size);
        } else {
            return foobar::visualisation::data_t{};
        }
    }

    //------------------------------------------------------

    template<typename ContainerT>
    inline auto get_spectrum_data(ContainerT& p_out,
                                  stream_ptr_t p_stream,
                                  double p_offset,
                                  unsigned p_fft_size) {
        const auto chunk{
            foobar::visualisation::get_spectrum_data(p_stream,
                                                     p_offset,
                                                     p_fft_size)
        };
        if (chunk.is_valid()) {
            p_out.resize(chunk.get_data_size());
            std::memcpy(std::data(p_out),
                        chunk.get_data(),
                        chunk.get_data_size());
            return chunk.get_channel_count();
        }
        return 0;
    }

    //------------------------------------------------------

    template<typename ContainerT>
    inline auto get_spectrum_data(ContainerT& p_out,
                                  stream_ptr_t p_stream,
                                  unsigned p_fft_size) {
        double absolute_time{ 0 };
        if (p_stream->get_absolute_time(absolute_time)) {
            return foobar::visualisation::get_spectrum_data(p_out,
                                                            p_stream,
                                                            absolute_time,
                                                            p_fft_size);
        } else {
            return 0;
        }
    }
} // namespace foobar::visualisation

#endif // GUID_6C78CCAA_008D_4FC8_9D8E_198AA26ACBE9
