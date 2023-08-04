#pragma once
#ifndef GUID_092C2A34_6D73_49D5_A963_0357BC35E999
#define GUID_092C2A34_6D73_49D5_A963_0357BC35E999
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
#include "foobar/UI/foobar_pref_vis_dlg.h"
#include "Windows/UI/ATL_EnumComboBox.h"
#include "Config/Config_TrackDetails.h"
//--------------------------------------

//--------------------------------------
//
#include "foobar/UI/Resources/foo_logitech_lcd.h"
//--------------------------------------

namespace foobar::UI::detail {
    //**************************************************************************
    // CTextOptionsDialogCommon
    //**************************************************************************
    class CTextOptionsDialogCommon :
        public foobar::UI::CVisualisationPrefsDlgT<::TrackDetailsType> {
    private: // Internal Types
        using thisClass = CTextOptionsDialogCommon;
        using baseClass = foobar::UI::CVisualisationPrefsDlgT<::TrackDetailsType>;

        using CString               = Windows::UI::CString;
        using CStringRef            = CString&;
        using CColorSwatchStatic    = Windows::UI::CColorSwatchStatic;
        using CTrackTimeComboHelper = foobar::UI::CSequentialEnumHelperT<TrackTime>;
        using CTrackTimeCombo       = Windows::UI::CEnumComboBoxT<TrackTime, CTrackTimeComboHelper>;

        inline static constexpr const auto TrackDetailsType_Expert{ ::TrackDetailsType::Page1 };

    public: // Public Types
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    protected: // Message Map
        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnUpdateUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled);

        LRESULT OnColorStatic(_In_ UINT uMsg,
                              _In_ WPARAM wParam,
                              _In_ LPARAM lParam,
                              _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled);

    private: //Message Map Helpers
        void UpdateControls();
        void EnableControls() noexcept;

    protected:
        INT m_nIDD{ 0 };

    private: // Data
        auto IsTrackInfoExpertDialog() const noexcept {
            return m_nIDD == IDD_TRACK_INFO_TAB_1_EXPERT;
        }

        void UpdateTrackInfoFmt();

    private:
        CColorSwatchStatic m_BackgroundColor{};
        CTrackTimeCombo    m_TimeCombo_Left{};
        CTrackTimeCombo    m_TimeCombo_Right{};

        CString    m_strText[::TrackDetailsType::count()]{};
        CStringRef m_strTextExpert{ m_strText[TrackDetailsType_Expert] };
    }; // class CTextOptionsDialogCommon
} // namespace foobar::UI::detail

//==============================================================================

