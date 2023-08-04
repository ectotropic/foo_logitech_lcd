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
namespace {
    struct MainWindowHandle final {
        inline static HWND window_{ NULL };
    };
} // namespace <anonymous>
//--------------------------------------

//--------------------------------------
//
namespace Windows::UI {
    BOOL HaveMainWindow() noexcept {
        if (::MainWindowHandle::window_ == NULL) { return FALSE; }
        return ::IsWindow(::MainWindowHandle::window_);
    }

    HWND GetMainWindow() noexcept {
        return ::MainWindowHandle::window_;
    }

    void SetMainWindow(_In_opt_ HWND hWnd) noexcept {
        ::MainWindowHandle::window_ = hWnd;
    }
} // namespace Windows::UI
//--------------------------------------
