#pragma once
#ifndef GUID_5178B707_2B8A_448E_AA66_536CB3CE2196
#define GUID_5178B707_2B8A_448E_AA66_536CB3CE2196
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
// Feature Configuration
//******************************************************************************

//--------------------------------------
//
#define ENABLE_ATL
//--------------------------------------

//--------------------------------------
//
#define ENABLE_WTL
//--------------------------------------

//--------------------------------------
//
#define ENABLE_GDI
//--------------------------------------

//--------------------------------------
//
#define ENABLE_ATL_GDI
//--------------------------------------

//--------------------------------------
//
#define ENABLE_WTL_GDI
//--------------------------------------

//--------------------------------------
//
#define ENABLE_GDI_PLUS
//--------------------------------------

//******************************************************************************
// Feature Verification
//******************************************************************************
#if defined(ENABLE_WTL) && !defined(ENABLE_ATL)
#   error "Invalid configuration"
#endif

#if defined(ENABLE_ATL_GDI) && !(defined(ENABLE_GDI) && defined(ENABLE_ATL))
#   error "Invalid configuration"
#endif

#if defined(ENABLE_WTL_GDI) && !(defined(ENABLE_GDI) && defined(ENABLE_WTL))
#   error "Invalid configuration"
#endif

//******************************************************************************
// General Windows Configuration
//******************************************************************************

//--------------------------------------
// Default SDK version
#ifndef WINVER
#   define WINVER           0x0601  // 0x0601 == Windows 7
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT     WINVER
#endif

#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS   WINVER
#endif

#ifndef NTDDI_VERSION
#   define NTDDI_VERSION    0x06010000
#endif
//--------------------------------------

//--------------------------------------
// Silence some warnings
#ifndef _CRT_SECURE_NO_DEPRECATE
#   define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _SCL_SECURE_NO_WARNINGS
#   define _SCL_SECURE_NO_WARNINGS
#endif
#ifndef _AFX_SECURE_NO_WARNINGS
#   define _AFX_SECURE_NO_WARNINGS
#endif
#ifndef _ATL_SECURE_NO_WARNINGS
#   define _ATL_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#   define _CRT_NONSTDC_NO_WARNINGS
#endif
//--------------------------------------

//--------------------------------------
// Unicode
//VC:	_UNICODE	Use Unicode character set
//VC:	_MBCS		Use Multi-byte character set
#if defined(_UNICODE) || defined(UNICODE)
#   ifndef UNICODE
#       define UNICODE
#   endif
#   ifndef _UNICODE
#       define _UNICODE
#   endif
#   ifndef _MBCS
#       define _MBCS
#   endif
#endif
//--------------------------------------

//--------------------------------------
// Disable components not required
#ifndef ENABLE_GDI_PLUS
#   define WIN32_LEAN_AND_MEAN
#endif
#define VC_EXTRALEAN

//Disable additional things not used
//#define NOATOM
//#define NOCLIPBOARD
//#define NOCOLOR
//#define NOCOMM
#define NOCRYPT
//#define NOCTLMGR
//#define NODEFERWINDOWPOS
#ifndef ENABLE_GDI
#   define NODRAWTEXT
#   define NOGDI
#   define NOGDICAPMASKS
#endif
//#define NOHELP
//#define NOICONS
#define NOIME
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
//#define NOMB
#define NOMCX
#define NOMEMMGR
//#define NOMENUS
//#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
//#define NONLS
//#define NOOPENFILE
#define NOPROFILER
#ifndef ENABLE_GDI
#   define NORASTEROPS
#endif
//#define NOSCROLL
#define NOSERVICE
//#define NOSHOWWINDOW
#define NOSOUND
//#define NOSYSCOMMANDS
//#define NOSYSMETRICS
#ifndef ENABLE_GDI
#   define NOTEXTMETRIC
#endif
//#define NOUSER
//#define NOVIRTUALKEYCODES
#define NOWH
//#define NOWINOFFSETS
//#define NOWINSTYLES

#define OEMRESOURCE
//--------------------------------------

#endif // GUID_5178B707_2B8A_448E_AA66_536CB3CE2196
