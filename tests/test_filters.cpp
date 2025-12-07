#include <gtest/gtest.h>
#include "DSP/BiQuadFilter.hpp"
#include "DSP/FilterDesign.hpp"
#include "Effects/FilterEffects.hpp"
#include "Effects/equalizer.hpp"
#include "AudioBuffer.hpp"
#include <cmath>
#include <complex>

using namespace audio;
using namespace audio::dsp;
using namespace audio::effects;

class FilterTest : public ::testing::Test
{
protected:
    static constexpr double SAMPLE_RATE = 44100.0;
    static constexpr double PI = 3.14159265358979323846;

    // Generate sine wave
    AudioBuffer<float> generate_sine(double frequency, double duration, size_t channels = 1)
    {
        size_t num_samples = static_cast<size_t>(SAMPLE_RATE * duration);
        AudioBuffer<float> buffer(num_samples, channels);

        for (size_t i = 0; i < num_samples; ++i)
        {
            float t = static_cast<float>(i) / SAMPLE_RATE;
            float value = std::sin(2.0f * PI * frequency * t);

            for (size_t ch = 0; ch < channels; ++ch)
            {
                buffer(i, ch) = value;
            }
        }

        return buffer;
    }

    // Calculate RMS (root mean square) amplitude
    float calculate_rms(const AudioBuffer<float> &buffer, size_t channel = 0)
    {
        float sum = 0.0f;
        for (size_t i = 0; i < buffer.num_samples(); ++i)
        {
            float sample = buffer(i, channel);
            sum += sample * sample;
        }
        return std::sqrt(sum / buffer.num_samples());
    }
};

// Biquad Coefficient Tests
TEST_F(FilterTest, BiquadCoefficientsNormalize)
{
    BiquadCoefficients coeffs;
    coeffs.b0 = 2.0;
    coeffs.b1 = 4.0;
    coeffs.b2 = 6.0;
    coeffs.a0 = 2.0;
    coeffs.a1 = 8.0;
    coeffs.a2 = 10.0;

    coeffs.normalize();

    EXPECT_DOUBLE_EQ(coeffs.a0, 1.0);
    EXPECT_DOUBLE_EQ(coeffs.b0, 1.0);
    EXPECT_DOUBLE_EQ(coeffs.b1, 2.0);
    EXPECT_DOUBLE_EQ(coeffs.b2, 3.0);
    EXPECT_DOUBLE_EQ(coeffs.a1, 4.0);
    EXPECT_DOUBLE_EQ(coeffs.a2, 5.0);
}

TEST_F(FilterTest, BiquadCoefficientsNormalizeZeroA0)
{
    BiquadCoefficients coeffs;
    coeffs.a0 = 0.0; // Invalid, but should not crash

    coeffs.normalize();

    // Should set a0 to 1.0 to prevent division by zero
    EXPECT_DOUBLE_EQ(coeffs.a0, 1.0);
}

// Filter Design Tests
TEST_F(FilterTest, LowpassCoefficientsValid)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);

    EXPECT_DOUBLE_EQ(coeffs.a0, 1.0); // Should be normalized
    EXPECT_TRUE(std::isfinite(coeffs.b0));
    EXPECT_TRUE(std::isfinite(coeffs.b1));
    EXPECT_TRUE(std::isfinite(coeffs.b2));
    EXPECT_TRUE(std::isfinite(coeffs.a1));
    EXPECT_TRUE(std::isfinite(coeffs.a2));
}

TEST_F(FilterTest, HighpassCoefficientsValid)
{
    auto coeffs = FilterDesign::highpass(SAMPLE_RATE, 1000.0);

    EXPECT_DOUBLE_EQ(coeffs.a0, 1.0);
    EXPECT_TRUE(std::isfinite(coeffs.b0));
}

