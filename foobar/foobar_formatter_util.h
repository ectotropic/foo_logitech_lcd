#pragma once
#ifndef GUID_91498136_B106_4614_8D67_6F276A56FD5F
#define GUID_91498136_B106_4614_8D67_6F276A56FD5F
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
#include <cassert>
//--------------------------------------

namespace foobar::metadata {
    //**************************************************************************
    // Metadata Helpers
    //**************************************************************************
    inline auto compile_script(const char* p_spec) {
        ::titleformat_object_ptr script{};
        ::titleformat_compiler::get()->compile_safe(script, p_spec);
        return script;
    }

    //------------------------------------------------------

    inline decltype(auto) compile_script(::pfc::string8 p_spec) {
        return compile_script(p_spec.get_ptr());
    }

    //------------------------------------------------------

    inline decltype(auto) compile_script(std::string p_spec) {
        return compile_script(p_spec.c_str());
    }

    //------------------------------------------------------

    inline decltype(auto) compile_script(std::wstring p_spec) {
        const ::pfc::stringcvt::string_utf8_from_wide spec{ p_spec.c_str(), p_spec.size() };
        return compile_script(spec.get_ptr());
    }

    //------------------------------------------------------

    inline auto get_metadata(::titleformat_object_ptr   p_script,
                             ::metadb_handle_ptr        p_metadb,
                             const ::file_info*         p_info   = nullptr,
                             ::titleformat_hook*        p_hook   = nullptr,
                             ::titleformat_text_filter* p_filter = nullptr) {
        ::pfc::string8 utf8Metadata{};
        if (p_info) {
            p_metadb->format_title_from_external_info(*p_info, p_hook,
                                                      utf8Metadata, p_script,
                                                      p_filter);
        } else {
            p_metadb->format_title(p_hook, utf8Metadata, p_script, p_filter);
        }
        return utf8Metadata;
    }

    //------------------------------------------------------

    inline decltype(auto) get_metadata(::pfc::string8             p_spec,
                                       ::metadb_handle_ptr        p_metadb,
                                       const ::file_info*         p_info   = nullptr,
                                       ::titleformat_hook*        p_hook   = nullptr,
                                       ::titleformat_text_filter* p_filter = nullptr) {
        return get_metadata(compile_script(p_spec), p_metadb,
                            p_info, p_hook, p_filter);
    }

    //------------------------------------------------------

    template <typename CharT, class TraitsT, class AllocatorT>
    inline decltype(auto) get_metadata(std::basic_string<CharT, TraitsT, AllocatorT> p_spec,
                                       ::metadb_handle_ptr                           p_metadb,
                                       const ::file_info*                            p_info   = nullptr,
                                       ::titleformat_hook*                           p_hook   = nullptr,
                                       ::titleformat_text_filter*                    p_filter = nullptr) {
        return get_metadata(compile_script(p_spec), p_metadb,
                            p_info, p_hook, p_filter);
    }

    //**************************************************************************
    // title_formatter
    //**************************************************************************
    class title_formatter final {
    public:
        using formatted_type     = ::pfc::string8;

        using script_source_type = ::pfc::string8;

        using script_type        = ::titleformat_object;
        using script_ptr_type    = typename script_type::ptr;
        using script_array_type  = ::pfc::array_t<::titleformat_object_ptr>;
        using script_hook_type   = ::titleformat_hook;
        using script_filter_type = ::titleformat_text_filter;

        using static_metadata_type      = ::metadb_handle;
        using static_metadata_ptr_type  = typename static_metadata_type::ptr;
        using dynamic_metadata_type     = ::file_info;
        using dynamic_metadata_ptr_type = const dynamic_metadata_type*;

    public:
        title_formatter() noexcept = default;

        title_formatter(const script_array_type& p_scripts) :
            m_scripts{ p_scripts } {}

        title_formatter(script_array_type&& p_scripts) :
            m_scripts{ std::forward<script_array_type>(p_scripts) } {}

        template <typename ContainerT>
        title_formatter(const ContainerT& p_scripts) {
            assign(p_scripts);
        }

        void assign(const script_array_type& p_scripts) {
            m_scripts = p_scripts;
        }

        void assign(script_array_type&& p_scripts) {
            m_scripts = std::forward<script_array_type>(p_scripts);
        }

        template <typename ContainerT>
        void assign(const ContainerT& p_scripts) {
            m_scripts.set_size(0);
            m_scripts.prealloc(p_scripts.size());
            for (const auto& spec : p_scripts) {
                auto script{ ::foobar::metadata::compile_script(spec) };
                m_scripts.add_item(std::move(script));
            }
        }

        template <typename ContainerT>
        void format(ContainerT&               p_out,
                    static_metadata_ptr_type  p_metadb,
                    dynamic_metadata_ptr_type p_info   = nullptr,
                    script_hook_type*         p_hook   = nullptr,
                    script_filter_type*       p_filter = nullptr) {
            p_out.resize(m_scripts.size());
            auto it_out{ std::begin(p_out) };
            if (p_info) {
                for (const auto& script : m_scripts) {
                    assert(it_out != std::end(p_out));
                    p_metadb->format_title_from_external_info(*p_info, p_hook,
                                                              (*it_out), script,
                                                              p_filter);
                    ++it_out;
                }
            } else {
                for (const auto& script : m_scripts) {
                    assert(it_out != std::end(p_out));
                    p_metadb->format_title(p_hook, (*it_out),
                                           script, p_filter);
                    ++it_out;
                }
            }
        }

    private:
        script_array_type m_scripts{};
    }; // class title_formatter final
} // namespace foobar::metadata

#endif // GUID_91498136_B106_4614_8D67_6F276A56FD5F
