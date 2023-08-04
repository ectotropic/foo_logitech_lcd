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
#include "Windows/Windows_MessageQueue.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // EnumThreadWindows helpers
    //**************************************************************************

    //======================================================
    // ThreadWindowMessageParams
    //======================================================
    struct ThreadWindowMessageParams final {
        ThreadWindowMessageParams() = default;
        ThreadWindowMessageParams(_In_ UINT   msg,
                                  _In_ WPARAM w,
                                  _In_ LPARAM l) noexcept :
            uMessage{ msg },
            wParam  { w },
            lParam  { l } {}

        DWORD  dwSize  { sizeof(ThreadWindowMessageParams) };
        UINT   uMessage{ 0 };
        WPARAM wParam  { 0 };
        LPARAM lParam  { 0 };
    }; // struct ThreadWindowMessageParams final

    //------------------------------------------------------
    // PostThreadWindowMessageCallback
    //------------------------------------------------------
    BOOL CALLBACK PostThreadWindowMessageCallbackA(_In_ HWND hwnd,
                                                   _In_ LPARAM lParam) noexcept {
        assert(lParam); if (lParam == 0) { return FALSE; }
        const auto& params = *reinterpret_cast<ThreadWindowMessageParams*>(lParam);
        assert(params.dwSize == sizeof(ThreadWindowMessageParams));
        if (params.dwSize != sizeof(ThreadWindowMessageParams)) {
            return FALSE;
        }
        const auto res = ::PostMessageA(hwnd,
                                        params.uMessage,
                                        params.wParam,
                                        params.lParam);
        WinAPIAssert(res);
        return res;
    }

    BOOL CALLBACK PostThreadWindowMessageCallbackW(_In_ HWND hwnd,
                                                   _In_ LPARAM lParam) noexcept {
        assert(lParam); if (lParam == 0) { return FALSE; }
        const auto& params = *reinterpret_cast<ThreadWindowMessageParams*>(lParam);
        assert(params.dwSize == sizeof(ThreadWindowMessageParams));
        if (params.dwSize != sizeof(ThreadWindowMessageParams)) {
            return FALSE;
        }
        const auto res = ::PostMessageW(hwnd,
                                        params.uMessage,
                                        params.wParam,
                                        params.lParam);
        WinAPIAssert(res);
        return res;
    }
} // namespace <anonymous>

//==============================================================================

namespace Windows {
    //**************************************************************************
    // PostThreadWindowMessage
    //**************************************************************************
    BOOL PostThreadWindowMessageA(_In_ DWORD  idThread,
                                  _In_ UINT   uMsg,
                                  _In_ WPARAM wParam,
                                  _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        // NOTE: Only unsafe if EnumThreadWindows asynchronously!
        ThreadWindowMessageParams params{ uMsg, wParam, lParam };
        return ::EnumThreadWindows(idThread,
                                   ::PostThreadWindowMessageCallbackA,
                                   reinterpret_cast<LPARAM>(&params));
    }

    //------------------------------------------------------

    BOOL PostThreadWindowMessageW(_In_ DWORD  idThread,
                                  _In_ UINT   uMsg,
                                  _In_ WPARAM wParam,
                                  _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        // NOTE: Only unsafe if EnumThreadWindows asynchronously!
        ThreadWindowMessageParams params{ uMsg, wParam, lParam };
        return ::EnumThreadWindows(idThread,
                                   ::PostThreadWindowMessageCallbackW,
                                   reinterpret_cast<LPARAM>(&params));
    }

    //**************************************************************************
    // PostTerminateMessage
    //**************************************************************************
    BOOL PostTerminateMessageA(_In_opt_ HWND hwnd,
                               _In_ WPARAM wParam,
                               _In_ LPARAM lParam) noexcept {
        const auto res = ::PostMessageA(hwnd,
                                        WM_USER_TERMINATE,
                                        wParam,
                                        lParam);
        WinAPIAssert(res);
        return res;
    }

    //------------------------------------------------------

    BOOL PostTerminateMessageW(_In_opt_ HWND hwnd,
                               _In_ WPARAM wParam,
                               _In_ LPARAM lParam) noexcept {
        const auto res = ::PostMessageW(hwnd,
                                        WM_USER_TERMINATE,
                                        wParam,
                                        lParam);
        WinAPIAssert(res);
        return res;
    }

    //**************************************************************************
    // PostTerminateMessageThread
    //**************************************************************************
    BOOL PostTerminateMessageThreadA(_In_ DWORD  idThread,
                                     _In_ WPARAM wParam,
                                     _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        const auto res = ::PostThreadMessageA(idThread,
                                              WM_USER_TERMINATE,
                                              wParam,
                                              lParam);
        WinAPIAssert(res);
        return res;
    }

    //------------------------------------------------------

    BOOL PostTerminateMessageThreadW(_In_ DWORD  idThread,
                                     _In_ WPARAM wParam,
                                     _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        const auto res = ::PostThreadMessageW(idThread,
                                              WM_USER_TERMINATE,
                                              wParam,
                                              lParam);
        WinAPIAssert(res);
        return res;
    }

    //**************************************************************************
    // PostTerminateMessageThreadWindow
    //**************************************************************************
    BOOL PostTerminateMessageThreadWindowA(_In_ DWORD  idThread,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        return PostThreadWindowMessageA(idThread,
                                        WM_USER_TERMINATE,
                                        wParam,
                                        lParam);
    }

    //------------------------------------------------------

    BOOL PostTerminateMessageThreadWindowW(_In_ DWORD  idThread,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam) noexcept {
        assert(idThread != 0);
        return PostThreadWindowMessageW(idThread,
                                        WM_USER_TERMINATE,
                                        wParam,
                                        lParam);
    }
} // namespace Windows
