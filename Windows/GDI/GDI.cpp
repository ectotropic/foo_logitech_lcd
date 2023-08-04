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
#include "Windows/GDI/GDI.h"
//--------------------------------------

namespace {
    //**************************************************************************
    // DeviceContextHandle: Storage for the thread "main" device context
    //**************************************************************************
    struct DeviceContextHandle final {
        inline thread_local static HDC CurrentDeviceContext{ NULL };
    };
} // namespace <anonymous>

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // GDI Helpers
    //**************************************************************************
    BOOL HaveCurrentDC() noexcept {
        if (::DeviceContextHandle::CurrentDeviceContext == NULL) { return FALSE; }
        // No ::IsDC function, so use a function that's likely
        // to be cheap to execute and easy to test
        return ::GetBkMode(::DeviceContextHandle::CurrentDeviceContext) != 0 ? TRUE : FALSE;
    }

    //----------------------------------

    HDC GetCurrentDC() noexcept {
        return ::DeviceContextHandle::CurrentDeviceContext;
    }

    //----------------------------------

    void SetCurrentDC(_In_opt_ HDC hDC) noexcept {
        ::DeviceContextHandle::CurrentDeviceContext = hDC;
    }
} // namespace Windows::GDI
