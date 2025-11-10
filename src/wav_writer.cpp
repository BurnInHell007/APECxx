#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "wav_writer.hpp"

namespace wav_writer
{
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
} // namespace wav_writer

WavWriter::WavWriter(const std::string& filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample)
    : file_(std::fopen(filename.c_str(), "wb"), &std::fclose)
    , sample_rate_(sample_rate)
    , num_channels_(num_channels)
    , bits_per_sample_(bits_per_sample)
{
    if (!file_) 
        throw std::runtime_error("Cannot open file : " + filename);
}

// Header file writer 
void WavWriter::write_header(uint32_t data_size) {
    // fixed size blocks
    size_t word_block = 4;
    size_t int_block = 1;

    auto write_id = [this, word_block](const char* chunk_id){
        std::fwrite(chunk_id, sizeof(char), word_block, file_.get());
    };

    auto write_u32 = [this, int_block](uint32_t value){
        std::fwrite(&value, sizeof(uint32_t), int_block, file_.get());
    };

    auto write_u16 = [this, int_block](uint16_t value){
        std::fwrite(&value, sizeof(uint16_t), int_block, file_.get());
    };

    // RIFF chunk
    write_id("RIFF");
    write_u32(36 + data_size);
    write_id("WAVE");

    // fmt  chunk
    write_id("fmt ");
    const uint32_t pcm_chunk_size = 16;
    write_u32(pcm_chunk_size);
    const uint16_t linear_quantization = 1;
    write_u16(linear_quantization);
    write_u16(num_channels_);
    write_u32(sample_rate_);
    const uint32_t byte_rate = (sample_rate_ * num_channels_ * bits_per_sample_ / 8);
    write_u32(byte_rate);
    const uint16_t block_align = (num_channels_ * bits_per_sample_ / 8);
    write_u16(block_align);
    write_u16(bits_per_sample_);

    // data chunk
    write_id("data");
    write_u32(data_size);
}

/// @brief read from the buffer and write into the file
/// @tparam SampleType 
/// @param buffer audio buffer with audio stored in it
template<typename SampleType>
void WavWriter::write(const AudioBuffer<SampleType>& buffer) {
    uint32_t data_size = (buffer.num_samples() * buffer.num_channels() * bits_per_sample_ / 8);
    write_header(data_size);

    // convert the types
    if (bits_per_sample_ == 16) {
        std::vector<int16_t> temp(buffer.num_channels() * buffer.num_samples());

        for(size_t i = 0; i < temp.size(); ++i) {
            temp[i] = wav_writer::convert_sample<int16_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int16_t), temp.size(), file_.get());
    } else if (bits_per_sample_ == 8) {
        std::vector<int8_t> temp(buffer.num_channels() * buffer.num_samples());

        for(size_t i = 0; i < temp.size(); ++i) {
            temp[i] = wav_writer::convert_sample<int8_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int8_t), temp.size(), file_.get());

    } else if (bits_per_sample_ == 32) {
        std::vector<float> temp(buffer.num_channels() * buffer.num_samples());

        for(size_t i = 0; i < temp.size(); ++i) {
            temp[i] = wav_writer::convert_sample<float>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(float), temp.size(), file_.get());
    }
}
