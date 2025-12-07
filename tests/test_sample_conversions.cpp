#include <gtest/gtest.h>
#include "SampleConversion.hpp"
#include <cmath>

using namespace audio;

class SampleConversionTest : public ::testing::Test
{
};

// Int16 to Float conversions
TEST_F(SampleConversionTest, Int16ToFloatMaxPositive)
{
    int16_t sample = 32767;
    float result = convert_sample<float>(sample);
    EXPECT_NEAR(result, 1.0f, 0.0001f);
}

TEST_F(SampleConversionTest, Int16ToFloatMaxNegative)
{
    int16_t sample = -32768;
    float result = convert_sample<float>(sample);
    EXPECT_NEAR(result, -1.0f, 0.0001f);
}

TEST_F(SampleConversionTest, Int16ToFloatZero)
{
    int16_t sample = 0;
    float result = convert_sample<float>(sample);
    EXPECT_FLOAT_EQ(result, 0.0f);
}

TEST_F(SampleConversionTest, Int16ToFloatHalfScale)
{
    int16_t sample = 16384;
    float result = convert_sample<float>(sample);
    EXPECT_NEAR(result, 0.5f, 0.01f);
}

// Float to Int16 conversions
TEST_F(SampleConversionTest, FloatToInt16MaxPositive)
{
    float sample = 1.0f;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, 32767);
}

TEST_F(SampleConversionTest, FloatToInt16MaxNegative)
{
    float sample = -1.0f;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, -32767);
}

TEST_F(SampleConversionTest, FloatToInt16Zero)
{
    float sample = 0.0f;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, 0);
}

TEST_F(SampleConversionTest, FloatToInt16Clamping)
{
    // Test clamping of out-of-range values
    float too_high = 1.5f;
    float too_low = -1.5f;

    int16_t result_high = convert_sample<int16_t>(too_high);
    int16_t result_low = convert_sample<int16_t>(too_low);

    EXPECT_EQ(result_high, 32767);
    EXPECT_EQ(result_low, -32767);
}

// 24-bit utilities
TEST_F(SampleConversionTest, Int24ReadMaxPositive)
{
    uint8_t bytes[3] = {0xFF, 0xFF, 0x7F}; // Max positive 24-bit
    int32_t result = int24::read(bytes);
    EXPECT_EQ(result, 8388607);
}

TEST_F(SampleConversionTest, Int24ReadMaxNegative)
{
    uint8_t bytes[3] = {0x00, 0x00, 0x80}; // Min negative 24-bit
    int32_t result = int24::read(bytes);
    EXPECT_EQ(result, -8388608);
}

TEST_F(SampleConversionTest, Int24ReadZero)
{
    uint8_t bytes[3] = {0x00, 0x00, 0x00};
    int32_t result = int24::read(bytes);
    EXPECT_EQ(result, 0);
}

TEST_F(SampleConversionTest, Int24WriteMaxPositive)
{
    uint8_t bytes[3];
    int24::write(8388607, bytes);

    EXPECT_EQ(bytes[0], 0xFF);
    EXPECT_EQ(bytes[1], 0xFF);
    EXPECT_EQ(bytes[2], 0x7F);
}

TEST_F(SampleConversionTest, Int24WriteMaxNegative)
{
    uint8_t bytes[3];
    int24::write(-8388608, bytes);

    EXPECT_EQ(bytes[0], 0x00);
    EXPECT_EQ(bytes[1], 0x00);
    EXPECT_EQ(bytes[2], 0x80);
}

TEST_F(SampleConversionTest, Int24WriteZero)
{
    uint8_t bytes[3];
    int24::write(0, bytes);

    EXPECT_EQ(bytes[0], 0x00);
    EXPECT_EQ(bytes[1], 0x00);
    EXPECT_EQ(bytes[2], 0x00);
}

TEST_F(SampleConversionTest, Int24ToFloatConversion)
{
    int32_t max_24bit = 8388607;
    float result = int24::to_float(max_24bit);
    EXPECT_NEAR(result, 1.0f, 0.0001f);

    int32_t min_24bit = -8388608;
    result = int24::to_float(min_24bit);
    EXPECT_NEAR(result, -1.0f, 0.0001f);
}

TEST_F(SampleConversionTest, FloatToInt24Conversion)
{
    float sample = 1.0f;
    int32_t result = int24::from_float(sample);
    EXPECT_EQ(result, 8388607);

    sample = -1.0f;
    result = int24::from_float(sample);
    EXPECT_EQ(result, -8388607);
}

TEST_F(SampleConversionTest, FloatToInt24Clamping)
{
    float too_high = 1.5f;
    int32_t result = int24::from_float(too_high);
    EXPECT_EQ(result, 8388607);

    float too_low = -1.5f;
    result = int24::from_float(too_low);
    EXPECT_EQ(result, -8388607);
}

// Int32 (24-bit) to Float
TEST_F(SampleConversionTest, Int32ToFloatConversion)
{
    int32_t sample = 8388607;
    float result = convert_sample<float>(sample);
    EXPECT_NEAR(result, 1.0f, 0.0001f);
}

TEST_F(SampleConversionTest, FloatToInt32Conversion)
{
    float sample = 1.0f;
    int32_t result = convert_sample<int32_t>(sample);
    EXPECT_EQ(result, 8388607);
}

// Identity conversions
TEST_F(SampleConversionTest, Int16IdentityConversion)
{
    int16_t sample = 12345;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, sample);
}

TEST_F(SampleConversionTest, FloatIdentityConversion)
{
    float sample = 0.12345f;
    float result = convert_sample<float>(sample);
    EXPECT_FLOAT_EQ(result, sample);
}

// Cross conversions
TEST_F(SampleConversionTest, Int16ToInt32Conversion)
{
    int16_t sample = 1000;
    int32_t result = convert_sample<int32_t>(sample);
    EXPECT_EQ(result, 1000 << 8); // Should shift left 8 bits
}

TEST_F(SampleConversionTest, Int32ToInt16Conversion)
{
    int32_t sample = 1000 << 8;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, 1000); // Should shift right 8 bits
}

// Double precision tests
TEST_F(SampleConversionTest, Int16ToDoubleConversion)
{
    int16_t sample = 32767;
    double result = convert_sample<double>(sample);
    EXPECT_NEAR(result, 1.0, 0.0001);
}

TEST_F(SampleConversionTest, DoubleToInt16Conversion)
{
    double sample = 1.0;
    int16_t result = convert_sample<int16_t>(sample);
    EXPECT_EQ(result, 32767);
}

TEST_F(SampleConversionTest, FloatToDoubleConversion)
{
    float sample = 0.5f;
    double result = convert_sample<double>(sample);
    EXPECT_DOUBLE_EQ(result, 0.5);
}

TEST_F(SampleConversionTest, DoubleToFloatConversion)
{
    double sample = 0.123456789;
    float result = convert_sample<float>(sample);
    EXPECT_NEAR(result, 0.123456789f, 0.0000001f);
}

// Round-trip conversions
TEST_F(SampleConversionTest, Int16FloatRoundTrip)
{
    int16_t original = 10000;
    float intermediate = convert_sample<float>(original);
    int16_t result = convert_sample<int16_t>(intermediate);

    // Should be very close (within rounding error)
    EXPECT_NEAR(result, original, 1);
}

TEST_F(SampleConversionTest, FloatInt16FloatRoundTrip)
{
    float original = 0.75f;
    int16_t intermediate = convert_sample<int16_t>(original);
    float result = convert_sample<float>(intermediate);

    // Should be very close
    EXPECT_NEAR(result, original, 0.0001f);
}
