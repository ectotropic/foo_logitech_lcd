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
#include "CommonHeaders.h"
#include "logger.h"
//--------------------------------------

//--------------------------------------
//
#include <spdlog/sinks/msvc_sink.h>
//--------------------------------------

//--------------------------------------
//
#include <array>
#include <memory>
#include <utility>
//--------------------------------------

namespace {
    //**************************************************************************
    // Internal Helpers
    //**************************************************************************

    template <std::size_t CountT>
    using sink_array = std::array<::logger::sink_ptr_t, CountT>;

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <class... ArgsT>
    decltype(auto) make_logger(::logger::string_t name, ArgsT&&... Args) {
        return std::make_shared<::logger::logger_t>(name, std::forward<ArgsT>(Args)...);
    }

    //----------------------------------

    template <class... ArgsT>
    decltype(auto) make_logger_debug(::logger::string_t name, ArgsT&&... Args) {
        sink_array<2> sinks{
            std::make_shared<spdlog::sinks::msvc_sink_mt>(),
            std::forward<ArgsT>(Args)...
        };
        return make_logger(name, std::begin(sinks), std::end(sinks));
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    ::logger::logger_ptr_t g_MainLogger{};
} // namespace <anonymous>

//==============================================================================

namespace logger {
    //**************************************************************************
    // SPDLOG Helpers
    //**************************************************************************

    void initialise(::logger::string_t name,
                    ::logger::level_t level,
                    ::logger::sink_ptr_t sink,
                    ::logger::string_t pattern) {
#if (!defined(DEBUG) && !defined(_DEBUG)) || defined(NDEBUG)
        if (::IsDebuggerPresent()) {
#endif
            g_MainLogger = make_logger_debug(name, sink);
#if (!defined(DEBUG) && !defined(_DEBUG)) || defined(NDEBUG)
        } else {
            g_MainLogger = make_logger(name, sink);
        }
#endif
        if (g_MainLogger) { ::spdlog::register_logger(g_MainLogger); }
        if (g_MainLogger) { ::spdlog::set_default_logger(g_MainLogger); }

        ::spdlog::set_level(level);
        if (!pattern.empty()) {
            spdlog::set_pattern(pattern);
        }
    }

    //----------------------------------

    void uninitialise() noexcept {
        g_MainLogger.reset();
    }
} // namespace logger