TEST_F(FilterTest, InvalidFrequencyThrows)
{
    // Frequency above Nyquist
    EXPECT_THROW(
        FilterDesign::lowpass(SAMPLE_RATE, 30000.0),
        std::invalid_argument);

    // Negative frequency
    EXPECT_THROW(
        FilterDesign::lowpass(SAMPLE_RATE, -100.0),
        std::invalid_argument);

    // Zero frequency
    EXPECT_THROW(
        FilterDesign::lowpass(SAMPLE_RATE, 0.0),
        std::invalid_argument);
}

TEST_F(FilterTest, InvalidQFactorThrows)
{
    EXPECT_THROW(
        FilterDesign::lowpass(SAMPLE_RATE, 1000.0, 0.0),
        std::invalid_argument);

    EXPECT_THROW(
        FilterDesign::lowpass(SAMPLE_RATE, 1000.0, -1.0),
        std::invalid_argument);
}

// Biquad Filter Tests
TEST_F(FilterTest, BiquadFilterProcessesSample)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);
    BiquadFilter<float> filter(coeffs);

    float output = filter.process_sample(0.5f, 0);

    EXPECT_TRUE(std::isfinite(output));
}

TEST_F(FilterTest, BiquadFilterReset)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);
    BiquadFilter<float> filter(coeffs);

    // Process some samples
    for (int i = 0; i < 10; ++i)
    {
        filter.process_sample(1.0f, 0);
    }

    filter.reset();

    // After reset, DC input should gradually reach steady state
    float first_output = filter.process_sample(1.0f, 0);
    EXPECT_NE(first_output, 1.0f); // Should not instantly reach 1.0
}

TEST_F(FilterTest, BiquadFilterStereo)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);
    BiquadFilter<float> filter(coeffs);

    auto buffer = generate_sine(440.0, 0.1, 2);
    filter.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());

    // Both channels should be processed independently
    EXPECT_TRUE(std::isfinite(buffer(50, 0)));
    EXPECT_TRUE(std::isfinite(buffer(50, 1)));
}

// Lowpass Filter Effect Tests
TEST_F(FilterTest, LowpassReducesHighFrequencies)
{
    // Generate test signal: 200Hz + 5kHz
    auto low_freq = generate_sine(200.0, 0.1);
    auto high_freq = generate_sine(5000.0, 0.1);

    // Mix them
    for (size_t i = 0; i < low_freq.num_samples(); ++i)
    {
        low_freq(i, 0) = (low_freq(i, 0) + high_freq(i, 0)) * 0.5f;
    }

    float original_rms = calculate_rms(low_freq);

    // Apply lowpass at 1kHz (should remove 5kHz)
    LowpassEffect<float> lpf(SAMPLE_RATE, 1000.0);
    lpf.process(low_freq);

    float filtered_rms = calculate_rms(low_freq);

    // Filtered signal should have lower RMS (5kHz component removed)
    EXPECT_LT(filtered_rms, original_rms * 0.8f);
}