namespace foobar::UI {
    //**************************************************************************
    // CTextOptionsDlg
    //**************************************************************************
    class CTextOptionsDlg :
        public Windows::UI::CDialogImpl<CTextOptionsDlg,
                                        foobar::UI::detail::CTextOptionsDialogCommon> {
    private: // Internal Types
        using thisClass  = CTextOptionsDlg;
        using baseClass  = foobar::UI::detail::CTextOptionsDialogCommon;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

    public: // Construction
        CTextOptionsDlg() noexcept { m_nIDD = IDD; };

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum { IDD = IDD_TRACK_INFO_TAB_1, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
        END_MSG_MAP();

        using baseClass::OnInit;
        using baseClass::OnShow;
        using baseClass::OnUpdateUIState;
        using baseClass::OnColorStatic;
        using baseClass::OnCommand;
    };

    //**************************************************************************
    // CTextOptionsExpertDlg
    //**************************************************************************
    class CTextOptionsExpertDlg :
        public Windows::UI::CDialogImpl<CTextOptionsExpertDlg,
                                        foobar::UI::detail::CTextOptionsDialogCommon> {
    private: // Internal Types
        using thisClass  = CTextOptionsExpertDlg;
        using baseClass  = foobar::UI::detail::CTextOptionsDialogCommon;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

    public: // Construction
        CTextOptionsExpertDlg() noexcept { m_nIDD = IDD; };

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum { IDD = IDD_TRACK_INFO_TAB_1_EXPERT, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
        END_MSG_MAP();

        using baseClass::OnInit;
        using baseClass::OnShow;
        using baseClass::OnUpdateUIState;
        using baseClass::OnColorStatic;
        using baseClass::OnCommand;
    };

    //**************************************************************************
    // CFontOptionsDlg
    //**************************************************************************
    class CFontOptionsDlg :
        public Windows::UI::CDialogImpl<CFontOptionsDlg,
                                        CVisualisationPrefsDlgT<::TrackDetailsType>> {
    private: // Internal Types
        using thisClass  = CFontOptionsDlg;
        using baseClass  = foobar::UI::CVisualisationPrefsDlgT<::TrackDetailsType>;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        using CColorSwatchStatic = Windows::UI::CColorSwatchStatic;

        using ScrollMode         = Windows::GDI::ScrollMode;
        using CScrollComboHelper = foobar::UI::CSequentialEnumHelperT<ScrollMode>;
        using CScrollCombo       = Windows::UI::CEnumComboBoxT<ScrollMode, CScrollComboHelper>;

        using VerticalAlign      = Windows::GDI::Text::Align::Vertical;
        using CAlignVComboHelper = foobar::UI::CSequentialEnumHelperT<VerticalAlign>;
        using CAlignVCombo       = Windows::UI::CEnumComboBoxT<VerticalAlign, CAlignVComboHelper>;

        using HorizontalAlign    = Windows::GDI::Text::Align::Horizontal;
        using CAlignHComboHelper = foobar::UI::CSequentialEnumHelperT<HorizontalAlign>;
        using CAlignHCombo       = Windows::UI::CEnumComboBoxT<HorizontalAlign, CAlignHComboHelper>;

    public: // Construction
        CFontOptionsDlg() = default;

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum { IDD = IDD_TRACK_INFO_TAB_2, };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
        END_MSG_MAP();

        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnUpdateUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled);

        LRESULT OnColorStatic(_In_ UINT uMsg,
                              _In_ WPARAM wParam,
                              _In_ LPARAM lParam,
                              _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled);

    private: //Message Map Helpers
        void UpdateControls();
        void EnableControls() noexcept;

    private:
        CColorSwatchStatic m_FontColor   { };
        CScrollCombo       m_ScrollVCombo{ };
        CScrollCombo       m_ScrollHCombo{ };
        CAlignVCombo       m_AlignVCombo { };
        CAlignHCombo       m_AlignHCombo { };
    };

    //**************************************************************************
    // CTrackDetailsDlg
    //**************************************************************************
    class CTrackDetailsDlg :
        public Windows::UI::CDialogImpl<CTrackDetailsDlg,
                                        foobar::UI::CVisualisationPrefsDlgT<::TrackDetailsType>> {
    private: // Internal Types
        using thisClass  = CTrackDetailsDlg;
        using baseClass  = foobar::UI::CVisualisationPrefsDlgT<::TrackDetailsType>;
        using dialogImpl = Windows::UI::CDialogImpl<thisClass, baseClass>;

        using CTextOptionsDlgPtr       = typename foobar::UI::CTextOptionsDlg::Ptr;
        using CTextOptionsExpertDlgPtr = typename foobar::UI::CTextOptionsExpertDlg::Ptr;
        using CFontOptionsDlgPtr       = typename foobar::UI::CFontOptionsDlg::Ptr;
        using CTabCtrl                 = Windows::UI::CTabCtrl;
        using CTabWindow               = Windows::UI::CWindow;

    public: // Construction
        CTrackDetailsDlg() = default;

    public: // Public Types
        using Ptr = Windows::UI::CAutoPtr<thisClass>;
        using baseClass::visualisation_mode;
        using baseClass::visualisation_type;

    public: // Static Members
        static HWND ShowDialog(_In_ HWND hParentWnd,
                               _In_ INT nShowCmd,
                               _In_ LPVOID pData,
                               _In_opt_ HWND hSizerWnd = NULL) {
            ATLASSERT(pData);
            thisClass* pDlg = static_cast<thisClass*>(pData);
            return pDlg->baseClass::ShowDialog<thisClass>(hParentWnd,
                                                          nShowCmd,
                                                          hSizerWnd);
        }

        static decltype(auto) MakeDialog(_In_z_ LPCTSTR szName,
                                         _In_ visualisation_mode eMode,
                                         _In_ visualisation_type eType,
                                         _In_ const SVisPrefConfig& config) {
            return baseClass::MakeDialog<thisClass>(szName, eMode,
                                                    eType, config);
        }

    public: // CDialogImpl Data
        enum {
            IDD           = IDD_TRACK_INFO_TABS_CFG,
            IDD_TABCTRL   = IDC_TRACK_INFO_TABS,
            IDD_TAB_SIZER = IDC_TRACK_INFO_SIZER,
        };

    protected: // Message Map
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_INITDIALOG    , OnInit)
            MESSAGE_HANDLER(WM_SHOWWINDOW    , OnShow)
            MESSAGE_HANDLER(WM_CHANGEUISTATE , OnChangeUIState)
            MESSAGE_HANDLER(WM_UPDATEUISTATE , OnUpdateUIState)
            MESSAGE_HANDLER(WM_COMMAND       , OnCommand)
            NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnTabChange)                     pragma_suppress(26454) // C26454: Arithmetic overflow: '...' operation produces a negative unsigned result at compile time (io.5).
        END_MSG_MAP();

        LRESULT OnInit(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnShow(_In_ UINT uMsg,
                       _In_ WPARAM wParam,
                       _In_ LPARAM lParam,
                       _Inout_ BOOL& bHandled);

        LRESULT OnChangeUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnUpdateUIState(_In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnCommand(_In_ UINT uMsg,
                          _In_ WPARAM wParam,
                          _In_ LPARAM lParam,
                          _Inout_ BOOL& bHandled) noexcept;

        LRESULT OnTabChange(_In_ WPARAM wParam,
                            _In_ LPNMHDR lpnmHdr,
                            _Inout_ BOOL& bHandled);

    private: //Message Map Helpers
        void UpdateControls() noexcept;
        void EnableControls() noexcept;
        void ShowCurrentTab();

        template <class TabDlgT>
        void ShowCurrentTab(_In_ TabDlgT& dialog) {
            if (m_CurrentTab.IsWindow()) {
                if (m_CurrentTab.m_hWnd == dialog->m_hWnd) {
                    m_CurrentTab.ShowWindow(SW_SHOW);
                    return;
                } else {
                    m_CurrentTab.ShowWindow(SW_HIDE);
                    m_CurrentTab.Detach();
                }
            }
            HWND hDlg = dialog->ShowDialog(*this, SW_SHOW, dialog.m_p,
                                           GetDlgItem(IDD_TAB_SIZER));
            m_CurrentTab.Attach(hDlg);
        }

    private:
        bool                     m_bExpertMode       { false };
        CTextOptionsDlgPtr       m_pTextOptions      {};
        CTextOptionsExpertDlgPtr m_pTextOptionsExpert{};
        CFontOptionsDlgPtr       m_pFontOptions      {};
        CTabCtrl                 m_TabCtrl           {};
        CTabWindow               m_CurrentTab        {};
    }; // class CTrackDetailsDlg
} // namespace foobar::UI

#endif // GUID_092C2A34_6D73_49D5_A963_0357BC35E999