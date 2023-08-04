#pragma once
#ifndef GUID_FE1752EF_C6A2_4A12_9631_DC44206E8ABC
#define GUID_FE1752EF_C6A2_4A12_9631_DC44206E8ABC
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
//  Include ATL Core
#include <atlbase.h>
#include <atldef.h>
#include <atlwin.h>
#include <atlstr.h>
//--------------------------------------

//--------------------------------------
//  Include Comm
#include <commdlg.h>
#include <Cderr.h>
//--------------------------------------

//--------------------------------------
//  Include WTL
#include <atlapp.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlgdi.h>
//--------------------------------------

//--------------------------------------
// Additional Macros
#ifndef DLG_TRANS_FAILURE
#   define DLG_TRANS_FAILURE    FALSE
#endif

#ifndef DLG_TRANS_SUCCESS
#   define DLG_TRANS_SUCCESS    TRUE
#endif

#ifndef DLG_TRANS_CLAMPED
#   define DLG_TRANS_CLAMPED    (DLG_TRANS_SUCCESS + 1)
#endif
//--------------------------------------

//--------------------------------------
//
namespace Windows::UI {
    using namespace ::ATL;
    using namespace ::WTL;

    BOOL HaveMainWindow()               noexcept;
    HWND  GetMainWindow()               noexcept;
    void  SetMainWindow(_In_opt_ HWND hWnd) noexcept;
}
//--------------------------------------

#endif // GUID_FE1752EF_C6A2_4A12_9631_DC44206E8ABC