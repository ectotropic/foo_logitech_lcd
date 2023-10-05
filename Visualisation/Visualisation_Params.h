#ifndef GUID_4C32CE58_21E1_48FE_970E_6C382873E729
#define GUID_4C32CE58_21E1_48FE_970E_6C382873E729

//--------------------------------------
//
#include "Util/FlagEnum.h"
#include "Color.h"
//--------------------------------------

//--------------------------------------
//
#include <functional>
#include <cstddef>
#include <utility>
//--------------------------------------

namespace Visualisation {
    //**************************************************************************
    // DataType
    //**************************************************************************
    FLAG_ENUM(DataType,
              FLAG_ENUM_VALUES(None             = 0,
                               Spectrum         = 1 << 0,
                               CombinedSpectrum = 1 << 1,
                               Waveform         = 1 << 2,
                               CombinedWaveform = 1 << 3,
                               Decibel          = 1 << 4,
                               CombinedDecibel  = 1 << 5,
                               TrackDetails     = 1 << 6));

    //**************************************************************************
    // SpectrumParams
    //**************************************************************************
    struct SpectrumParams final {
    public:
#if SIZE_MAX < UINT64_MAX
        using sample_type = float;
#else
        using sample_type = double;
#endif
        using peak_type   = sample_type;
        using size_type   = std::size_t;

        // TODO: Using `std::function` here is far from
        //       ideal, but it's hard to find a suitable
        //       alternative.
        //       The costs of  `std::function` are likely
        //       to be significant and this will be called
        //       frequently.
        using transform_type =
            std::function<sample_type(size_type, sample_type)>;

    private:
        template <typename TypeT>
        static [[nodiscard]]
        auto exchange_zero(TypeT& val) noexcept
            -> decltype(std::exchange(val, static_cast<TypeT>(0))) {
            return std::exchange(val, static_cast<TypeT>(0));
        }

    public:
        SpectrumParams() noexcept = default;
        SpectrumParams(size_type sampleCountHint,
                       peak_type peakMininum = 0,
                       peak_type peakDecayRate = 0,
                       transform_type transform_type = {}) noexcept :
            nSampleCountHint{ sampleCountHint },
            fPeakMininum    { peakMininum },
            fPeakDecayRate  { peakDecayRate },
            fnTransform     { transform_type } {}

        SpectrumParams(const SpectrumParams& other) noexcept :
            nSampleCountHint{ other.nSampleCountHint },
            fPeakMininum    { other.fPeakMininum },
            fPeakDecayRate  { other.fPeakDecayRate },
            fnTransform     { other.fnTransform } {}

        SpectrumParams(SpectrumParams&& other) noexcept :
            nSampleCountHint{ exchange_zero(other.nSampleCountHint) },
            fPeakMininum    { exchange_zero(other.fPeakMininum) },
            fPeakDecayRate  { exchange_zero(other.fPeakDecayRate) },
            fnTransform     { std::move(other.fnTransform) } {}

        SpectrumParams& operator=(const SpectrumParams& other) noexcept {
            nSampleCountHint = other.nSampleCountHint;
            fPeakMininum     = other.fPeakMininum;
            fPeakDecayRate   = other.fPeakDecayRate;
            fnTransform      = other.fnTransform;
            return *this;
        }

        SpectrumParams& operator=(SpectrumParams&& other) noexcept {
            nSampleCountHint = exchange_zero(other.nSampleCountHint);
            fPeakMininum     = exchange_zero(other.fPeakMininum);
            fPeakDecayRate   = exchange_zero(other.fPeakDecayRate);
            fnTransform      = std::move(other.fnTransform);
            return *this;
        }

    public:
        size_type      nSampleCountHint{ 0 };
        peak_type      fPeakMininum    { 0 };
        peak_type      fPeakDecayRate  { 0 };
        transform_type fnTransform     { };
    }; // struct SpectrumParams final

    //**************************************************************************
    // WaveformParams
    //**************************************************************************
    struct WaveformParams final {
    public:
#if SIZE_MAX < UINT64_MAX
        using sample_type = float;
#else
        using sample_type = double;
#endif
        using peak_type   = sample_type;
        using size_type   = std::size_t;

