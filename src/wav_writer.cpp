#include "audio_buffer.hpp"
#include "wav_writer.hpp"

namespace WWriter
{
    template <typename T>
    T convert_sample(int16_t sample)
    {
        return sample;
    }

    template <>
    float convert_sample<float>(int16_t sample)
    {
        return static_cast<float>(sample) / 32768.0f;
    }

    template <>
    int8_t convert_sample<int8_t>(int16_t sample)
    {
        return static_cast<int8_t>(sample / 256);
    }

    template <typename T>
    T convert_sample(float sample)
    {
        return sample;
    }

    template <>
    int16_t convert_sample<int16_t>(float sample)
    {
        return static_cast<int16_t>(sample * 32768);
    }

    template <>
    int8_t convert_sample<int8_t>(float sample)
    {
        return static_cast<int8_t>(sample * 256);
    }

    template <typename T>
    T convert_sample(int8_t sample)
    {
        return sample;
    }
}

/// @brief Wav writer instance for creating wav copies
/// @param filename
/// @param sample_rate
/// @param num_channels
/// @param bits_per_sample
WavWriter::WavWriter(const std::string &filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample)
    : file_(std::fopen(filename.c_str(), "wb"), &std::fclose), sample_rate_(sample_rate), num_channels_(num_channels), bits_per_sample_(bits_per_sample)
{
    if (!file_)
    {
        throw std::runtime_error("Cannot create file : " + filename);
    }
}

void WavWriter::write_header(uint32_t data_size)
{
    size_t INT_BLOCK = 1;
    auto write_u16 = [this, INT_BLOCK](uint16_t value)
    {
        std::fwrite(&value, sizeof(uint16_t), INT_BLOCK, file_.get());
    };

    auto write_u32 = [this, INT_BLOCK](uint32_t value)
    {
        std::fwrite(&value, sizeof(uint32_t), INT_BLOCK, file_.get());
    };

    size_t WORD_BLOCK = 4;
    auto write_id = [this, WORD_BLOCK](const char *id)
    {
        std::fwrite(id, sizeof(char), WORD_BLOCK, file_.get());
    };

    // RIFF Header
    write_id("RIFF");
    write_u32(36 + data_size);
    write_id("WAVE");

    // fmt chunk
    write_id("fmt ");
    const uint32_t pcm_chunk_size = 16;
    write_u32(pcm_chunk_size);
    const uint16_t linear_quantization = 1;
    write_u16(linear_quantization);
    write_u16(num_channels_);
    write_u32(sample_rate_);
    uint32_t byte_rate = sample_rate_ * num_channels_ * bits_per_sample_ / 8;
    write_u32(byte_rate);
    uint16_t block_align = static_cast<uint16_t>(num_channels_ * bits_per_sample_ / 8);
    write_u16(block_align);
    write_u16(bits_per_sample_);

    // data chunk
    write_id("data");
    write_u32(data_size);
}

template <typename SampleType>
void WavWriter::write(const AudioBuffer<SampleType> &buffer)
{
    uint32_t data_size = buffer.num_samples() * buffer.num_channels() * bits_per_sample_ / 8;
    write_header(data_size);

    if (bits_per_sample_ == 32)
    {
        std::vector<float> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WWriter::convert_sample<float>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(float), temp.size(), file_.get());
    }
    else if (bits_per_sample_ == 16)
    {
        std::vector<int16_t> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WWriter::convert_sample<int16_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int16_t), temp.size(), file_.get());
    }
    else if (bits_per_sample_ == 8)
    {
        std::vector<int8_t> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WWriter::convert_sample<int8_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int8_t), temp.size(), file_.get());
    }
}

/// @brief Explicit template definition
template void WavWriter::write<float>(const AudioBuffer<float> &);
template void WavWriter::write<int16_t>(const AudioBuffer<int16_t> &);
template void WavWriter::write<int8_t>(const AudioBuffer<int8_t> &);
