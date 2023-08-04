#pragma once
#ifndef GUID_68FDF677_08D5_4AC7_B70E_8DFB01BB77AF
#define GUID_68FDF677_08D5_4AC7_B70E_8DFB01BB77AF
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
//
#include "Windows/Windows_Error.h"
//--------------------------------------

//--------------------------------------
//
#ifndef WM_USER_TERMINATE
#   define WM_USER_TERMINATE WM_USER + 1
#endif
//--------------------------------------

namespace Windows {
    //**************************************************************************
    // PostThreadWindowMessage
    // -----------------------
    //
    // Post a message to all windows owned by a specific thread.
    //**************************************************************************
    BOOL PostThreadWindowMessageA(_In_ DWORD  idThread,
                                  _In_ UINT   uMsg,
                                  _In_ WPARAM wParam,
                                  _In_ LPARAM lParam) noexcept;

    BOOL PostThreadWindowMessageW(_In_ DWORD  idThread,
                                  _In_ UINT   uMsg,
                                  _In_ WPARAM wParam,
                                  _In_ LPARAM lParam) noexcept;
#ifdef UNICODE
#   define PostThreadWindowMessage  PostThreadWindowMessageW
#else
#   define PostThreadWindowMessage  PostThreadWindowMessageA
#endif

    //**************************************************************************
    // PostTerminateMessage
    // --------------------
    //
    // Like PostQuitMessage but using WM_USER_TERMINATE.
    //**************************************************************************
    BOOL PostTerminateMessageA(_In_opt_ HWND hwnd,
                               _In_ WPARAM wParam = 0,
                               _In_ LPARAM lParam = 0) noexcept;

    BOOL PostTerminateMessageW(_In_opt_ HWND hwnd,
                               _In_ WPARAM wParam = 0,
                               _In_ LPARAM lParam = 0) noexcept;
#ifdef UNICODE
#   define PostTerminateMessage  PostTerminateMessageW
#else
#   define PostTerminateMessage  PostTerminateMessageA
#endif

    inline BOOL PostTerminateMessageA(_In_ WPARAM wParam = 0,
                                      _In_ LPARAM lParam = 0) noexcept {
        return PostTerminateMessageA(NULL, wParam, lParam);
    }

    inline BOOL PostTerminateMessageW(_In_ WPARAM wParam = 0,
                                      _In_ LPARAM lParam = 0) noexcept {
        return PostTerminateMessageW(NULL, wParam, lParam);
    }
#ifdef UNICODE
#   define PostTerminateMessage  PostTerminateMessageW
#else
#   define PostTerminateMessage  PostTerminateMessageA
#endif

    //**************************************************************************
    // PostTerminateMessageThread
    // --------------------------
    //
    // PostTerminateMessage to a specific thread.
    //**************************************************************************
    BOOL PostTerminateMessageThreadA(_In_ DWORD  idThread,
                                     _In_ WPARAM wParam = 0,
                                     _In_ LPARAM lParam = 0) noexcept;

    BOOL PostTerminateMessageThreadW(_In_ DWORD  idThread,
                                     _In_ WPARAM wParam = 0,
                                     _In_ LPARAM lParam = 0) noexcept;
#ifdef UNICODE
#   define PostTerminateMessageThread  PostTerminateMessageThreadW
#else
#   define PostTerminateMessageThread  PostTerminateMessageThreadA
#endif

    //**************************************************************************
    // PostTerminateMessageThreadWindow
    // --------------------------------
    //
    // Post WM_USER_TERMINATE to all windows owned by a specific thread.
    //**************************************************************************
    BOOL PostTerminateMessageThreadWindowA(_In_ DWORD  idThread,
                                           _In_ WPARAM wParam = 0,
                                           _In_ LPARAM lParam = 0) noexcept;

    BOOL PostTerminateMessageThreadWindowW(_In_ DWORD  idThread,
                                           _In_ WPARAM wParam = 0,
                                           _In_ LPARAM lParam = 0) noexcept;
#ifdef UNICODE
#   define PostTerminateMessageThreadWindow  PostTerminateMessageThreadWindowW
#else
#   define PostTerminateMessageThreadWindow  PostTerminateMessageThreadWindowA
#endif
} // namespace Windows

#endif // GUID_68FDF677_08D5_4AC7_B70E_8DFB01BB77AF