        // TODO: Using `std::function` here is far from
        //       ideal, but it's hard to find a suitable
        //       alternative.
        //       The costs of  `std::function` are likely
        //       to be significant and this will be called
        //       frequently.
        using transform_type =
            std::function<sample_type(size_type, sample_type)>;

    private:
        template <typename TypeT>
        static decltype(auto) exchange_zero(TypeT& val) noexcept {
            return std::exchange(val, static_cast<TypeT>(0));
        }

    public:
        WaveformParams() noexcept = default;
        WaveformParams(size_type sampleCountHint,
                       sample_type peakMininum = 0,
                       peak_type peakDecayRate = 0,
                       transform_type transform_type = {}) noexcept :
            nSampleCountHint{ sampleCountHint },
            fPeakMininum    { peakMininum },
            fPeakDecayRate  { peakDecayRate },
            fnTransform     { transform_type } {}

        WaveformParams(const WaveformParams& other) noexcept :
            nSampleCountHint{ other.nSampleCountHint },
            fPeakMininum    { other.fPeakMininum },
            fPeakDecayRate  { other.fPeakDecayRate },
            fnTransform     { other.fnTransform } {}

        WaveformParams(WaveformParams&& other) noexcept :
            nSampleCountHint{ exchange_zero(other.nSampleCountHint) },
            fPeakMininum    { exchange_zero(other.fPeakMininum) },
            fPeakDecayRate  { exchange_zero(other.fPeakDecayRate) },
            fnTransform     { std::move(other.fnTransform) } {}

        WaveformParams& operator=(const WaveformParams& other) noexcept {
            nSampleCountHint = other.nSampleCountHint;
            fPeakMininum     = other.fPeakMininum;
            fPeakDecayRate   = other.fPeakDecayRate;
            fnTransform      = other.fnTransform;
            return *this;
        }

        WaveformParams& operator=(WaveformParams&& other) noexcept {
            nSampleCountHint = exchange_zero(other.nSampleCountHint);
            fPeakMininum     = exchange_zero(other.fPeakMininum);
            fPeakDecayRate   = exchange_zero(other.fPeakDecayRate);
            fnTransform      = std::move(other.fnTransform);
            return *this;
        }

    public:
        size_type      nSampleCountHint{ 0 };
        peak_type      fPeakMininum    { 0 };
        peak_type      fPeakDecayRate  { 0 };
        transform_type fnTransform     {};
    }; // struct WaveformParams final

    //**************************************************************************
    // DecibelParams
    //**************************************************************************
    struct DecibelParams final {
    public:
#if SIZE_MAX < UINT64_MAX
        using dB_type = float;
#else
        using dB_type = double;
#endif
        using decibel_type = dB_type;
        using peak_type    = dB_type;
        using size_type    = std::size_t;

        // TODO: Using `std::function` here is far from
        //       ideal, but it's hard to find a suitable
        //       alternative.
        //       The costs of  `std::function` are likely
        //       to be significant and this will be called
        //       frequently.
        using transform_type =
            std::function<dB_type(size_type, dB_type)>;
    private:
        template <typename TypeT>
        static decltype(auto) exchange_zero(TypeT& val) noexcept {
            return std::exchange(val, static_cast<TypeT>(0));
        }

    public:
        DecibelParams() noexcept = default;
        DecibelParams(peak_type peakMininum,
                      peak_type peakDecayRate,
                      transform_type transform_type = {}) noexcept :
            fPeakMininum  { peakMininum },
            fPeakDecayRate{ peakDecayRate },
            fnTransform   { transform_type } {}

        DecibelParams(const DecibelParams& other) noexcept :
            fPeakMininum  { other.fPeakMininum },
            fPeakDecayRate{ other.fPeakDecayRate },
            fnTransform   { other.fnTransform } {}

        DecibelParams(DecibelParams&& other) noexcept :
            fPeakMininum  { exchange_zero(other.fPeakMininum) },
            fPeakDecayRate{ exchange_zero(other.fPeakDecayRate) },
            fnTransform   { std::move(other.fnTransform) } {}

        DecibelParams& operator=(const DecibelParams& other) noexcept {
            fPeakDecayRate = other.fPeakDecayRate;
            fnTransform    = other.fnTransform;
            return *this;
        }

