#pragma once
#ifndef GUID_56BF1566_11E4_4706_9AAE_EEBD02331B81
#define GUID_56BF1566_11E4_4706_9AAE_EEBD02331B81
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

namespace foobar::io {
    //**************************************************************************
    // IO Types
    //**************************************************************************
    using data_ptr_t = typename fb2k::memBlockRef;

    //**************************************************************************
    // IO Helpers
    //**************************************************************************
    inline decltype(auto) path_exists(const char* p_path,
                                      foobar::abort_callback_param p_abort = {}) {
        return foobar2000_io::filesystem::g_exists(p_path, p_abort);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline decltype(auto) file_open(const char* p_path) {
        return foobar2000_io::filesystem::tryGet(p_path);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline auto path_stats(foobar2000_io::filesystem::ptr p_filesystem,
                           const char* p_path,
                           foobar::abort_callback_param p_abort = {})
        -> decltype(std::declval<foobar2000_io::filesystem>().get_stats(p_path, p_abort)) {
        if (!p_filesystem.is_valid()) { return {}; }
        return p_filesystem->get_stats(p_path, p_abort);
    }

    //------------------------------------------------------

    inline auto path_stats(const char* p_path)
        -> decltype(path_stats(std::declval<foobar2000_io::filesystem::ptr>(), p_path)) {
        auto file{ file_open(p_path) };
        if (!file.is_valid()) { return {}; }
        return path_stats(file, p_path);
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline auto file_read(const char* p_path,
                          foobar::abort_callback_param p_abort = {})
        -> decltype(foobar2000_io::filesystem::g_readWholeFile(p_path,
                                                               std::declval<size_t>(),
                                                               p_abort)) {
        auto file{ file_open(p_path) };
        if (!file.is_valid()) { return {}; }
        const auto stats{ path_stats(file, p_path) };
        if (stats.m_size == filesize_invalid) { return {}; }
        return foobar2000_io::filesystem::g_readWholeFile(p_path,
                                                          static_cast<size_t>(stats.m_size),
                                                          p_abort);
    }
} // namespace foobar::io

#endif // GUID_56BF1566_11E4_4706_9AAE_EEBD02331B81
