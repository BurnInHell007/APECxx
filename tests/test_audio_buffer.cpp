#include <gtest/gtest.h>
#include "AudioBuffer.hpp"
#include <stdexcept>

using namespace audio;

// Test fixture for AudioBuffer
class AudioBufferTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Common setup if needed
    }
};

// Construction and basic properties
TEST_F(AudioBufferTest, ConstructorCreatesValidBuffer)
{
    AudioBuffer<float> buffer(1024, 2);

    EXPECT_EQ(buffer.num_samples(), 1024);
    EXPECT_EQ(buffer.num_channels(), 2);
    EXPECT_EQ(buffer.total_samples(), 2048);
    EXPECT_FALSE(buffer.empty());
}

TEST_F(AudioBufferTest, DefaultConstructorCreatesEmptyBuffer)
{
    AudioBuffer<float> buffer;

    EXPECT_EQ(buffer.num_samples(), 0);
    EXPECT_EQ(buffer.num_channels(), 0);
    EXPECT_TRUE(buffer.empty());
}

TEST_F(AudioBufferTest, ConstructorThrowsOnZeroSamples)
{
    EXPECT_THROW(AudioBuffer<float>(0, 2), std::invalid_argument);
}

TEST_F(AudioBufferTest, ConstructorThrowsOnZeroChannels)
{
    EXPECT_THROW(AudioBuffer<float>(1024, 0), std::invalid_argument);
}

// Sample access
TEST_F(AudioBufferTest, SampleAccessWorks)
{
    AudioBuffer<float> buffer(10, 2);

    buffer(5, 0) = 0.5f;
    buffer(5, 1) = -0.3f;

    EXPECT_FLOAT_EQ(buffer(5, 0), 0.5f);
    EXPECT_FLOAT_EQ(buffer(5, 1), -0.3f);
}

TEST_F(AudioBufferTest, OutOfBoundsAccessThrows)
{
    AudioBuffer<float> buffer(10, 2);

    EXPECT_THROW(buffer(10, 0), std::out_of_range); // Sample index too high
    EXPECT_THROW(buffer(0, 2), std::out_of_range);  // Channel index too high
}

TEST_F(AudioBufferTest, ClearFillsWithZeros)
{
    AudioBuffer<float> buffer(10, 2);

    // Fill with non-zero values
    for (size_t i = 0; i < 10; ++i)
    {
        buffer(i, 0) = 1.0f;
        buffer(i, 1) = -1.0f;
    }

    buffer.clear();

    // Check all samples are zero
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(buffer(i, 0), 0.0f);
        EXPECT_FLOAT_EQ(buffer(i, 1), 0.0f);
    }
}

// Copy semantics
TEST_F(AudioBufferTest, CopyConstructorWorks)
{
    AudioBuffer<float> original(10, 2);
    original(5, 0) = 0.7f;
    original(5, 1) = -0.2f;

    AudioBuffer<float> copy(original);

    EXPECT_EQ(copy.num_samples(), original.num_samples());
    EXPECT_EQ(copy.num_channels(), original.num_channels());
    EXPECT_FLOAT_EQ(copy(5, 0), 0.7f);
    EXPECT_FLOAT_EQ(copy(5, 1), -0.2f);

    // Modify copy, original should be unchanged
    copy(5, 0) = 0.1f;
    EXPECT_FLOAT_EQ(original(5, 0), 0.7f);
}

TEST_F(AudioBufferTest, CopyAssignmentWorks)
{
    AudioBuffer<float> original(10, 2);
    original(3, 1) = 0.9f;

    AudioBuffer<float> copy(5, 1); // Different size
    copy = original;

    EXPECT_EQ(copy.num_samples(), 10);
    EXPECT_EQ(copy.num_channels(), 2);
    EXPECT_FLOAT_EQ(copy(3, 1), 0.9f);
}

// Move semantics
TEST_F(AudioBufferTest, MoveConstructorWorks)
{
    AudioBuffer<float> original(10, 2);
    original(5, 0) = 0.8f;

    AudioBuffer<float> moved(std::move(original));

    EXPECT_EQ(moved.num_samples(), 10);
    EXPECT_EQ(moved.num_channels(), 2);
    EXPECT_FLOAT_EQ(moved(5, 0), 0.8f);

    // Original should be empty after move
    EXPECT_EQ(original.num_samples(), 0);
    EXPECT_EQ(original.num_channels(), 0);
}

