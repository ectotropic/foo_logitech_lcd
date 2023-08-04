#pragma once
#ifndef GUID_3FD9CE33_5FF8_4620_98A5_39BAD6CFB391
#define GUID_3FD9CE33_5FF8_4620_98A5_39BAD6CFB391
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
#include "foobar/foobar_sdk.h"
//--------------------------------------

//--------------------------------------
//
#include "Windows/UI/ATL_PreferencesDialog.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/foobar_pref_vis_spectrum_analyser.h"
#include "foobar/UI/foobar_pref_vis_oscilloscope.h"
#include "foobar/UI/foobar_pref_vis_vu_meter.h"
#include "foobar/UI/foobar_pref_vis_track_details.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI {
    /**************************************************************************
     * CVisualisationsDlg *
     **************************************************************************/
    class CVisualisationsDlg :
        public Windows::UI::CDialogImpl<CVisualisationsDlg, Windows::UI::CPreferencesDialog> {
    private: // Internal Types
        using thisClass  = CVisualisationsDlg;
        using baseClass  = Windows::UI::CPreferencesDialog;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        template <class DialogT>
        using CDialogPtrT           = Windows::UI::CAutoPtr<DialogT>;

        template <typename TypeT>
        using CVisDialogPtrT        = Windows::UI::CAutoPtr<CVisualisationPrefsDlgT<TypeT>>;

        using CVisDialogPtr_Spec    = CVisDialogPtrT<SpectrumAnalyserType>;
        using CVisDialogPtr_Osc     = CVisDialogPtrT<OscilloscopeType>;
        using CVisDialogPtr_VU      = CVisDialogPtrT<VUMeterType>;
        using CVisDialogPtr_Details = CVisDialogPtrT<TrackDetailsType>;

        using CDialogs_Spec         = std::array<CVisDialogPtr_Spec   , SpectrumAnalyserType::count()>;
        using CDialogs_Osc          = std::array<CVisDialogPtr_Osc    , OscilloscopeType::count()>;
        using CDialogs_VU           = std::array<CVisDialogPtr_VU     , VUMeterType::count()>;
        using CDialogs_TrackDetails = std::array<CVisDialogPtr_Details, TrackDetailsType::count()>;

        struct TreeCtrlItemData {
            TreeCtrlItemData() noexcept {
                ZeroMemory(this, sizeof(TreeCtrlItemData));
            }

            TreeCtrlItemData(pfnShowDialogCallBack_t pfShow,
                             LPVOID pDlg) noexcept :
                pfShowDialog{ pfShow },
                pDialog     { pDlg } {}

            pfnShowDialogCallBack_t pfShowDialog{ NULL };
            LPVOID                  pDialog     { NULL };
        };

        using CTreeData       = Windows::UI::CSimpleMap<HTREEITEM, TreeCtrlItemData>;
        using CTreeViewCtrl   = Windows::UI::CTreeViewCtrl;
        using CTreeActiveItem = Windows::UI::CWindow;

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;

    public:
        CVisualisationsDlg(const SVisPrefConfig& config) :
            m_Config{ config } {}

    private: // No Copy
        CVisualisationsDlg(const CVisualisationsDlg&) = delete;
        CVisualisationsDlg& operator=(const CVisualisationsDlg&) = delete;

    public:
        enum {
            IDD                 = IDD_VIS_CFG_TAB,
            IDD_TREE_ITEM_SIZER = IDC_VIS_CFG_SIZER_STATIC,
        };

    public:
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd, nShowCmd, hSizerWnd);
        }

    protected:
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG     , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW     , OnShow)
            MESSAGE_HANDLER(WM_CHANGEUISTATE  , OnChangeUIState)
            NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelectionChanged)             pragma_suppress(26454) // C26454: Arithmetic overflow: '...' operation produces a negative unsigned result at compile time (io.5).
        END_MSG_MAP();

        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnChangeUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnSelectionChanged(_In_ INT iCtrlID,
                                   _In_ LPNMHDR pNotifyDesc,
                                   _Inout_ BOOL& bHandled);

    private:
        void BuildTree();

        gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        HTREEITEM InsertTreeBranch(_In_z_ LPTSTR szName) noexcept {
            TVINSERTSTRUCT TreeInsertStruct;
            ZeroMemory(&TreeInsertStruct, sizeof(TreeInsertStruct));
            TreeInsertStruct.hParent          = TVI_ROOT;
            TreeInsertStruct.hInsertAfter     = TVI_LAST;
            TreeInsertStruct.itemex.mask      = TVIF_CHILDREN | TVIF_TEXT;
            TreeInsertStruct.itemex.cChildren = 1; // 0 == no children; 1 == one or more children
            TreeInsertStruct.itemex.pszText   = szName;

            auto hItem = m_TreeControl.InsertItem(&TreeInsertStruct);
            ATLASSERT(hItem);
            return hItem;
        }

        template <class DialogT> gsl_suppress(26447) // C26447: The function is declared 'noexcept' but calls function '...' which may throw exceptions (f.6).
        HTREEITEM InsertTreeLeaf(_In_ HTREEITEM hParent,
                                 _In_ CDialogPtrT<DialogT>& pDialog) noexcept {
            ATLASSERT(pDialog);
            TVINSERTSTRUCT TreeInsertStruct;
            ZeroMemory(&TreeInsertStruct, sizeof(TreeInsertStruct));
            TreeInsertStruct.hParent          = hParent;
            TreeInsertStruct.hInsertAfter     = TVI_LAST;
            TreeInsertStruct.itemex.mask      = TVIF_CHILDREN | TVIF_TEXT;
            TreeInsertStruct.itemex.cChildren = 0;
            TreeInsertStruct.itemex.pszText   = (LPTSTR)(pDialog->Name());

            auto hItem = m_TreeControl.InsertItem(&TreeInsertStruct);
            ATLASSERT(hItem);

            const TreeCtrlItemData data{ &DialogT::ShowDialog, pDialog.m_p };
            const auto nIndex = m_ItemData.FindKey(hItem);
            if (nIndex == -1) {
                ATLVERIFY(m_ItemData.Add(hItem, data));
            } else {
                ATLVERIFY(m_ItemData.SetAtIndex(nIndex, hItem, data));
            }
            return hItem;
        }

        BOOL GetTreeItemData(_In_ HTREEITEM hItem,
                             _In_ TreeCtrlItemData& data) const {
            const int nIndex = m_ItemData.FindKey(hItem);
            if (nIndex == -1) { return FALSE; }
            data = m_ItemData.GetValueAt(nIndex);
            return TRUE;
        }

        void ShowTreeItem(_In_ HTREEITEM hItem) {
            if (m_ActiveItem.IsWindow()) {
                //TODO: Avoid hide if same window!
                m_ActiveItem.ShowWindow(SW_HIDE);
                m_ActiveItem.Detach();
            }
            TreeCtrlItemData data{};
            if (GetTreeItemData(hItem, data)) {
                HWND hDlg = data.pfShowDialog(*this, SW_SHOW, data.pDialog,
                                              GetDlgItem(IDD_TREE_ITEM_SIZER));
                m_ActiveItem.Attach(hDlg);
            }
        }

        constexpr auto&       Config() const noexcept { return m_Config; }
        constexpr auto& CanvasConfig() const noexcept { return m_Config.Canvas; }
        constexpr auto&   CoreConfig() const noexcept { return *m_Config.pConfig; }

    private:
        CDialogs_Spec         m_Spectrum     {};
        CDialogs_Osc          m_Oscilloscope {};
        CDialogs_VU           m_VUMeter      {};
        CDialogs_TrackDetails m_TrackDetails {};

        CTreeData       m_ItemData   {};
        CTreeActiveItem m_ActiveItem {};
        CTreeViewCtrl   m_TreeControl{};
        SVisPrefConfig  m_Config     {};
    };

    //------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------
} // namespace foobar::UI

#endif //GUID_3FD9CE33_5FF8_4620_98A5_39BAD6CFB391