        DecibelParams& operator=(DecibelParams&& other) noexcept {
            fPeakMininum   = exchange_zero(other.fPeakMininum);
            fPeakDecayRate = exchange_zero(other.fPeakDecayRate);
            fnTransform    = std::move(other.fnTransform);
            return *this;
        }

    public:
        peak_type      fPeakMininum  { 0 };
        peak_type      fPeakDecayRate{ 0 };
        transform_type fnTransform   { };
    }; // struct DecibelParams final

    //**************************************************************************
    // RequestParams
    //**************************************************************************
    struct RequestParams final {
    public:
        using spectrum_param_type = SpectrumParams;
        using waveform_param_type = WaveformParams;
        using decibel_param_type  = DecibelParams;
        using color_type          = Color::Color3f;
        using want_type           = DataType;
        using size_type           = std::size_t;

        using spectrum_sample_type    = typename spectrum_param_type::sample_type;
        using spectrum_peak_type      = typename spectrum_param_type::peak_type;
        using spectrum_transform_type = typename spectrum_param_type::transform_type;

        using waveform_sample_type    = typename waveform_param_type::sample_type;
        using waveform_peak_type      = typename waveform_param_type::peak_type;
        using waveform_transform_type = typename waveform_param_type::transform_type;

        using dB_type           = typename decibel_param_type::dB_type;
        using decibel_type      = typename decibel_param_type::decibel_type;
        using dB_peak_type      = typename decibel_param_type::peak_type;
        using dB_transform_type = typename decibel_param_type::transform_type;

    public:
        RequestParams() noexcept = default;

        RequestParams(const spectrum_param_type& spec,
                      const color_type& primColor = {},
                      const color_type& clearColor = {}) noexcept :
            Spectrum  { spec },
            PrimaryColor{ primColor },
            ClearColor{ clearColor },
            Want      { want_type::Spectrum | want_type::CombinedSpectrum } {}

        RequestParams(const waveform_param_type& spec,
                      const color_type& primColor = {},
                      const color_type& clearColor = {}) noexcept :
            Waveform  { spec },
            PrimaryColor{ primColor },
            ClearColor{ clearColor },
            Want      { want_type::Waveform | want_type::CombinedWaveform } {}

        RequestParams(const decibel_param_type& dB,
                      const color_type& primColor = {},
                      const color_type& clearColor = {}) noexcept :
            Decibel   { dB },
            PrimaryColor{ primColor },
            ClearColor{ clearColor },
            Want      { want_type::Decibel | want_type::CombinedDecibel } {}

        RequestParams(size_type sampleCountHint,
                      const color_type& primColor = {},
                      const color_type& clearColor = {}) noexcept :
            Spectrum  { sampleCountHint },
            Waveform  { sampleCountHint },
            PrimaryColor{ primColor },
            ClearColor{ clearColor } {}

        RequestParams(const RequestParams& other) noexcept :
            Spectrum  { other.Spectrum },
            Waveform  { other.Waveform },
            Decibel   { other.Decibel },
            ClearColor{ other.ClearColor },
            Want      { other.Want } {}

        RequestParams(RequestParams&& other) noexcept :
            Spectrum  { std::move(other.Spectrum) },
            Waveform  { std::move(other.Waveform) },
            Decibel   { std::move(other.Decibel) },
            ClearColor{ std::move(other.ClearColor) },
            Want      { std::exchange(other.Want, want_type::None) } {}

        RequestParams& operator=(const RequestParams& other) noexcept {
            Spectrum   = other.Spectrum;
            Waveform   = other.Waveform;
            Decibel    = other.Decibel;
            ClearColor = other.ClearColor;
            Want       = other.Want;
            return *this;
        }

        RequestParams& operator=(RequestParams&& other) noexcept {
            Spectrum   = std::move(other.Spectrum);
            Waveform   = std::move(other.Waveform);
            Decibel    = std::move(other.Decibel);
            ClearColor = std::move(other.ClearColor);
            Want       = std::exchange(other.Want, want_type::None);
            return *this;
        }

    public:
        want_type           Want        { want_type::None };
        color_type          PrimaryColor{ };
        color_type          ClearColor  { };
        spectrum_param_type Spectrum    { };
        waveform_param_type Waveform    { };
        decibel_param_type  Decibel     { };
    }; // struct RequestParams final
} // namespace Visualisation

#endif // GUID_4C32CE58_21E1_48FE_970E_6C382873E729