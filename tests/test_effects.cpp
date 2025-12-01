#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"
#include "Effects/GainEffect.hpp"
#include "Effects/FadeEffect.hpp"
#include <gtest/gtest.h>

bool checkFileExists(const std::string& inputPath)
{
    std::fstream file(inputPath, std::ios::in);
    return file.good();
}

TEST(EffectTest, GainEffectTest)
{
    // Arrange
    const std::string inputPath = "../../wav-files/stereo_1mb.wav";
    const std::string outputPath = "../../wav-files/gainEffect_stereo_1mb.wav";

    // Act
    WavTools::Reader reader(inputPath);
    WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
    auto buffer = reader.read<float>();
    // Test
    GainEffect<float> gainEffect(2.0f);
    gainEffect.process(buffer);
    writer.save(outputPath, buffer);

    // Assert
    ASSERT_TRUE(checkFileExists(outputPath));
}
