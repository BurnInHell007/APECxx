#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "audio_buffer.hpp"
#include "wav_reader.hpp"

namespace wav_reader {
    /// @brief int16_t converter type 
    template <typename T>
    T convert_sample(int16_t sample) {
        return sample;
    }

    /// @brief Takes in a 16 bit sample, converts into float type 
    /// @return high precision audio ranging from [-1.0, 1.0]
    template<>
    float convert_sample<float>(int16_t sample) {
        return static_cast<float>(sample / 32768.0f);
    }

    template<>
    int8_t convert_sample<int8_t>(int16_t sample) {
        return static_cast<int8_t>(sample / 256);
    }

    /// @brief int8_t converter type 
    template <typename T>
    T convert_sample(int8_t sample) {
        return sample;
    }

    template<>
    float convert_sample<float>(int8_t sample) {
        return static_cast<float>(sample / 256.0f);
    }
    
    /// @brief float converter type
    template <typename T>
    T convert_sample(float sample) {
        return sample;
    }

    template<>
    int16_t convert_sample<int16_t>(float sample) {
        return static_cast<int16_t>(sample * 32768);
    }

    template<>
    int8_t convert_sample<int8_t>(float sample) {
        return static_cast<int8_t>(sample * 256);
    }
};

/// @brief Open file in read mode
/// @param filename 
WavReader::WavReader(const std::string& filename)
    : file_(std::fopen(filename.c_str(), "rb"), &std::fclose)
{
    if (!file_)
        throw std::runtime_error("Cannot open file : " + filename);
    read_header();
}

/// @brief Parse the Header
void WavReader::read_header() {
    // to consider 4 char as a word
    size_t word_block = 4;
    // to consider the next data as whole integer
    size_t int_block = 1;

    // read chunk id
    auto read_id = [this, word_block](const char* expected) { 
        char chunk_id[4];
        if(std::fread(chunk_id, sizeof(char), word_block, file_.get()) != 4)
            throw std::runtime_error("Failed to read chunk ID");;
        if(std::memcmp(chunk_id, expected, 4) != 0)
            throw std::runtime_error(std::string("Expected '") + expected + "' chunk");
    };

    // read uint32_t blocks
    auto read_u32 = [this, int_block]() {
        uint32_t value;
        std::fread(&value, sizeof(uint32_t), int_block, file_.get());
        return value;
    };

    // read uint16_t blocks
    auto read_u16 = [this, int_block]() {
        uint16_t value;
        std::fread(&value, sizeof(uint16_t), int_block, file_.get());
        return value;
    };

    // Read RIFF chunk
    read_id("RIFF");
    uint32_t file_size = read_u32();
    read_id("WAVE");

    // Read fmt  chunk
    read_id("fmt ");
    uint32_t fmt_size = read_u32();
    uint16_t audio_format = read_u16();

    if (audio_format != 1)
        throw std::runtime_error("Only PCM format supported");
    num_channels_ = read_u16();
    sample_rate_ = read_u32();
    uint32_t byte_rate = read_u32();
    uint16_t block_align = read_u16();
    bits_per_sample_ = read_u16();

    if (fmt_size > 16)
        std::fseek(file_.get(), static_cast<long>(fmt_size - 16), SEEK_CUR);
    
    // Find data chunk
    while (true) {
        char chunk_id[4];
        if (std::fread(chunk_id, sizeof(char), word_block, file_.get()) != 4)
            throw std::runtime_error("Data chunk not found");

        uint32_t chunk_size = read_u32();

        if(std::memcmp(chunk_id, "data", 4) == 0) {
            num_samples_ = chunk_size / (num_channels_ * bits_per_sample_ / uint32_t(8));
            data_start_pos_ = static_cast<size_t>(std::ftell(file_.get()));
            break;
        } else {
            std::fseek(file_.get(), static_cast<long>(chunk_size), SEEK_CUR);
        }
    }
}

/// @brief Reads audio file
/// @return audio buffer with data
template <typename SampleType>
AudioBuffer<SampleType> WavReader::read() {
    // Set pointer to begining
    std::fseek(file_.get(), data_start_pos_, SEEK_SET);

    AudioBuffer<SampleType> buffer(num_samples_, num_channels_);
    
    size_t total_size = num_samples_ * num_channels_;

    if (bits_per_sample_ == 16) {
        std::vector<int16_t> temp(total_size);
        std::fread(temp.data(), sizeof(int16_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++) {
            buffer.data()[i] = wav_reader::convert_sample<SampleType>(temp[i]);
        }
    }  else if (bits_per_sample_ == 8) {
        std::vector<int8_t> temp(total_size);
        std::fread(temp.data(), sizeof(int8_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++) {
            buffer.data()[i] = wav_reader::convert_sample<SampleType>(temp[i]);
        }
    } else if (bits_per_sample_ == 32) {
        std::vector<int32_t> temp(total_size);
        std::fread(temp.data(), sizeof(int32_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++) {
            buffer.data()[i] = wav_reader::convert_sample<SampleType>(temp[i]);
        }
    }

    return buffer;
}
