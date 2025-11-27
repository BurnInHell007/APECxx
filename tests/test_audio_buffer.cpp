#include "AudioBuffer.hpp"
#include "project.h"
#include <gtest/gtest.h>

AudioBuffer<float> Buffer(44000, 2);

struct AudioBufferTests: public ::testing::Test
{
    size_t GetSampleRate() const
    {
        return Buffer.num_samples();
    }
    
    size_t GetNumChannels() const
    {
        return Buffer.num_channels();
    }
};

TEST_F(AudioBufferTests, CheckSampleRates) {
    ASSERT_EQ(GetSampleRate(), 44000);
}

TEST_F(AudioBufferTests, CheckNumChannels) {
    ASSERT_EQ(GetNumChannels(), 2);
}