TEST_F(AudioBufferTest, MoveAssignmentWorks)
{
    AudioBuffer<float> original(10, 2);
    original(4, 1) = 0.6f;

    AudioBuffer<float> moved(5, 1);
    moved = std::move(original);

    EXPECT_EQ(moved.num_samples(), 10);
    EXPECT_EQ(moved.num_channels(), 2);
    EXPECT_FLOAT_EQ(moved(4, 1), 0.6f);
}

// Buffer operations
TEST_F(AudioBufferTest, ApplyGainWorks)
{
    AudioBuffer<float> buffer(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        buffer(i, 0) = 0.5f;
    }

    buffer.apply_gain(2.0f);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(buffer(i, 0), 1.0f);
    }
}

TEST_F(AudioBufferTest, MixBuffersWorks)
{
    AudioBuffer<float> buffer1(10, 1);
    AudioBuffer<float> buffer2(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        buffer1(i, 0) = 0.3f;
        buffer2(i, 0) = 0.2f;
    }

    buffer1.mix(buffer2, 1.0f);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(buffer1(i, 0), 0.5f);
    }
}

TEST_F(AudioBufferTest, MixThrowsOnDimensionMismatch)
{
    AudioBuffer<float> buffer1(10, 2);
    AudioBuffer<float> buffer2(5, 2); // Different size

    EXPECT_THROW(buffer1.mix(buffer2), std::invalid_argument);
}

TEST_F(AudioBufferTest, ResizeWorks)
{
    AudioBuffer<float> buffer(10, 2);
    buffer(5, 0) = 0.7f;

    buffer.resize(20, 1);

    EXPECT_EQ(buffer.num_samples(), 20);
    EXPECT_EQ(buffer.num_channels(), 1);
    // Data should be cleared after resize
    EXPECT_FLOAT_EQ(buffer(5, 0), 0.0f);
}

TEST_F(AudioBufferTest, GetChannelWorks)
{
    AudioBuffer<float> stereo(10, 2);

    for (size_t i = 0; i < 10; ++i)
    {
        stereo(i, 0) = 0.1f * i;
        stereo(i, 1) = 0.2f * i;
    }

    AudioBuffer<float> left = stereo.get_channel(0);
    AudioBuffer<float> right = stereo.get_channel(1);

    EXPECT_EQ(left.num_samples(), 10);
    EXPECT_EQ(left.num_channels(), 1);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(left(i, 0), 0.1f * i);
        EXPECT_FLOAT_EQ(right(i, 0), 0.2f * i);
    }
}

TEST_F(AudioBufferTest, SetChannelWorks)
{
    AudioBuffer<float> stereo(10, 2);
    AudioBuffer<float> mono(10, 1);

    for (size_t i = 0; i < 10; ++i)
    {
        mono(i, 0) = 0.5f;
    }

    stereo.set_channel(0, mono);

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(stereo(i, 0), 0.5f);
    }
}

// Different sample types
TEST_F(AudioBufferTest, Int16BufferWorks)
{
    AudioBuffer<int16_t> buffer(10, 2);

    buffer(5, 0) = 1000;
    buffer(5, 1) = -2000;

    EXPECT_EQ(buffer(5, 0), 1000);
    EXPECT_EQ(buffer(5, 1), -2000);
}

TEST_F(AudioBufferTest, DoubleBufferWorks)
{
    AudioBuffer<double> buffer(10, 2);

    buffer(5, 0) = 0.123456789;

    EXPECT_DOUBLE_EQ(buffer(5, 0), 0.123456789);
}

// Raw pointer access
TEST_F(AudioBufferTest, RawPointerAccessWorks)
{
    AudioBuffer<float> buffer(10, 2);

    float *data = buffer.data();
    data[0] = 0.5f;
    data[1] = 0.6f;

    EXPECT_FLOAT_EQ(buffer(0, 0), 0.5f);
    EXPECT_FLOAT_EQ(buffer(0, 1), 0.6f);
}

TEST_F(AudioBufferTest, ConstPointerAccessWorks)
{
    AudioBuffer<float> buffer(10, 2);
    buffer(0, 0) = 0.7f;

    const AudioBuffer<float> &const_ref = buffer;
    const float *data = const_ref.data();

    EXPECT_FLOAT_EQ(data[0], 0.7f);
}
