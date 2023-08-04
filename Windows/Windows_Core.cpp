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

namespace {
    //**************************************************************************
    // InstanceHandle: storage for the application HINSTANCE
    //**************************************************************************
    struct InstanceHandle final {
        inline static HINSTANCE s_hInstance{ NULL };
    };
} // namespace <anonymous>

//==============================================================================

namespace Windows {
    //**************************************************************************
    // Application HINSTANCE helpers
    //**************************************************************************
    BOOL HaveInstanceHandle() noexcept {
        return ::InstanceHandle::s_hInstance != NULL ? TRUE : FALSE;
    }

    HINSTANCE GetInstanceHandle() noexcept {
        return ::InstanceHandle::s_hInstance;
    }

    void SetInstanceHandle(_In_ HINSTANCE instance) noexcept {
        ::InstanceHandle::s_hInstance = instance;
    }
} // namespace Windows
