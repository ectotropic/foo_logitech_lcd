#pragma once
#ifndef GUID_F7073DC8_E88E_4097_9002_BBF9B5B2BDB8
#define GUID_F7073DC8_E88E_4097_9002_BBF9B5B2BDB8
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
#include <helpers/image_load_save.h>
//--------------------------------------

//--------------------------------------
//
#include "Util/TypeTraits.h"
#include <cassert>
//--------------------------------------

namespace foobar::metadata::album_art {
    //**************************************************************************
    // Album Art Types
    //**************************************************************************
    using manager_t     = album_art_manager_v2;
    using manager_ptr_t = typename album_art::manager_t::ptr;

    using extractor_v1_t     = album_art_extractor_instance;
    using extractor_v1_ptr_t = typename album_art::extractor_v1_t::ptr;
    using extractor_v2_t     = album_art_extractor_instance_v2;
    using extractor_v2_ptr_t = typename album_art::extractor_v2_t::ptr;

    using extractor_t     = typename album_art::extractor_v2_t;
    using extractor_ptr_t = typename album_art::extractor_v2_ptr_t;

    using data_t     = album_art_data;
    using data_ptr_t = typename album_art::data_t::ptr;

    using path_list_t     = album_art_path_list;
    using path_list_ptr_t = typename album_art::path_list_t::ptr;

    //**************************************************************************
    // id_helper
    //**************************************************************************
    struct id_helper final {
    public:
        using id_type      = ::util::remove_cvref_t<decltype(::album_art_ids::cover_front)>;
        using id_list_type = pfc::list_t<id_type>;

    public:
        inline static constexpr const auto& cover_front{
            ::album_art_ids::cover_front
        };

        inline static constexpr const auto& cover_back{
            ::album_art_ids::cover_back
        };

        inline static constexpr const auto& disc{
            ::album_art_ids::disc
        };

        inline static constexpr const auto& icon{
            ::album_art_ids::icon
        };

        inline static constexpr const auto& artist{
            ::album_art_ids::artist
        };

        static auto all() {
            static auto s_all{
                ::foobar::make_list(cover_front, cover_back,
                                    disc, icon, artist) };
            return s_all;
        }
    }; // struct id_helper final

    //------------------------------------------------------
    //------------------------------------------------------

    using id_t      = typename album_art::id_helper::id_type;
    using id_list_t = typename album_art::id_helper::id_list_type;

    //------------------------------------------------------
    //------------------------------------------------------

    inline auto get_extractor(const album_art::manager_ptr_t& p_manager,
                              const ::metadb_handle::ptr p_metadb,
                              const album_art::id_list_t& ids,
                              foobar::abort_callback_param p_abort = {})
        -> decltype(std::declval<album_art::manager_t>().open(std::declval<metadb_handle_list>(),
                                                              ids, p_abort)) {
        if (!p_manager.is_valid()) { return {}; }
        metadb_handle_list db_list{};
        db_list.add_item(p_metadb);
        return p_manager->open(db_list, ids, p_abort);
    }

    //------------------------------------------------------

    inline decltype(auto) get_extractor(const ::metadb_handle::ptr p_metadb,
                                        const album_art::id_list_t& ids,
                                        foobar::abort_callback_param p_abort = {}) {
        return get_extractor(album_art::manager_t::get(),
                             p_metadb, ids, std::move(p_abort));
    }

    //------------------------------------------------------
    //------------------------------------------------------

    inline data_ptr_t query_single_embeded(const album_art::extractor_v1_ptr_t& p_extractor,
                                           const album_art::id_list_t& p_type_ids,
                                           foobar::abort_callback_param p_abort = {}) {
        if (!p_extractor.is_valid()) { return {}; }

        data_ptr_t p_art{};
        for (auto& id : p_type_ids) {
            if (p_extractor->have_entry(id, std::move(p_abort)) &&
                p_extractor->query(id, p_art, std::move(p_abort))) {
                break;
            }
        }
        return p_art;
    }

