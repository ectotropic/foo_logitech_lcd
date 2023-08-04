#pragma once
#ifndef GUID_325F9701_A3DD_45BF_A809_4558ED5A4007
#define GUID_325F9701_A3DD_45BF_A809_4558ED5A4007
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
#include "Windows/UI/ATL_Window.h"
//--------------------------------------

namespace Windows::UI {
    //**************************************************************************
    // CEnumComboBoxT
    //**************************************************************************
    template <
        typename EnumT,
        class HelperT,
        class WindowT = Windows::UI::CATLWindow
    >
    class CEnumComboBoxT :
        public Windows::UI::CComboBoxT<WindowT>,
        private HelperT {
    private:
        using windowClass = WindowT;
        using helperClass = HelperT;
        using baseClass   = Windows::UI::CComboBoxT<windowClass>;
        using thisClass   = CEnumComboBoxT<EnumT, HelperT, WindowT>;

    public:
        using enumType = EnumT;
        using dataType = DWORD_PTR;

    private:
        const auto& helper() const noexcept {
            return reinterpret_cast<const helperClass&>(*this);
        }

        //------------------------------

        auto& helper() noexcept {
            return reinterpret_cast<helperClass&>(*this);
        }

        //------------------------------

        decltype(auto) begin() const
            noexcept(noexcept(helper().begin())) {
            return helper().begin();
        }

        //------------------------------

        decltype(auto) end() const
            noexcept(noexcept(helper().end())) {
            return helper().end();
        }

        //------------------------------

        decltype(auto) to_string(enumType e) const
            noexcept(noexcept(helper().to_string(e))) {
            return helper().to_string(e);
        }

        //------------------------------

        decltype(auto) next(enumType e) const
            noexcept(noexcept(helper().next(e))) {
            return helper().next(e);
        }

        //------------------------------

        decltype(auto) to_enum(dataType d) const
            noexcept(noexcept(helper().to_enum(d))) {
            return helper().to_enum(d);
        }

        //------------------------------

        decltype(auto) to_data(enumType e) const
            noexcept(noexcept(helper().to_data(e))) {
            return helper().to_data(e);
        }

    public:
        using baseClass::baseClass;

        //------------------------------

        BOOL InsertAll() {
            baseClass::ResetContent();
            for (enumType eType = begin(); eType != end(); eType = next(eType)) {

                const auto nIndex = baseClass::InsertString(-1, to_string(eType));
                ATLASSERT(nIndex != CB_ERR && nIndex != CB_ERRSPACE);
                if (nIndex < 0) { return FALSE; }

                const auto nResult = baseClass::SetItemData(nIndex, to_data(eType));
                ATLASSERT(nResult != CB_ERR);
                if (nResult == CB_ERR) { return FALSE; }
            }
            return TRUE;
        }

        //------------------------------

        BOOL GetCurSelVal(enumType& val) const {
            const auto nIndex = baseClass::GetCurSel();
            ATLASSERT(nIndex != CB_ERR);
            if (nIndex < 0) { return FALSE; }

            const auto nData = baseClass::GetItemData(nIndex);
            ATLASSERT(nData != CB_ERR);
            if (nData == CB_ERR) { return FALSE; }

            val = to_enum(nData);
            return TRUE;
        }

        //------------------------------

        auto SelectValue(enumType val) {
            const auto nIndex = baseClass::SelectString(-1, to_string(val));
            ATLASSERT(nIndex != CB_ERR);
            return nIndex;
        }
    }; //class CEnumComboBoxT
} // namespace Windows::UI
#endif // GUID_325F9701_A3DD_45BF_A809_4558ED5A4007