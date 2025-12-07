#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"
#include <gtest/gtest.h>
#include <filesystem>

using namespace audio;
namespace fs = std::filesystem;

class WavIOTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        test_dir_ = "test_wav_files";
        fs::create_directories(test_dir_);
    }

    void TearDown() override
    {
        // Clean up test files
        if (fs::exists(test_dir_))
        {
            fs::remove_all(test_dir_);
        }
    }

    std::string test_dir_;

    // Helper: Create a test WAV file with sine wave
    void create_test_wav(const std::string &filename,
                         uint32_t sample_rate,
                         uint16_t num_channels,
                         uint16_t bits_per_sample,
                         double duration_seconds,
                         double frequency)
    {
        size_t num_samples = static_cast<size_t>(sample_rate * duration_seconds);
        AudioBuffer<float> buffer(num_samples, num_channels);

        // Generate sine wave
        for (size_t i = 0; i < num_samples; ++i)
        {
            float t = static_cast<float>(i) / sample_rate;
            float value = std::sin(2.0f * 3.14159265359f * frequency * t) * 0.5f;

            for (size_t ch = 0; ch < num_channels; ++ch)
            {
                buffer(i, ch) = value;
            }
        }

        WavWriter writer(filename, sample_rate, num_channels, bits_per_sample);
        writer.write(buffer);
    }
};

// Basic WAV writing
TEST_F(WavIOTest, WriteBasicWavFile)
{
    std::string filename = test_dir_ + "/test_write.wav";

    AudioBuffer<float> buffer(100, 2);
    buffer(50, 0) = 0.5f;
    buffer(50, 1) = -0.3f;

    EXPECT_NO_THROW({
        WavWriter writer(filename, 44100, 2, 16);
        writer.write(buffer);
    });

    EXPECT_TRUE(fs::exists(filename));
}

TEST_F(WavIOTest, WriteDifferentBitDepths)
{
    std::string base = test_dir_ + "/test_";

    AudioBuffer<float> buffer(100, 1);
    for (size_t i = 0; i < 100; ++i)
    {
        buffer(i, 0) = 0.5f;
    }

    // Test 8, 16, 24, 32 bit
    for (uint16_t bits : {8, 16, 24, 32})
    {
        std::string filename = base + std::to_string(bits) + "bit.wav";

        EXPECT_NO_THROW({
            WavWriter writer(filename, 44100, 1, bits);
            writer.write(buffer);
        });

        EXPECT_TRUE(fs::exists(filename));
    }
}

TEST_F(WavIOTest, WriteInvalidBitDepthThrows)
{
    std::string filename = test_dir_ + "/invalid.wav";

    EXPECT_THROW({
        WavWriter writer(filename, 44100, 2, 12); // Invalid bit depth
    },
                 std::invalid_argument);
}

// Basic WAV reading
TEST_F(WavIOTest, ReadBasicWavFile)
{
    std::string filename = test_dir_ + "/test_read.wav";
    create_test_wav(filename, 44100, 2, 16, 0.1, 440.0);

    EXPECT_NO_THROW({
        WavReader reader(filename);

        EXPECT_EQ(reader.sample_rate(), 44100);
        EXPECT_EQ(reader.num_channels(), 2);
        EXPECT_EQ(reader.bits_per_sample(), 16);
        EXPECT_GT(reader.num_samples(), 0);
    });
}

TEST_F(WavIOTest, ReadNonExistentFileThrows)
{
    EXPECT_THROW({ WavReader reader("nonexistent.wav"); }, std::runtime_error);
}

// Round-trip tests (write then read)
TEST_F(WavIOTest, RoundTrip16BitMono)
{
    std::string filename = test_dir_ + "/roundtrip_16_mono.wav";

    // Create original buffer with known values
    AudioBuffer<float> original(100, 1);
    for (size_t i = 0; i < 100; ++i)
    {
        original(i, 0) = std::sin(i * 0.1f) * 0.8f;
    }

    // Write
    WavWriter writer(filename, 44100, 1, 16);
    writer.write(original);

    // Read back
    WavReader reader(filename);
    auto recovered = reader.read<float>();

    // Check properties
    EXPECT_EQ(recovered.num_samples(), original.num_samples());
    EXPECT_EQ(recovered.num_channels(), original.num_channels());

    // Check values (with tolerance for quantization)
    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_NEAR(recovered(i, 0), original(i, 0), 0.01f);
    }
}

TEST_F(WavIOTest, RoundTrip16BitStereo)
{
    std::string filename = test_dir_ + "/roundtrip_16_stereo.wav";

    AudioBuffer<float> original(100, 2);
    for (size_t i = 0; i < 100; ++i)
    {
        original(i, 0) = std::sin(i * 0.1f) * 0.7f;
        original(i, 1) = std::cos(i * 0.1f) * 0.7f;
    }

    WavWriter writer(filename, 48000, 2, 16);
    writer.write(original);

    WavReader reader(filename);
    auto recovered = reader.read<float>();

    EXPECT_EQ(reader.sample_rate(), 48000);
    EXPECT_EQ(recovered.num_channels(), 2);

    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_NEAR(recovered(i, 0), original(i, 0), 0.01f);
        EXPECT_NEAR(recovered(i, 1), original(i, 1), 0.01f);
    }
}

