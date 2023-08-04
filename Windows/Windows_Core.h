#pragma once
#ifndef GUID_54F94C98_C872_4570_B3FD_BDB5A96C27AC
#define GUID_54F94C98_C872_4570_B3FD_BDB5A96C27AC
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
#include "Windows/_config.h"
//--------------------------------------

//--------------------------------------
// HACK:
// <windows.h> pulls in <winsock.h>
// which means trying to use
// <winsock2.h> later, breaks
// everything.
//
// <windows.h> sucks!
#ifndef ENABLE_WINSOCK_V1
#   ifdef _WINSOCKAPI_
#       define WINSOCKAPI_WAS_PREDEFINED
#   else
#       define _WINSOCKAPI_
#   endif
#endif

#include <windows.h>

#ifndef ENABLE_WINSOCK_V1
#   ifdef WINSOCKAPI_WAS_PREDEFINED
#       undef WINSOCKAPI_WAS_PREDEFINED
#   else
#       undef _WINSOCKAPI_
#   endif
#endif
//--------------------------------------

//--------------------------------------
//
#ifndef WM_USER_TERMINATE
#   define WM_USER_TERMINATE (WM_USER + 1)
#endif
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_Error.h"
//--------------------------------------

namespace Windows {
    //**************************************************************************
    // Application HINSTANCE helpers
    //
    // NOTE: Not thread safe (shouldn't matter though).
    //**************************************************************************
    BOOL      HaveInstanceHandle() noexcept;
    HINSTANCE GetInstanceHandle() noexcept;
    void      SetInstanceHandle(_In_ HINSTANCE instance) noexcept;
} // namespace Windows

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#ifdef _MSC_VER
#   ifndef pragma_suppress
#	    define pragma_suppress(...)     __pragma(warning(suppress:__VA_ARGS__))
#   endif
#   if defined(_PREFAST_) && !defined(gsl_suppress)
#       define gsl_suppress(...)        [[gsl::suppress(__VA_ARGS__)]]
#   endif
#endif

#ifndef pragma_suppress
#   define pragma_suppress(...)
#endif

#ifndef gsl_suppress
#   define gsl_suppress(...)
#endif
//--------------------------------------

#endif // #ifndef GUID_54F94C98_C872_4570_B3FD_BDB5A96C27AC