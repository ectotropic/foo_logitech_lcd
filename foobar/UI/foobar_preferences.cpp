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
#include "foobar/UI/foobar_preferences.h"
#include "Visualisation/Visualisation_Manager.h"
//--------------------------------------

namespace foobar::UI {
    //**************************************************************************
    // CFoobarPreferencesDlg
    //**************************************************************************

    //--------------------------------------------------------------------------
    // preferences_page_instance overrides
    //--------------------------------------------------------------------------

    t_uint32 CFoobarPreferencesDlg::get_state() noexcept {
        constexpr auto stateDefault = preferences_state::resettable;
        constexpr auto stateChanged = stateDefault | preferences_state::changed;
        return m_bPendingChanges ? stateChanged : stateDefault;
    }

    //------------------------------------------------------

    void CFoobarPreferencesDlg::apply() {
        if (!m_bPendingChanges) { return; }
        m_bPendingChanges = false;
        Config::IConfigManager::instance()->SetConfig(m_CoreConfig);
    }

    //------------------------------------------------------

    void CFoobarPreferencesDlg::reset() {
        m_CoreConfig = Config::IConfigManager::instance()->GetDefault();
        SendMessageToDescendants(WM_UPDATEUISTATE);
        m_bPendingChanges = true;
    }

    //--------------------------------------------------------------------------
    // Message Map Handlers
    //--------------------------------------------------------------------------

    LRESULT CFoobarPreferencesDlg::OnInit(_In_ UINT /*uMsg*/,
                                          _In_ WPARAM /*wParam*/,
                                          _In_ LPARAM /*lParam*/,
                                          _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());

        ATLASSERT(IsDlgItem(IDD_TABCTRL));
        m_TabCtrl.Detach();
        m_TabCtrl.Attach(GetDlgItem(IDD_TABCTRL));
        ATLASSERT(m_TabCtrl.IsWindow());

        m_CoreConfig = Config::IConfigManager::instance()->GetConfig();
        m_Config.pConfig = &m_CoreConfig;

        m_bPendingChanges = false;

        if (auto pVisMan = VisualisationManager::instance_peek()) {
            if (pVisMan->IsRunning()) {
                m_Config.Canvas.bColor  = pVisMan->HasColor();
                m_Config.Canvas.nHeight = pVisMan->GetHeight();
                m_Config.Canvas.nWidth  = pVisMan->GetWidth();
            }
        }

        if (!m_pGeneralOptions) {
            m_pGeneralOptions.Attach(new CGeneralDlg{ m_Config });
        }

        if (!m_pVisualisationOptions) {
            m_pVisualisationOptions.Attach(new CVisualisationsDlg{ m_Config });
        }

        {
            [[maybe_unused]]
            const auto nTab = m_TabCtrl.AddItem(TCIF_TEXT | TCIF_IMAGE,
                                                TEXT("General Options"),
                                                -1, 0);
            WinAPIAssert(nTab >= 0);
        }
        {
            [[maybe_unused]]
            const auto nTab = m_TabCtrl.AddItem(TCIF_TEXT | TCIF_IMAGE,
                                                TEXT("Visualisation Options"),
                                                -1, 0);
            WinAPIAssert(nTab >= 0);
        }
        m_TabCtrl.SetCurSel(0);
        ShowCurrentTab();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CFoobarPreferencesDlg::OnShow(_In_ UINT /*uMsg*/,
                                          _In_ WPARAM wParam,
                                          _In_ LPARAM lParam,
                                          _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        SendMessageToDescendants(WM_SHOWWINDOW, wParam, lParam);
        ShowCurrentTab();
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CFoobarPreferencesDlg::OnChangeUIState(_In_ UINT /*uMsg*/,
                                                   _In_ WPARAM /*wParam*/,
                                                   _In_ LPARAM /*lParam*/,
                                                   _Inout_ BOOL& bHandled) {
        m_bPendingChanges = true;
        m_pCallback->on_state_changed();
        SendMessageToDescendants(WM_UPDATEUISTATE);
        bHandled = TRUE;
        return 0;
    }

    //------------------------------------------------------

    LRESULT CFoobarPreferencesDlg::OnTabChange(_In_ WPARAM /*wParam*/,
                                               _In_ LPNMHDR /*lpnmHdr*/,
                                               _Inout_ BOOL& bHandled) {
        ATLASSERT(IsWindow());
        ShowCurrentTab();
        bHandled = TRUE;
        return 0;
    }

    //--------------------------------------------------------------------------
    // Helpers
    //--------------------------------------------------------------------------

    void CFoobarPreferencesDlg::ShowCurrentTab() {
        ATLASSERT(IsWindow());
        ATLASSERT(m_TabCtrl.IsWindow());
        switch (m_TabCtrl.GetCurSel()) {
            case -1:
                // No Tab Selected
                break;
            case 0:
                ShowCurrentTab(*m_pGeneralOptions);
                break;
            case 1:
                ShowCurrentTab(*m_pVisualisationOptions);
                break;
            DEFAULT_UNREACHABLE;
        }
    }
} // namespace foobar::UI