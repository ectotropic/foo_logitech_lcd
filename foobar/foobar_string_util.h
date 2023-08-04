#pragma once
#ifndef GUID_B9E3D176_47C0_4F6F_A743_0731FB8A8B96
#define GUID_B9E3D176_47C0_4F6F_A743_0731FB8A8B96
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
#include <string>
//--------------------------------------

namespace foobar {
    //**************************************************************************
    // String Helpers
    //**************************************************************************
    inline [[nodiscard]]
    auto to_utf8(const wchar_t* src) {
        using converter_type =
            pfc::stringcvt::string_utf8_from_wide;
        return converter_type{ src };
    }

    //------------------------------------------------------

    template <class SizeT = std::size_t>
    inline [[nodiscard]]
    auto to_utf8(const wchar_t* src, SizeT len) {
        using converter_type =
            pfc::stringcvt::string_utf8_from_wide;
        return converter_type{ src, static_cast<t_size>(len) };
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) to_utf8(std::wstring src) {
        return foobar::to_utf8(src.c_str(), src.size());
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline [[nodiscard]]
    auto to_wide(const char* src) {
        using converter_type =
            pfc::stringcvt::string_wide_from_utf8;
        return converter_type{ src };
    }

    //------------------------------------------------------

    template <class SizeT = t_size>
    inline [[nodiscard]]
    auto to_wide(const char* src, SizeT len) {
        using converter_type =
            pfc::stringcvt::string_wide_from_utf8;
        return converter_type{ src, len };
    }

    //------------------------------------------------------

    inline [[nodiscard]]
    decltype(auto) to_wide(pfc::string8 src) {
        return foobar::to_wide(src.c_str(),
                               src.get_length());
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline void convert_string(pfc::string8& dst,
                               const pfc::string8& src) {
        dst = src;
    }

    //------------------------------------------------------

    inline void convert_string(std::string& dst,
                               const std::string& src) {
        dst = src;
    }

    //------------------------------------------------------

    inline void convert_string(std::wstring& dst,
                               const std::wstring& src) {
        dst = src;
    }

    //------------------------------------------------------

    inline void convert_string(pfc::string8& dst,
                               const std::string& src) {
        pfc::string8 converted{ src.c_str(), src.size() };
        using std::swap;
        swap(dst, converted);
    }

    //------------------------------------------------------

    inline void convert_string(std::string& dst,
                               const pfc::string8& src) {
        std::string converted{
            src.c_str(), src.get_length()
        };
        using std::swap;
        swap(dst, converted);
    }

    //------------------------------------------------------

    inline void convert_string(pfc::string8& dst,
                               const std::wstring& src) {
        const auto convertedBuf{ foobar::to_utf8(src) };
        pfc::string8 converted{ convertedBuf.get_ptr(),
                                convertedBuf.length() };
        using std::swap;
        swap(dst, converted);
    }

    //------------------------------------------------------

    inline void convert_string(std::wstring& dst, const pfc::string8& src) {
        const auto convertedBuf{ foobar::to_wide(src) };
        std::wstring converted{
            convertedBuf.get_ptr(), convertedBuf.length()
        };
        using std::swap;
        swap(dst, converted);
    }

    //------------------------------------------------------

    template <class TargetT, class SourceT>
    inline [[nodiscard]]
    auto convert_string(const SourceT& src) {
        TargetT dst{};
        foobar::convert_string(dst, src);
        return dst;
    }
} // namespace foobar

#endif // GUID_B9E3D176_47C0_4F6F_A743_0731FB8A8B96