TEST_F(FilterTest, LowpassPassesLowFrequencies)
{
    auto signal = generate_sine(200.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Apply lowpass at 1kHz (should not affect 200Hz much)
    LowpassEffect<float> lpf(SAMPLE_RATE, 1000.0);
    lpf.process(signal);

    float filtered_rms = calculate_rms(signal);

    // Low frequency should pass through mostly unchanged
    EXPECT_NEAR(filtered_rms, original_rms, original_rms * 0.3f);
}

// Highpass Filter Effect Tests
TEST_F(FilterTest, HighpassReducesLowFrequencies)
{
    auto signal = generate_sine(50.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Apply highpass at 200Hz (should remove 50Hz)
    HighpassEffect<float> hpf(SAMPLE_RATE, 200.0);
    hpf.process(signal);

    float filtered_rms = calculate_rms(signal);

    // Low frequency should be significantly attenuated
    EXPECT_LT(filtered_rms, original_rms * 0.5f);
}

TEST_F(FilterTest, HighpassPassesHighFrequencies)
{
    auto signal = generate_sine(5000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Apply highpass at 200Hz (should not affect 5kHz)
    HighpassEffect<float> hpf(SAMPLE_RATE, 200.0);
    hpf.process(signal);

    float filtered_rms = calculate_rms(signal);

    // High frequency should pass through mostly unchanged
    EXPECT_NEAR(filtered_rms, original_rms, original_rms * 0.2f);
}

// Bandpass Filter Effect Tests
TEST_F(FilterTest, BandpassFilterPassesCenterFrequency)
{
    auto signal = generate_sine(1000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Bandpass centered at 1kHz
    BandpassEffect<float> bpf(SAMPLE_RATE, 1000.0, 1.0);
    bpf.process(signal);

    float filtered_rms = calculate_rms(signal);

    // Center frequency should pass through
    EXPECT_GT(filtered_rms, original_rms * 0.3f);
}

TEST_F(FilterTest, BandpassFilterRejectsOutsideFrequencies)
{
    // Test with frequency outside the pass band
    auto signal = generate_sine(5000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Narrow bandpass at 1kHz
    BandpassEffect<float> bpf(SAMPLE_RATE, 1000.0, 0.5);
    bpf.process(signal);

    float filtered_rms = calculate_rms(signal);

    // Frequency outside pass band should be attenuated
    EXPECT_LT(filtered_rms, original_rms * 0.3f);
}

// Parametric EQ Tests
TEST_F(FilterTest, ParametricEQBoost)
{
    auto signal = generate_sine(1000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Boost 1kHz by 6dB
    ParametricEQBand<float> eq(SAMPLE_RATE, 1000.0, 6.0, 1.0);
    eq.process(signal);

    float boosted_rms = calculate_rms(signal);

    // Boosted signal should be louder
    EXPECT_GT(boosted_rms, original_rms);
}

TEST_F(FilterTest, ParametricEQCut)
{
    auto signal = generate_sine(1000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // Cut 1kHz by 6dB
    ParametricEQBand<float> eq(SAMPLE_RATE, 1000.0, -6.0, 1.0);
    eq.process(signal);

    float cut_rms = calculate_rms(signal);

    // Cut signal should be quieter
    EXPECT_LT(cut_rms, original_rms);
}

TEST_F(FilterTest, ParametricEQZeroGain)
{
    auto signal = generate_sine(1000.0, 0.1);
    float original_rms = calculate_rms(signal);

    // 0dB gain should not change signal
    ParametricEQBand<float> eq(SAMPLE_RATE, 1000.0, 0.0, 1.0);
    eq.process(signal);

    float processed_rms = calculate_rms(signal);

    EXPECT_NEAR(processed_rms, original_rms, original_rms * 0.1f);
}

// Multi-band Equalizer Tests
TEST_F(FilterTest, EqualizerAddRemoveBands)
{
    Equalizer<float> eq(SAMPLE_RATE);

    EXPECT_EQ(eq.num_bands(), 0);

    eq.add_band(1000.0, 3.0);
    EXPECT_EQ(eq.num_bands(), 1);

    eq.add_band(2000.0, -6.0);
    EXPECT_EQ(eq.num_bands(), 2);

    eq.remove_band(0);
    EXPECT_EQ(eq.num_bands(), 1);

    eq.clear();
    EXPECT_EQ(eq.num_bands(), 0);
}

TEST_F(FilterTest, EqualizerSetBandParameters)
{
    Equalizer<float> eq(SAMPLE_RATE);

    size_t idx = eq.add_band(1000.0, 0.0);

    eq.set_band_frequency(idx, 2000.0);
    eq.set_band_gain(idx, 6.0);
    eq.set_band_bandwidth(idx, 2.0);

    const auto &band = eq.get_band(idx);
    EXPECT_DOUBLE_EQ(band.frequency, 2000.0);
    EXPECT_DOUBLE_EQ(band.gain_db, 6.0);
    EXPECT_DOUBLE_EQ(band.bandwidth, 2.0);
}

TEST_F(FilterTest, EqualizerEnableDisableBands)
{
    Equalizer<float> eq(SAMPLE_RATE);

    size_t idx = eq.add_band(1000.0, 6.0);

    auto signal = generate_sine(1000.0, 0.1);
    auto original = signal;

    // Process with enabled band
    eq.process(signal);
    float enabled_rms = calculate_rms(signal);

    // Disable band and process again
    signal = original;
    eq.set_band_enabled(idx, false);
    eq.process(signal);
    float disabled_rms = calculate_rms(signal);

    // Disabled band should not affect signal
    EXPECT_NEAR(disabled_rms, calculate_rms(original), 0.01f);
    EXPECT_GT(enabled_rms, disabled_rms);
}

TEST_F(FilterTest, Equalizer5BandPreset)
{
    Equalizer<float> eq(SAMPLE_RATE);
    eq.create_5band_eq();

    EXPECT_EQ(eq.num_bands(), 5);

    // Check that bands are at expected frequencies
    EXPECT_DOUBLE_EQ(eq.get_band(0).frequency, 100.0);
    EXPECT_DOUBLE_EQ(eq.get_band(4).frequency, 10000.0);
}

TEST_F(FilterTest, Equalizer10BandPreset)
{
    Equalizer<float> eq(SAMPLE_RATE);
    eq.create_10band_eq();

    EXPECT_EQ(eq.num_bands(), 10);
}

// Three-Band EQ Tests
TEST_F(FilterTest, ThreeBandEQBassControl)
{
    ThreeBandEQ<float> eq(SAMPLE_RATE);

    auto low_freq = generate_sine(100.0, 0.1);
    float original_rms = calculate_rms(low_freq);

    eq.set_bass(6.0);
    eq.process(low_freq);

    float boosted_rms = calculate_rms(low_freq);

    // Bass boost should increase low frequency amplitude
    EXPECT_GT(boosted_rms, original_rms);
}

TEST_F(FilterTest, ThreeBandEQTrebleControl)
{
    ThreeBandEQ<float> eq(SAMPLE_RATE);

    auto high_freq = generate_sine(8000.0, 0.1);
    float original_rms = calculate_rms(high_freq);

    eq.set_treble(6.0);
    eq.process(high_freq);

    float boosted_rms = calculate_rms(high_freq);

    // Treble boost should increase high frequency amplitude
    EXPECT_GT(boosted_rms, original_rms);
}

TEST_F(FilterTest, ThreeBandEQMidControl)
{
    ThreeBandEQ<float> eq(SAMPLE_RATE);

    auto mid_freq = generate_sine(1000.0, 0.1);
    float original_rms = calculate_rms(mid_freq);

    eq.set_mid(6.0);
    eq.process(mid_freq);

    float boosted_rms = calculate_rms(mid_freq);

    EXPECT_GT(boosted_rms, original_rms);
}

TEST_F(FilterTest, ThreeBandEQCombined)
{
    ThreeBandEQ<float> eq(SAMPLE_RATE);

    auto signal = generate_sine(1000.0, 0.1);

    eq.set_bass(3.0);
    eq.set_mid(-2.0);
    eq.set_treble(4.0);

    EXPECT_NO_THROW(eq.process(signal));
}

// Filter stability tests
TEST_F(FilterTest, FilterStabilityDCInput)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);
    BiquadFilter<float> filter(coeffs);

    // Process DC signal (all 1.0)
    for (int i = 0; i < 1000; ++i)
    {
        float output = filter.process_sample(1.0f, 0);
        EXPECT_TRUE(std::isfinite(output));
        EXPECT_LE(std::abs(output), 2.0f); // Should not blow up
    }
}

TEST_F(FilterTest, FilterStabilityImpulse)
{
    auto coeffs = FilterDesign::lowpass(SAMPLE_RATE, 1000.0);
    BiquadFilter<float> filter(coeffs);

    // Impulse response
    float impulse = filter.process_sample(1.0f, 0);
    EXPECT_TRUE(std::isfinite(impulse));

    // Following samples should decay
    for (int i = 0; i < 100; ++i)
    {
        float output = filter.process_sample(0.0f, 0);
        EXPECT_TRUE(std::isfinite(output));
    }
}
