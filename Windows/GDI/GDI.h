#pragma once
#ifndef GUID_06B5A1BC_8612_4660_AB99_9700B5298CD6
#define GUID_06B5A1BC_8612_4660_AB99_9700B5298CD6
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
#include <wingdi.h>
//--------------------------------------

//--------------------------------------
//
#include "Windows/Windows_Param.h"
//--------------------------------------

//--------------------------------------
//  Include ATL Collections
#ifdef ENABLE_ATL_GDI
#   include <atlcoll.h>
#   include <atlstr.h>
#   include <atltypes.h>

    namespace Windows::GDI {
        using ::ATL::CAtlArray;
        using ::ATL::CAutoPtrArray;
        using ::ATL::CSimpleArray;

        using ::ATL::CAtlList;
        using ::ATL::CAutoPtrList;

        using ::ATL::CAtlMap;
        using ::ATL::CSimpleMap;

        using ::ATL::CRBMap;
        using ::ATL::CRBMultiMap;
        using ::ATL::CRBTree;

        using ::ATL::CString;
        using ::ATL::CSimpleString;

        using ::CSize;
        using ::CPoint;
        using ::CRect;
    } //namespace Windows::GDI
//--------------------------------------

//--------------------------------------
//  Include WTL GDI support
#   ifdef ENABLE_WTL_GDI
#       include <atlapp.h>
#       include <atlgdi.h>

        namespace Windows::GDI {
            using ::WTL::CBitmap;
            using ::WTL::CBitmapHandle;
            using ::WTL::CBrush;
            using ::WTL::CBrushHandle;
            using ::WTL::CClientDC;
            using ::WTL::CDC;
            using ::WTL::CDCHandle;
            using ::WTL::CEnhMetaFile;
            using ::WTL::CEnhMetaFileDC;
            using ::WTL::CEnhMetaFileHandle;
            using ::WTL::CEnhMetaFileInfo;
            using ::WTL::CFont;
            using ::WTL::CFontHandle;
            using ::WTL::CLogFont;
            using ::WTL::CMemoryDC;
            using ::WTL::CPaintDC;
            using ::WTL::CPalette;
            using ::WTL::CPaletteHandle;
            using ::WTL::CPen;
            using ::WTL::CPenHandle;
            using ::WTL::CRgn;
            using ::WTL::CRgnHandle;
            using ::WTL::CWindowDC;
        } // namespace Windows::GDI
#   endif
#endif //ENABLE_ATL_GDI
//--------------------------------------

//==============================================================================

namespace Windows::GDI {
    //**************************************************************************
    // GDI Helpers
    //**************************************************************************
    BOOL HaveCurrentDC()                 noexcept;
    HDC  GetCurrentDC ()                 noexcept;
    void SetCurrentDC (_In_opt_ HDC hDC) noexcept;
} // namespace Windows::GDI

//==============================================================================

namespace Windows::GDI::detail {
    //**************************************************************************
    // Core Types
    //**************************************************************************
    using PointParam   = ::Windows::detail::ParamT<POINT>;
    using RectParam    = ::Windows::detail::ParamT<RECT>; // See also ATL::_U_RECT
    using SizeParam    = ::Windows::detail::ParamT<SIZE>;
    using LogFontParam = ::Windows::detail::ParamT<LOGFONT>;
} // namespace Windows::GDI::detail

//******************************************************************************
//******************************************************************************
// Scoped GDI Object Usage
//******************************************************************************
//******************************************************************************

//--------------------------------------
//  Internal Helper Macros
#define GDI_DEFINE_SCOPED_CLASS_IMPL(name, func, type) \
    class name final { \
    public: \
        name(_In_ HDC dc, _In_ type newValue) noexcept : hDC{ dc }, \
            oldValue{ static_cast<type>(::func(dc, newValue)) } { assert(hDC); } \
        ~name() noexcept { assert(hDC); ::func(hDC, oldValue); } \
    private: \
        name(const name& )              = delete; \
        name(      name&&)              = delete; \
        name& operator=(const name& )   = delete; \
        name& operator=(      name&&)   = delete; \
    private: \
        HDC hDC{ NULL }; \
        const type oldValue; \
    }

#define GDI_DEFINE_SCOPED_SELECT_CLASS(name, type) \
            GDI_DEFINE_SCOPED_CLASS_IMPL(ScopedSelect ## name, SelectObject, type)

#define GDI_DEFINE_SCOPED_SET_CLASS(func, type) \
            GDI_DEFINE_SCOPED_CLASS_IMPL(Scoped ## func, func, type)
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // Scoped Select
    //**************************************************************************
    GDI_DEFINE_SCOPED_SELECT_CLASS(Bitmap, HBITMAP);
    GDI_DEFINE_SCOPED_SELECT_CLASS(Brush,  HBRUSH);
    GDI_DEFINE_SCOPED_SELECT_CLASS(Pen,    HPEN);
    GDI_DEFINE_SCOPED_SELECT_CLASS(Font,   HFONT);
    GDI_DEFINE_SCOPED_SELECT_CLASS(Rgn,    HRGN);
    GDI_DEFINE_SCOPED_SELECT_CLASS(Region, HRGN);

    //**************************************************************************
    // Scoped Set
    //**************************************************************************
    GDI_DEFINE_SCOPED_SET_CLASS(SetBkMode,       int);
    GDI_DEFINE_SCOPED_SET_CLASS(SetBkColor,      COLORREF);
    GDI_DEFINE_SCOPED_SET_CLASS(SetDCPenColor,   COLORREF);
    GDI_DEFINE_SCOPED_SET_CLASS(SetDCBrushColor, COLORREF);
    GDI_DEFINE_SCOPED_SET_CLASS(SetTextColor,    COLORREF);
    GDI_DEFINE_SCOPED_SET_CLASS(SetMapMode,      int);
} // namespace Windows::GDI

//--------------------------------------
//  Remove Internal Helper Macros
#undef GDI_DEFINE_SCOPED_SELECT_CLASS
#undef GDI_DEFINE_SCOPED_SET_CLASS
#undef GDI_DEFINE_SCOPED_CLASS_IMPL
//--------------------------------------

#endif // GUID_06B5A1BC_8612_4660_AB99_9700B5298CD6