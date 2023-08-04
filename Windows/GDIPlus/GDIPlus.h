#pragma once
#ifndef GUID_05C80B96_C9A4_4F90_9944_5EF18214F359
#define GUID_05C80B96_C9A4_4F90_9944_5EF18214F359
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
#include "Windows/Windows_Core.h"
//--------------------------------------

//--------------------------------------
//  Include GDI+
#ifndef GDIPVER
#   define GDIPVER 0x0110
#endif

#include <GdiPlus.h>
//--------------------------------------

//--------------------------------------
//
#include "Windows/GDIPlus/GDIPlus_Error.h"
//--------------------------------------

namespace Windows::GDIPlus {
    //**************************************************************************
    // Import
    //**************************************************************************
    using namespace ::Gdiplus;

    //**************************************************************************
    // GDIPlus
    //**************************************************************************
    struct GDIPlus final {
    public:
        using token_type  = ULONG_PTR;
        using input_type  = ::Gdiplus::GdiplusStartupInput;
        using output_type = ::Gdiplus::GdiplusStartupOutput;

    public:
        static auto Startup(const input_type& input = {},
                            output_type* output = nullptr) noexcept {
            token_type token{ 0 };
            ::Gdiplus::GdiplusStartup(&token, &input, output);
            return token;
        }

        static auto Shutdown(token_type token) noexcept {
            ::Gdiplus::GdiplusShutdown(token);
        }
    };

    //**************************************************************************
    // ScopedStartup
    //**************************************************************************
    class ScopedStartup final {
    public:
        using startup_type = ::Windows::GDIPlus::GDIPlus;
        using token_type   = typename startup_type::token_type;
        using input_type   = typename startup_type::input_type;
        using output_type  = typename startup_type::output_type;

    private:
        ScopedStartup(const ScopedStartup& )            = delete;
        ScopedStartup(      ScopedStartup&&)            = delete;
        ScopedStartup& operator=(const ScopedStartup& ) = delete;
        ScopedStartup& operator=(      ScopedStartup&&) = delete;

    public:
        ScopedStartup(const input_type& input = {},
                      output_type* output = nullptr) noexcept :
            m_Token{ startup_type::Startup(input, output) } {}

        ~ScopedStartup() noexcept {
            startup_type::Shutdown(m_Token);
        }

    private:
        const token_type m_Token{ 0 };
    }; // class ScopedStartup final
} // namespace Windows::GDIPlus

#endif // GUID_05C80B96_C9A4_4F90_9944_5EF18214F359