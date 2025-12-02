#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"
#include <gtest/gtest.h>
#include "Effects/GainEffect.hpp"
#include "Effects/FadeEffect.hpp"
#include "Effects/MixEffect.hpp"

namespace effects
{
    bool checkFileExists(const std::string &inputPath)
    {
        std::fstream file(inputPath, std::ios::in);
        return file.good();
    }
}

std::vector<std::string> files{"stereo_12mb.wav", "mono_8bits.wav"};

TEST(EffectTests, GainEffectTest)
{
    for (const auto &file : files)
    {
        // Arrange
        const std::string inputPath = "../../wav-files/" + file;
        const std::string outputPath = "../../wav-files/testfile.gain" + file;

        {
            // Act
            WavTools::Reader reader(inputPath);
            WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
            auto buffer = reader.read<float>();
            // Test
            try
            {
                GainEffect<float> gainEffect(1.5f);
                gainEffect.process(buffer);
                writer.save(outputPath, buffer);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
        }

        // Assert
        ASSERT_TRUE(effects::checkFileExists(outputPath));
    }
}

TEST(EffectTests, FadeInEffectTest)
{
    for (const auto &file : files)
    {
        // Arrange
        const std::string inputPath = "../../wav-files/" + file;
        const std::string outputPath = "../../wav-files/testfile.fadeIn" + file;

        // Act
        WavTools::Reader reader(inputPath);
        WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
        auto buffer = reader.read<float>();
        // Test
        FadeEffect<float> fadeIn(0.0f, 1.0f, std::min(size_t{100}, buffer.num_samples()));
        fadeIn.process(buffer);
        writer.save(outputPath, buffer);

        // Assert
        ASSERT_TRUE(effects::checkFileExists(outputPath));
    }
}

TEST(EffectTests, FadeOutEffectTest)
{
    for (const auto &file : files)
    {
        // Arrange
        const std::string inputPath = "../../wav-files/" + file;
        const std::string outputPath = "../../wav-files/testfile.fadeOut" + file;

        // Act
        WavTools::Reader reader(inputPath);
        WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
        auto buffer = reader.read<float>();
        // Test
        FadeEffect<float> fadeOut(1.0f, 0.0f, std::min(size_t{100}, buffer.num_samples()));
        fadeOut.process(buffer);
        writer.save(outputPath, buffer);

        // Assert
        ASSERT_TRUE(effects::checkFileExists(outputPath));
    }
}

// TEST(EffectTests, MixEffectTest)
// {
//     for (const auto &file : files)
//     {
//         // Arrange
//         const std::string inputPath = "../../wav-files/" + file;
//         const std::string outputPath = "../../wav-files/testfile.mix" + file;

//         // Act
//         WavTools::Reader reader(inputPath);
//         WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
//         auto buffer = reader.read<float>();
//         // Test
//         MixEffect<float> mixEffect;
//         mixEffect.process(buffer);
//         writer.save(outputPath, buffer);

//         // Assert
//         ASSERT_TRUE(effects::checkFileExists(outputPath));
//     }
// }

TEST(EffectTests, MixPanEffectTest)
{
    for (const auto &file : files)
    {
        // Arrange
        const std::string inputPath = "../../wav-files/" + file;
        const std::string outputPath = "../../wav-files/testfile.mixPan" + file;

        // Act
        WavTools::Reader reader(inputPath);
        WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());
        auto buffer = reader.read<float>();
        // Test
        MixEffect<float> mixPanEffect(true, 0.4f);
        mixPanEffect.process(buffer);
        writer.save(outputPath, buffer);

        // Assert
        if (buffer.num_channels() == 2)
            ASSERT_TRUE(effects::checkFileExists(outputPath));
    }
}
