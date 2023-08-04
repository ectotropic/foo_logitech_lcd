#pragma once
#ifndef GUID_5A621F4C_E084_44E5_8945_390D492D5B7F
#define GUID_5A621F4C_E084_44E5_8945_390D492D5B7F
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
#include "Windows/UI/UI.h"
//--------------------------------------

//--------------------------------------
//
#include <WinUser.h>
//--------------------------------------

//--------------------------------------
//
#include <utility>
//--------------------------------------

//--------------------------------------
//
namespace Windows::UI {
    //**************************************************************************
    // WindowProcHelperT
    //**************************************************************************
    template <class WindowT>
    struct WindowProcHelperT {
        static LRESULT CALLBACK WindowProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam) noexcept {
            gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
            WindowT* pWindow = (WindowT*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (pWindow) {
                return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
            } else {
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }

        static auto GetWindowProc() noexcept {
            return &WindowProcHelperT::WindowProc;
        }
    };

    //**************************************************************************
    // WindowClass
    //**************************************************************************
    class WindowClass final {
    public:
        static auto Register(_In_ const WNDCLASSEXA& wndClass) noexcept {
            const auto atom = ::RegisterClassExA(&wndClass);
            WinAPIAssert(atom != 0);
            return atom;
        }

        static auto Register(_In_ const WNDCLASSEXW& wndClass) noexcept {
            const auto atom = ::RegisterClassExW(&wndClass);
            WinAPIAssert(atom != 0);
            return atom;
        }

        static auto Unregister(_In_ HINSTANCE instance,
                               _In_z_ LPCSTR szClass) noexcept {
            const auto res = ::UnregisterClassA(szClass, instance);
            WinAPIAssert(res);
            return res;
        }

        static auto Unregister(_In_ HINSTANCE instance,
                               _In_z_ LPCWSTR szClass) noexcept {
            const auto res = ::UnregisterClassW(szClass, instance);
            WinAPIAssert(res);
            return res;
        }

        static decltype(auto) Unregister(_In_ HINSTANCE instance,
                                         _In_ ATOM atom) noexcept {
            return Unregister(instance, reinterpret_cast<LPCTSTR>(atom));
        }

    private:
        WindowClass(const WindowClass&)            = delete; // No Copy
        WindowClass& operator=(const WindowClass&) = delete; // No Copy

    public:
        WindowClass() = default;

        WindowClass(_In_ HINSTANCE instance,
                    _In_ ATOM atom) noexcept :
            hInstance{ instance },
            atomClass{ atom } {}

        WindowClass(_In_ const WNDCLASSEXA& wndClass) noexcept :
            hInstance{ wndClass.hInstance },
            atomClass{ Register(wndClass) } {}

        WindowClass(_In_ const WNDCLASSEXW& wndClass) noexcept :
            hInstance{ wndClass.hInstance },
            atomClass{ Register(wndClass) } {}


        WindowClass(_In_ WindowClass&& other) noexcept :
            hInstance{ std::exchange(other.hInstance, (HINSTANCE)NULL) },
            atomClass{ std::exchange(other.atomClass, (ATOM)0) } {}


        WindowClass& operator=(_In_ WindowClass&& other) noexcept {
            Unregister();
            hInstance = std::exchange(other.hInstance, (HINSTANCE)NULL);
            atomClass = std::exchange(other.atomClass, (ATOM)0);
            return *this;
        }

        ~WindowClass() noexcept { Unregister(); }

        bool Unregister() noexcept {
            if (atomClass) {
                return Unregister(std::exchange(hInstance, (HINSTANCE)NULL),
                                  std::exchange(atomClass, (ATOM)0)) != FALSE;
            }
            return false;
        }

        constexpr const auto GetInstance () const noexcept { return hInstance; }
        constexpr const auto GetClassAtom() const noexcept { return atomClass; }

        constexpr explicit operator bool() const noexcept { return atomClass != 0; }
        constexpr operator ATOM() const noexcept { return atomClass; }

        const auto GetClass() const noexcept {
            return reinterpret_cast<LPCTSTR>(atomClass);
        }

        operator LPCTSTR() const noexcept { return GetClass(); }

        HINSTANCE hInstance{ NULL };
        ATOM      atomClass{ 0 };
    }; // class WindowClass final
} // namespace Windows::UI

#endif // GUID_5A621F4C_E084_44E5_8945_390D492D5B7F