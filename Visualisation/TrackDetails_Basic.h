#ifndef GUID_CBDE1098_8CDF_4BB6_B94D_920D9F081478
#define GUID_CBDE1098_8CDF_4BB6_B94D_920D9F081478

//--------------------------------------
//
#include "Visualisation/TrackDetails.h"
//--------------------------------------

//--------------------------------------
//
#include "GDI_Text.h"
#include "GDI_ProgressBar.h"
//--------------------------------------

namespace Visualisation::Text {
    //**************************************************************************
    // TrackDetails
    //**************************************************************************
    class TrackDetails : public ITrackDetails {
    public:
        using track_info_type = ::Windows::GDI::CText;
        using progress_bar_type = ::Windows::GDI::CProgressBar;

    public:
        TrackDetails(page_type ePage,
                     const config_type& config,
                     const dimensions_type& dim,
                     bool bMonochrome);

    public: //Interface overrides
        virtual void Activate(request_param_type& params,
                              const audio_data_manager_type& AudioDataManager) override;
        virtual void Deactivate() noexcept override;

        virtual void Update(const audio_data_manager_type& AudioDataManager) override;
        virtual void Draw(render_pass_type ePass,
                          const audio_data_manager_type& AudioDataManager,
                          float fInterp) override;

    private: //Methods
        void UpdateText(const audio_data_manager_type& AudioDataManager);
        void UpdateProgress(const audio_data_manager_type& AudioDataManager);

    //public:
    //    static const auto Type() noexcept { return ETrackDetailsPage::TRACK_INFO_PAGE_1; }

    private: //Data
        track_info_type m_TrackDetails;
        progress_bar_type m_Progress;
        bool m_bShowProgress;

        int m_nMaxLines;
    }; // class TrackDetails
} // namespace Visualisation::Text

#endif // GUID_CBDE1098_8CDF_4BB6_B94D_920D9F081478