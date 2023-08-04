#pragma once
#ifndef GUID_419F7FCB_229A_4C25_8ADE_9693B12A04C3
#define GUID_419F7FCB_229A_4C25_8ADE_9693B12A04C3
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

//******************************************************************************
//******************************************************************************
// Global settings
//******************************************************************************
//******************************************************************************

//--------------------------------------
// Debug/release
#if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)
#   ifndef DEBUG
#       define DEBUG
#   endif
#   ifndef _DEBUG
#       define _DEBUG
#   endif
#   ifdef RELEASE
#	    undef RELEASE
#   endif
#   ifdef _RELEASE
#	    undef _RELEASE
#   endif
#else
#   ifndef RELEASE
#       define RELEASE
#   endif
#   ifndef _RELEASE
#       define _RELEASE
#   endif
#   ifndef NDEBUG
#       define NDEBUG
#   endif
#endif // #if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)
//--------------------------------------

//******************************************************************************
//******************************************************************************
// Platform Headers/Settings
//******************************************************************************
//******************************************************************************

//--------------------------------------
// Windows
#ifdef _WIN32
#   include "Windows/Windows_Core.h"
#endif // #ifdef _WIN32
//--------------------------------------

//******************************************************************************
//******************************************************************************
// Common Headers
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#include <cassert>
#include <cstdint>

#include <utility>  // move, exchange
//--------------------------------------

//--------------------------------------
//
#include "logger.h"
//--------------------------------------

//******************************************************************************
//******************************************************************************
// Helper Macros
//******************************************************************************
//******************************************************************************

//--------------------------------------
//
#ifndef HintAssume
#   ifdef _MSC_VER
#	    define HintAssume(e)    assert(e); __assume(e)
#   else
#	    define HintAssume(e)    assert(e)
#   endif
#endif

#ifndef HintUnreachable
#   define HintUnreachable()	HintAssume(false);
#endif

#ifndef HintNoDefault
#   define HintNoDefault()		default: HintUnreachable();
#endif
//--------------------------------------

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

//--------------------------------------
//
#ifndef noexcept_if
#   define noexcept_if(...)    noexcept(noexcept(__VA_ARGS__))
#endif
//--------------------------------------

#endif // GUID_419F7FCB_229A_4C25_8ADE_9693B12A04C3
