#include "project.h"
#include "wav_reader.hpp"
#include "wav_writer.hpp"
#include <gtest/gtest.h>

bool checkFileExists(const std::string& inputPath)
{
    std::fstream file(inputPath, std::ios::in);
    return file.good();
}

TEST(WavIOTest, ReadWavFile)
{
    // Arrange 
    const std::string inputPath = "../../wav-files/sample-1.0.wav";
    
    // Act
    WavReader reader(inputPath);
    
    // Assert
    ASSERT_EQ(reader.sample_rate(), 44100);
    ASSERT_EQ(reader.num_channels(), 2);
    ASSERT_EQ(reader.bits_per_sample(), 16);
    ASSERT_GT(static_cast<float>(reader.num_samples() / (float)reader.sample_rate()), 5);
    /**
        Input file info:
       	Sample rate: 44100 Hz
    	Channels: 2
    	Bit depth: 16 bits
    	Duration: 5.94317 seconds
        Successfully wrote ..\wav-files\output.wav
    **/
}

TEST(WavIOTest, WriteWavFile)
{
    // Arrange
    const std::string inputPath = "../../wav-files/sample-1.0.wav";
    const std::string outputPath = "../../wav-files/output.wav";
    
    // Act
    WavReader reader(inputPath);
    WavWriter writer(outputPath, reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
    writer.write(reader.read<float>());
    
    // Assert
    ASSERT_TRUE(checkFileExists(outputPath));
}