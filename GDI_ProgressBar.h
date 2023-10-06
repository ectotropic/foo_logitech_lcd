#pragma once
#ifndef GUID_06CED4B1_AF66_4106_BB70_B02BAD2DF542
#define GUID_06CED4B1_AF66_4106_BB70_B02BAD2DF542
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

//--------------------------------------
//
#include "GDI_Drawable.h"
//--------------------------------------

//--------------------------------------
//
#include "Util/SequentialEnum.h"
//--------------------------------------

namespace Windows::GDI {
    //**************************************************************************
    // ProgressText
    //**************************************************************************
    SEQUENTIAL_NAMED_ENUM(ProgressText,
                          SEQUENTIAL_ENUM_VALUES(SEQUENTIAL_ENUM_FIRST(None, 0),
                                                 TotalTime,
                                                 ElapsedPercent,
                                                 RemainingPercent,
                                                 ElapsedTime,
                                                 RemainingTime,
                                                 SystemTime,
                                                 SystemDate),
                          SEQUENTIAL_NAMED_ENUM_STRINGS(L"None",
                                                        L"Total Time",
                                                        L"Elapsed Percent",
                                                        L"Remaining Percent",
                                                        L"Elapsed Time",
                                                        L"Remaining Time",
                                                        L"System Time",
                                                        L"System Date"));

    //**************************************************************************
    // CProgressBar
    //**************************************************************************
    class CProgressBar : public CDrawable {
    private:
        using thisClass = CProgressBar;
        using baseClass = CDrawable;

        class CProgress {
        public:
            constexpr auto   Start() const noexcept { return m_fStart; }
            constexpr auto  Finish() const noexcept { return m_fFinish; }
            constexpr auto Current() const noexcept { return m_fCurrent; }

            constexpr auto   Total() const noexcept { return Finish() - Start(); }
            constexpr auto Elapsed() const noexcept { return Current() - Start(); }
            constexpr auto  Remain() const noexcept { return Total() - Current(); }

            constexpr auto ElapsedFraction() const noexcept {
                const auto fract = Elapsed() / Total();
                if (fract > 1.f) { return 1.f; }
                if (fract < 0.f) { return 0.f; }
                return fract;
            }

            constexpr auto ElapsedPercent() const noexcept { return ElapsedFraction() * 100.f; }

            constexpr auto RemainFraction() const noexcept {
                const auto fract = Remain() / Total();
                if (fract > 1.f) { return 1.f; }
                if (fract < 0.f) { return 0.f; }
                return fract;
            }

            constexpr auto RemainPercent() const noexcept { return RemainFraction() * 100.f; }

            constexpr bool Range(_In_ float fStart,
                                 _In_ float fFinish) noexcept {
                if ((m_fStart == fStart) &&
                    (m_fFinish == fFinish)) {
                    return false;
                }
                m_fCurrent = m_fStart = fStart;
                m_fFinish = fFinish;
                return true;
            }

            constexpr bool Current(_In_ float fCurrent) noexcept {
                if (m_fCurrent == fCurrent) { return false; }
                m_fCurrent = std::min(std::max(fCurrent, Start()), Finish());
                return true;
            }

            constexpr bool Fraction(_In_ float fFraction) noexcept {
                assert(fFraction >= 0.f && fFraction <= 1.f);
                return Current(Start() + (Total() * fFraction));
            }

        private:
            float m_fStart{ 0.f };
            float m_fFinish{ 0.f };
            float m_fCurrent{ 0.f };
        };

    public:
        CProgressBar(_In_ HDC hDC);
        virtual ~CProgressBar() noexcept = default;

        CProgressBar(const CProgressBar&) = delete;
        CProgressBar operator= (const CProgressBar&) = delete;

        void Update(_In_ float fProgress);

        void Range(_In_ float fMin,
                   _In_ float fMax);
        void SetTextMode(_In_ ProgressText eLeft,
                         _In_ ProgressText eRight);
        bool SetFont(_In_ const LOGFONT& logFont) ;
        bool SetFont(_In_z_ LPCTSTR szFamily,
                     _In_ int iHeight);

        constexpr void SetClear(_In_ bool bClear) noexcept { m_bClear = bClear; }

    private:
        virtual void OnDraw(float fInterp) noexcept override;
        virtual void OnChanged(_In_ GDIChange eWhat,
                               _In_ LPARAM lparam,
                               _In_ WPARAM wparam) override;

    private:
        void UpdateBounds();
        void UpdateProgress();
        int CalcTextSize(ProgressText eMode);

    private:
        CProgress m_Progress;
        CRect m_OutlineRect{ 0, 0, 0, 0 };
        CRect m_FillRect{ 0, 0, 0, 0 };

        struct {
            CFont font{};
            struct : public TEXTMETRIC {
                int tmDateStringWidth{ 0 };
                int tmTimeStringWidth{ 0 };
                int tmPercentStringWidth{ 0 };
            } Metrics;
        } m_Font;

        enum { TextLeft = 0, TextRight, TextCount };
        struct {
            void Draw(HDC hDC, UINT uFormat) const noexcept {
                WinAPIVerify(::DrawText(hDC,
                                        m_strText.GetString(), -1,
                                        (LPRECT)&m_Bounds,
                                        uFormat | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE));
            }

            ProgressText m_eMode{ ProgressText::None };
            CRect m_Bounds{ 0, 0, 0, 0 };
            CString m_strText{};
        } m_Text[TextCount];

        CString m_TimeFormat{};

        bool m_bClear{ false };
    }; // class CProgressBar
} // namespace Windows::GDI

#endif // GUID_06CED4B1_AF66_4106_BB70_B02BAD2DF542