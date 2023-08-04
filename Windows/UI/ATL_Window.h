#pragma once
#ifndef GUID_7E5410BC_EFEF_4A37_9CD9_C5DB2420F272
#define GUID_7E5410BC_EFEF_4A37_9CD9_C5DB2420F272
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

namespace Windows::UI {
    //**************************************************************************
    // CATLWindow
    //**************************************************************************
    class ATL_NO_VTABLE CATLWindow : public CWindow {
    private:
        using thisClass = CATLWindow;
        using baseClass = CWindow;

    public:
        using windowClass     = CWindow;
        using stringClass     = CString;
        using stringBaseClass = CSimpleString;

    public:
        using baseClass::baseClass;

    public:
        LRESULT SendMessageToParent(_In_ UINT message,
                                    _In_ WPARAM wParam = 0,
                                    _In_ LPARAM lParam = 0) throw() {
            WinAPIAssert(IsWindow());
            auto parent = GetParent();
            WinAPIAssert(parent.IsWindow());
            if (!parent.IsWindow()) { return 0; }
            return parent.SendMessage(message, wParam, lParam);
        }

    public:
        using baseClass::GetWindowTextLength;

        auto GetWindowTextLength(_Out_ LPUINT lpLength) const throw() {
            WinAPIAssert(IsWindow()); WinAPIAssert(lpLength);
            if (!lpLength) { return FALSE; }
            (*lpLength) = 0;
            if (UINT uLength = baseClass::GetWindowTextLength()) {
                (*lpLength) = uLength;
                return TRUE;
            } else {
                WinAPIAssertErrorSuccess();
                return (::GetLastError() == ERROR_SUCCESS) ? TRUE : FALSE;
            }
        }
    }; // class CATLWindow
} // namespace Windows::UI

#endif // GUID_7E5410BC_EFEF_4A37_9CD9_C5DB2420F272