    //------------------------------------------------------

    inline auto query_single_embeded(const album_art::manager_ptr_t& p_manager,
                                     const ::metadb_handle::ptr p_metadb,
                                     const album_art::id_list_t& p_type_ids,
                                     foobar::abort_callback_param p_abort = {})
        -> decltype(query_single_embeded(std::declval<extractor_ptr_t>(), p_type_ids)) {
        const auto p_extractor{ get_extractor(p_manager, p_metadb, p_type_ids) };
        if (!p_extractor.is_valid()) { return {}; }
        return query_single_embeded(p_extractor, p_type_ids, std::move(p_abort));
    }

    //------------------------------------------------------

    inline auto query_single_external(const album_art::extractor_v2_ptr_t& p_extractor_v2,
                                      const album_art::id_list_t& p_type_ids,
                                      foobar::abort_callback_param p_abort = {})
        -> decltype(::fb2k::readPictureFile(std::declval<const char*>(), p_abort)) {
        for (auto& id : p_type_ids) {
            try {
                auto p_paths{ p_extractor_v2->query_paths(id, std::move(p_abort)) };
                if (p_paths.is_empty()) { continue; }
                for (size_t p = 0; p < p_paths->get_count(); ++p) {
                    const auto* sz_path{ p_paths->get_path(p) };
                    if (!sz_path) { continue; }
                    auto p_data{ ::fb2k::readPictureFile(sz_path, std::move(p_abort)) };
                    if (p_data.is_valid()) {
                        return p_data;
                    }
                }
            } catch (const ::exception_album_art_not_found&) {
                continue;
            }
        }
        return {};
    }

    //------------------------------------------------------

    inline auto query_single_external(const album_art::extractor_v1_ptr_t& p_extractor,
                                      const album_art::id_list_t& p_type_ids,
                                      foobar::abort_callback_param p_abort = {})
        -> decltype(query_single_external(std::declval<extractor_v2_ptr_t>(),
                                          p_type_ids)) {
        extractor_v2_ptr_t p_extractor_v2{};
        if (!p_extractor->service_query_t(p_extractor_v2)) { return {}; }
        return query_single_external(p_extractor_v2, p_type_ids, std::move(p_abort));
    }

    //------------------------------------------------------

    inline auto query_single_external(const album_art::manager_ptr_t& p_manager,
                                      const ::metadb_handle::ptr p_metadb,
                                      const album_art::id_list_t& p_type_ids,
                                      foobar::abort_callback_param p_abort = {})
        -> decltype(query_single_external(std::declval<extractor_ptr_t>(),
                                          p_type_ids)) {
        const auto p_extractor{ get_extractor(p_manager, p_metadb, p_type_ids) };
        assert(p_extractor.is_valid()); if (!p_extractor.is_valid()) { return {}; }
        return query_single_external(p_extractor, p_type_ids, std::move(p_abort));
    }

    //------------------------------------------------------

    inline auto query_single(const album_art::manager_ptr_t& p_manager,
                             ::metadb_handle::ptr p_metadb,
                             const album_art::id_list_t& p_type_ids,
                             foobar::abort_callback_param p_abort = {}) {
        auto p_data{ query_single_embeded(p_manager, p_metadb,
                                          p_type_ids, p_abort) };
        if (!p_data.is_valid()) {
            p_data = query_single_external(p_manager, p_metadb,
                                           p_type_ids, p_abort);
        }
        return p_data;
    }

    //------------------------------------------------------

    inline auto query_single(::metadb_handle::ptr p_metadb,
                             const album_art::id_list_t& p_type_ids,
                             foobar::abort_callback_param p_abort = {}) {
        return query_single(album_art::manager_t::get(),
                            p_metadb, p_type_ids, std::move(p_abort));
    }
} // namespace foobar::metadata

#endif // GUID_F7073DC8_E88E_4097_9002_BBF9B5B2BDB8
