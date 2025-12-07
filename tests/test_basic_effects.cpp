#include <gtest/gtest.h>
#include "Effects/BasicEffects.hpp"
#include "AudioBuffer.hpp"
#include <cmath>

using namespace audio;
using namespace audio::effects;

class BasicEffectsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test buffer with known values
        buffer_ = AudioBuffer<float>(100, 2);
        for (size_t i = 0; i < 100; ++i)
        {
            buffer_(i, 0) = 0.5f;
            buffer_(i, 1) = -0.3f;
        }
    }

    AudioBuffer<float> buffer_;
};

// Gain Effect Tests
TEST_F(BasicEffectsTest, GainEffectUnityGain)
{
    GainEffect<float> gain(1.0f);

    AudioBuffer<float> test = buffer_;
    gain.process(test);

    // Unity gain should not change values
    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_FLOAT_EQ(test(i, 0), 0.5f);
        EXPECT_FLOAT_EQ(test(i, 1), -0.3f);
    }
}

TEST_F(BasicEffectsTest, GainEffectDoubleAmplitude)
{
    GainEffect<float> gain(2.0f);

    AudioBuffer<float> test = buffer_;
    gain.process(test);

    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_FLOAT_EQ(test(i, 0), 1.0f);
        EXPECT_FLOAT_EQ(test(i, 1), -0.6f);
    }
}

TEST_F(BasicEffectsTest, GainEffectHalfAmplitude)
{
    GainEffect<float> gain(0.5f);

    AudioBuffer<float> test = buffer_;
    gain.process(test);

    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_FLOAT_EQ(test(i, 0), 0.25f);
        EXPECT_FLOAT_EQ(test(i, 1), -0.15f);
    }
}

TEST_F(BasicEffectsTest, GainEffectZeroGain)
{
    GainEffect<float> gain(0.0f);

    AudioBuffer<float> test = buffer_;
    gain.process(test);

    // Zero gain should produce silence
    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_FLOAT_EQ(test(i, 0), 0.0f);
        EXPECT_FLOAT_EQ(test(i, 1), 0.0f);
    }
}

TEST_F(BasicEffectsTest, GainEffectDecibelConversion)
{
    GainEffect<float> gain;

    // +6 dB should double amplitude
    gain.set_gain_db(6.0f);
    EXPECT_NEAR(gain.gain_linear(), 2.0f, 0.01f);

    // -6 dB should halve amplitude
    gain.set_gain_db(-6.0f);
    EXPECT_NEAR(gain.gain_linear(), 0.5f, 0.01f);

    // 0 dB should be unity gain
    gain.set_gain_db(0.0f);
    EXPECT_NEAR(gain.gain_linear(), 1.0f, 0.01f);
}

TEST_F(BasicEffectsTest, GainEffectDisabled)
{
    GainEffect<float> gain(0.0f);
    gain.set_enabled(false);

    AudioBuffer<float> test = buffer_;
    gain.process(test);

    // Disabled effect should not modify buffer
    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_FLOAT_EQ(test(i, 0), 0.5f);
        EXPECT_FLOAT_EQ(test(i, 1), -0.3f);
    }
}

// Fade Effect Tests
TEST_F(BasicEffectsTest, FadeInEffect)
{
    FadeEffect<float> fade(44100.0, 0.001, FadeEffect<float>::Type::FadeIn); // 1ms fade

    AudioBuffer<float> test(44, 1); // ~1ms at 44.1kHz
    for (size_t i = 0; i < 44; ++i)
    {
        test(i, 0) = 1.0f;
    }

    fade.process(test);

    // First sample should be near zero
    EXPECT_NEAR(test(0, 0), 0.0f, 0.1f);

    // Last sample should be near original value
    EXPECT_NEAR(test(43, 0), 1.0f, 0.1f);

    // Middle should be around 0.5
    EXPECT_NEAR(test(22, 0), 0.5f, 0.2f);
}

TEST_F(BasicEffectsTest, FadeOutEffect)
{
    FadeEffect<float> fade(44100.0, 0.001, FadeEffect<float>::Type::FadeOut);

    AudioBuffer<float> test(44, 1);
    for (size_t i = 0; i < 44; ++i)
    {
        test(i, 0) = 1.0f;
    }

    fade.process(test);

    // First sample should be near original
    EXPECT_NEAR(test(0, 0), 1.0f, 0.1f);

    // Last sample should be near zero
    EXPECT_NEAR(test(43, 0), 0.0f, 0.1f);
}

TEST_F(BasicEffectsTest, FadeCustomRange)
{
    FadeEffect<float> fade(44100.0, 0.001);
    fade.set_custom_range(0.2f, 0.8f);

    AudioBuffer<float> test(44, 1);
    for (size_t i = 0; i < 44; ++i)
    {
        test(i, 0) = 1.0f;
    }

    fade.process(test);

    // Should fade from 0.2 to 0.8
    EXPECT_NEAR(test(0, 0), 0.2f, 0.1f);
    EXPECT_NEAR(test(43, 0), 0.8f, 0.1f);
}

