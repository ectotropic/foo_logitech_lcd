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
#include "foobar/UI/foobar_pref_visualisation.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CVisualisationsDlg
    //**************************************************************************
    void CVisualisationsDlg::BuildTree() {
        HTREEITEM hBranch = NULL;

        hBranch = InsertTreeBranch(TEXT("Mode 1"));
        {
            for (const auto s : SpectrumAnalyserType{}) {
                switch (s) {
                    case SpectrumAnalyserType::Mono:
                        [[fallthrough]];
                    case SpectrumAnalyserType::Stereo: {
                        auto pDlg = CSpectrumAnalyserDlg::MakeDialog(s.to_string(),
                                                                     VisualisationMode::SpectrumAnalyser,
                                                                     s, Config());

                        InsertTreeLeaf(hBranch, pDlg);
                        m_Spectrum[s] = pDlg;
                        break;
                    }

                    case SpectrumAnalyserType::MonoBlock:
                        [[fallthrough]];
                    case SpectrumAnalyserType::StereoBlock: {
                        auto pDlg = CBlockSpecDialog::MakeDialog(s.to_string(),
                                                                 VisualisationMode::SpectrumAnalyser,
                                                                 s, Config());
                        InsertTreeLeaf(hBranch, pDlg);
                        m_Spectrum[s] = pDlg;
                        break;
                    }

                    DEFAULT_UNREACHABLE;
                }
            }
        }
        m_TreeControl.Expand(hBranch);

        hBranch = InsertTreeBranch(TEXT("Mode 2"));
        {
            for (const auto o : OscilloscopeType{}) {
                auto pDlg = COscilloscopeDlg::MakeDialog(o.to_string(),
                                                         VisualisationMode::Oscilloscope,
                                                         o, Config());
                InsertTreeLeaf(hBranch, pDlg);
                m_Oscilloscope[o] = pDlg;
            }
        }
        m_TreeControl.Expand(hBranch);

        hBranch = InsertTreeBranch(TEXT("Mode 3"));
        {
            const VisualisationMode eVisMode = VisualisationMode::VUMeter;
            for (const auto v : VUMeterType{}) {
                switch (v) {
                    case VUMeterType::Meter1:
                        [[fallthrough]];
                    case VUMeterType::Meter2: {
                        auto pDlg = CVUMeterDlg::MakeDialog(v.to_string(),
                                                            VisualisationMode::VUMeter,
                                                            v, Config());
                        InsertTreeLeaf(hBranch, pDlg);
                        m_VUMeter[v] = pDlg;
                        break;
                    }

                    case VUMeterType::Image: {
                        auto pDlg = CVUMeterImageDlg::MakeDialog(v.to_string(),
                                                                 VisualisationMode::VUMeter,
                                                                 v, Config());
                        InsertTreeLeaf(hBranch, pDlg);
                        m_VUMeter[v] = pDlg;
                        break;
                    }

                    DEFAULT_UNREACHABLE;
                }
            }
        }
        m_TreeControl.Expand(hBranch);

        hBranch = InsertTreeBranch(TEXT("Mode 4"));
        {
            for (const auto t : TrackDetailsType{}) {
                auto pDlg = CTrackDetailsDlg::MakeDialog(t.to_string(),
                                                         VisualisationMode::TrackDetails,
                                                         t, Config());
                InsertTreeLeaf(hBranch, pDlg);
                m_TrackDetails[t] = pDlg;
            }
        }
        m_TreeControl.Expand(hBranch);
    }

    //------------------------------------------------------

    LRESULT CVisualisationsDlg::OnInit(_In_ UINT /*uMsg*/,
                                       _In_ WPARAM /*wParam*/,
                                       _In_ LPARAM /*lParam*/,
                                       _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDC_VIS_TREE));
        m_TreeControl.Detach();
        m_TreeControl.Attach(GetDlgItem(IDC_VIS_TREE));
        ATLASSERT(m_TreeControl.IsWindow());

        BuildTree();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CVisualisationsDlg::OnShow(_In_ UINT /*uMsg*/,
                                       _In_ WPARAM wParam,
                                       _In_ LPARAM lParam,
                                       _Inout_ BOOL& bHandled) noexcept {
        ATLASSERT(IsWindow());
        // WM_SHOWWINDOW is not propagated automatically,
        // but is used by children to ensure dialogs are
        // up-to-date
        SendMessageToDescendants(WM_SHOWWINDOW, wParam, lParam);
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CVisualisationsDlg::OnChangeUIState(_In_ UINT /*uMsg*/,
                                                _In_ WPARAM /*wParam*/,
                                                _In_ LPARAM /*lParam*/,
                                                _Inout_ BOOL& bHandled) noexcept {
        SendMessageToParent(WM_CHANGEUISTATE);
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CVisualisationsDlg::OnSelectionChanged(_In_ INT /*iCtrlID*/,
                                                   _In_ LPNMHDR pNotifyDesc,
                                                   _Inout_ BOOL& bHandled) {
        ATLASSERT(pNotifyDesc);
        const LPNMTREEVIEW pSelectionDesc = (LPNMTREEVIEW)(pNotifyDesc);
        ShowTreeItem(pSelectionDesc->itemNew.hItem);
        bHandled = TRUE;
        return 0;
    }
} // namespace foobar::UI
