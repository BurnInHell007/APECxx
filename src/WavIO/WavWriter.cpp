#include "AudioBuffer.hpp"
#include "WavIO/WavWriter.hpp"

/// @brief Wav writer instance for creating wav copies
/// @param sample_rate
/// @param num_channels
/// @param bits_per_sample
WavTools::Writer::Writer(uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample)
    : sample_rate_(sample_rate), num_channels_(num_channels), bits_per_sample_(bits_per_sample)
{
}

void WavTools::Writer::write_header(std::FILE *file_, uint32_t data_size)
{
    size_t INT_BLOCK = 1;
    auto write_u16 = [this, file_, INT_BLOCK](uint16_t value)
    {
        std::fwrite(&value, sizeof(uint16_t), INT_BLOCK, file_);
    };

    auto write_u32 = [this, file_, INT_BLOCK](uint32_t value)
    {
        std::fwrite(&value, sizeof(uint32_t), INT_BLOCK, file_);
    };

    size_t WORD_BLOCK = 4;
    auto write_id = [this, file_, WORD_BLOCK](const char *id)
    {
        std::fwrite(id, sizeof(char), WORD_BLOCK, file_);
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

/// @brief Copies metadata from buffer and save it to given path
/// @param filepath file location
/// @param buffer modified buffer
template <typename SampleType>
void WavTools::Writer::save(const std::string &filepath, const AudioBuffer<SampleType> &buffer)
{
    uint32_t data_size = buffer.num_samples() * buffer.num_channels() * bits_per_sample_;

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_(std::fopen(filepath.c_str(), "wb"), &std::fclose);

    if (!file_)
    {
        throw std::runtime_error("Cannot create file : " + filepath);
    }

    write_header(file_.get(), data_size);

    if (bits_per_sample_ == 32)
    {
        std::vector<float> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WavTools::convert_sample<float>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(float), temp.size(), file_.get());
    }
    else if (bits_per_sample_ == 16)
    {
        std::vector<int16_t> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WavTools::convert_sample<int16_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int16_t), temp.size(), file_.get());
    }
    else if (bits_per_sample_ == 8)
    {
        std::vector<uint8_t> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); i++)
        {
            temp[i] = WavTools::convert_sample<uint8_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(uint8_t), temp.size(), file_.get());
    }
}

/// @brief Explicit template definition
template void WavTools::Writer::save<float>(const std::string &, const AudioBuffer<float> &);

template void WavTools::Writer::save<int16_t>(const std::string &, const AudioBuffer<int16_t> &);

template void WavTools::Writer::save<uint8_t>(const std::string &, const AudioBuffer<uint8_t> &);