TEST_F(WavIOTest, RoundTrip24Bit)
{
    std::string filename = test_dir_ + "/roundtrip_24.wav";

    AudioBuffer<float> original(100, 2);
    for (size_t i = 0; i < 100; ++i)
    {
        original(i, 0) = 0.123456f;
        original(i, 1) = -0.654321f;
    }

    WavWriter writer(filename, 44100, 2, 24);
    writer.write(original);

    WavReader reader(filename);
    auto recovered = reader.read<float>();

    EXPECT_EQ(reader.bits_per_sample(), 24);

    // 24-bit should have better precision than 16-bit
    for (size_t i = 0; i < 100; ++i)
    {
        EXPECT_NEAR(recovered(i, 0), original(i, 0), 0.0001f);
        EXPECT_NEAR(recovered(i, 1), original(i, 1), 0.0001f);
    }
}

// Different sample types
TEST_F(WavIOTest, ReadAsInt16)
{
    std::string filename = test_dir_ + "/read_int16.wav";
    create_test_wav(filename, 44100, 1, 16, 0.1, 440.0);

    WavReader reader(filename);
    auto buffer = reader.read<int16_t>();

    EXPECT_EQ(buffer.num_samples(), reader.num_samples());

    // Check that values are in valid int16 range
    bool has_nonzero = false;
    for (size_t i = 0; i < buffer.num_samples(); ++i)
    {
        EXPECT_GE(buffer(i, 0), -32768);
        EXPECT_LE(buffer(i, 0), 32767);
        if (buffer(i, 0) != 0)
            has_nonzero = true;
    }
    EXPECT_TRUE(has_nonzero); // Should have some signal
}

TEST_F(WavIOTest, ReadAsDouble)
{
    std::string filename = test_dir_ + "/read_double.wav";
    create_test_wav(filename, 44100, 1, 16, 0.1, 440.0);

    WavReader reader(filename);
    auto buffer = reader.read<double>();

    // Check that values are normalized to [-1.0, 1.0]
    for (size_t i = 0; i < buffer.num_samples(); ++i)
    {
        EXPECT_GE(buffer(i, 0), -1.0);
        EXPECT_LE(buffer(i, 0), 1.0);
    }
}

// File size and duration
TEST_F(WavIOTest, FilePropertiesCorrect)
{
    std::string filename = test_dir_ + "/properties.wav";

    uint32_t sample_rate = 44100;
    double duration = 1.0; // 1 second

    create_test_wav(filename, sample_rate, 2, 16, duration, 440.0);

    WavReader reader(filename);

    EXPECT_EQ(reader.sample_rate(), sample_rate);
    EXPECT_EQ(reader.num_channels(), 2);
    EXPECT_EQ(reader.bits_per_sample(), 16);

    // Duration should be approximately 1 second
    EXPECT_NEAR(reader.duration(), duration, 0.001);

    // Number of samples should match
    EXPECT_EQ(reader.num_samples(), static_cast<uint32_t>(sample_rate * duration));
}

// Edge cases
TEST_F(WavIOTest, EmptyBufferWrite)
{
    std::string filename = test_dir_ + "/empty.wav";

    AudioBuffer<float> buffer(1, 1); // Minimal valid buffer
    buffer(0, 0) = 0.0f;

    EXPECT_NO_THROW({
        WavWriter writer(filename, 44100, 1, 16);
        writer.write(buffer);
    });
}

TEST_F(WavIOTest, LargeBufferRoundTrip)
{
    std::string filename = test_dir_ + "/large.wav";

    // 10 seconds of audio
    size_t num_samples = 441000;
    AudioBuffer<float> original(num_samples, 2);

    // Fill with some pattern
    for (size_t i = 0; i < num_samples; i += 1000)
    {
        original(i, 0) = 0.5f;
        original(i, 1) = -0.5f;
    }

    WavWriter writer(filename, 44100, 2, 16);
    writer.write(original);

    WavReader reader(filename);
    auto recovered = reader.read<float>();

    EXPECT_EQ(recovered.num_samples(), num_samples);

    // Spot check some values
    for (size_t i = 0; i < num_samples; i += 10000)
    {
        EXPECT_NEAR(recovered(i, 0), original(i, 0), 0.01f);
    }
}

// Silence detection
TEST_F(WavIOTest, SilencePreserved)
{
    std::string filename = test_dir_ + "/silence.wav";

    AudioBuffer<float> silence(1000, 2);
    silence.clear(); // All zeros

    WavWriter writer(filename, 44100, 2, 16);
    writer.write(silence);

    WavReader reader(filename);
    auto recovered = reader.read<float>();

    // All samples should be very close to zero
    for (size_t i = 0; i < 1000; ++i)
    {
        EXPECT_NEAR(recovered(i, 0), 0.0f, 0.0001f);
        EXPECT_NEAR(recovered(i, 1), 0.0f, 0.0001f);
    }
}

// Sample rate variations
TEST_F(WavIOTest, DifferentSampleRates)
{
    std::vector<uint32_t> rates = {8000, 16000, 22050, 44100, 48000, 96000};

    for (uint32_t rate : rates)
    {
        std::string filename = test_dir_ + "/rate_" + std::to_string(rate) + ".wav";
        create_test_wav(filename, rate, 1, 16, 0.1, 440.0);

        WavReader reader(filename);
        EXPECT_EQ(reader.sample_rate(), rate);
    }
}