TEST_F(BasicEffectsTest, FadeEffectStereo)
{
    FadeEffect<float> fade(44100.0, 0.001, FadeEffect<float>::Type::FadeIn);

    AudioBuffer<float> test(44, 2);
    for (size_t i = 0; i < 44; ++i)
    {
        test(i, 0) = 1.0f;
        test(i, 1) = -1.0f;
    }

    fade.process(test);

    // Both channels should be faded equally
    EXPECT_NEAR(test(0, 0), 0.0f, 0.1f);
    EXPECT_NEAR(test(0, 1), 0.0f, 0.1f);
}

// Mix Effect Tests
TEST_F(BasicEffectsTest, MixEffectAdd)
{
    MixEffect<float> mixer(MixEffect<float>::MixMode::Add, 1.0f);

    AudioBuffer<float> dest(10, 1);
    AudioBuffer<float> source(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        dest(i, 0) = 0.3f;
        source(i, 0) = 0.2f;
    }

    mixer.mix_with(dest, source);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(dest(i, 0), 0.5f);
    }
}

TEST_F(BasicEffectsTest, MixEffectAverage)
{
    MixEffect<float> mixer(MixEffect<float>::MixMode::Average, 1.0f);

    AudioBuffer<float> dest(10, 1);
    AudioBuffer<float> source(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        dest(i, 0) = 0.4f;
        source(i, 0) = 0.2f;
    }

    mixer.mix_with(dest, source);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(dest(i, 0), 0.3f); // (0.4 + 0.2) / 2
    }
}

TEST_F(BasicEffectsTest, MixEffectWithGain)
{
    MixEffect<float> mixer(MixEffect<float>::MixMode::Add, 0.5f);

    AudioBuffer<float> dest(10, 1);
    AudioBuffer<float> source(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        dest(i, 0) = 0.5f;
        source(i, 0) = 0.4f;
    }

    mixer.mix_with(dest, source);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(dest(i, 0), 0.7f); // 0.5 + 0.4 * 0.5
    }
}

TEST_F(BasicEffectsTest, MixEffectDimensionMismatch)
{
    MixEffect<float> mixer;

    AudioBuffer<float> dest(10, 1);
    AudioBuffer<float> source(5, 1); // Different size

    EXPECT_THROW(mixer.mix_with(dest, source), std::invalid_argument);
}

TEST_F(BasicEffectsTest, MixEffectStereoToMono)
{
    MixEffect<float> mixer(MixEffect<float>::MixMode::StereoToMono);

    AudioBuffer<float> stereo(10, 2);
    for (size_t i = 0; i < 10; ++i)
    {
        stereo(i, 0) = 0.6f;
        stereo(i, 1) = 0.4f;
    }

    mixer.process(stereo);

    // Both channels should now have the average
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(stereo(i, 0), 0.5f);
        EXPECT_FLOAT_EQ(stereo(i, 1), 0.5f);
    }
}

// Pan Effect Tests
TEST_F(BasicEffectsTest, PanEffectCenter)
{
    PanEffect<float> pan(0.0f); // Center

    AudioBuffer<float> test(10, 2);
    for (size_t i = 0; i < 10; ++i)
    {
        test(i, 0) = 1.0f;
        test(i, 1) = 1.0f;
    }

    pan.process(test);

    // Center pan should maintain relative balance
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_NEAR(test(i, 0), test(i, 1), 0.01f);
    }
}

TEST_F(BasicEffectsTest, PanEffectFullLeft)
{
    PanEffect<float> pan(-1.0f); // Full left

    AudioBuffer<float> test(10, 2);
    for (size_t i = 0; i < 10; ++i)
    {
        test(i, 0) = 1.0f;
        test(i, 1) = 1.0f;
    }

    pan.process(test);

    // Left channel should be louder than right
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_GT(test(i, 0), test(i, 1));
    }
}

TEST_F(BasicEffectsTest, PanEffectFullRight)
{
    PanEffect<float> pan(1.0f); // Full right

    AudioBuffer<float> test(10, 2);
    for (size_t i = 0; i < 10; ++i)
    {
        test(i, 0) = 1.0f;
        test(i, 1) = 1.0f;
    }

    pan.process(test);

    // Right channel should be louder than left
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_GT(test(i, 1), test(i, 0));
    }
}

TEST_F(BasicEffectsTest, PanEffectMonoIgnored)
{
    PanEffect<float> pan(1.0f);

    AudioBuffer<float> mono(10, 1);
    for (size_t i = 0; i < 10; ++i)
    {
        mono(i, 0) = 0.5f;
    }

    pan.process(mono);

    // Mono buffer should be unchanged
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(mono(i, 0), 0.5f);
    }
}

// Effect chaining
TEST_F(BasicEffectsTest, ChainGainAndFade)
{
    AudioBuffer<float> test(100, 1);
    for (size_t i = 0; i < 100; ++i)
    {
        test(i, 0) = 1.0f;
    }

    // Apply gain first
    GainEffect<float> gain(0.5f);
    gain.process(test);

    // Then fade out
    FadeEffect<float> fade(44100.0, 100.0 / 44100.0, FadeEffect<float>::Type::FadeOut);
    fade.process(test);

    // First sample should be around 0.5
    EXPECT_NEAR(test(0, 0), 0.5f, 0.1f);

    // Last sample should be near zero
    EXPECT_NEAR(test(99, 0), 0.0f, 0.1f);
}
