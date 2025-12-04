#include "AudioBuffer.hpp"
#include "WavIO/WavReader.hpp"

/*************************
 * @brief Creates a WAV Reader instance that can read any WAV file
 * @param filename
 ************************/
WavTools::Reader::Reader(const std::string &filename)
    : file_(std::fopen(filename.c_str(), "rb"), &std::fclose)
{
    if (!file_)
    {
        throw std::runtime_error("Cannot open file : " + filename);
    }
    read_header();
}

void WavTools::Reader::read_header()
{
    size_t WORD_BLOCK = 4; // to consider next 4 values as char
    auto read_chunk_id = [this, WORD_BLOCK](const char *expected)
    {
        char id[4];
        if (std::fread(id, sizeof(char), WORD_BLOCK, file_.get()) != 4)
            throw std::runtime_error("Failed to read chunk ID");
        if (std::memcmp(id, expected, 4) != 0)
            throw std::runtime_error(std::string("Expected '") + expected + "' chunk");
    };

    size_t INT_BLOCK = 1; // to consider next 1 value as int
    auto read_u16 = [this, INT_BLOCK]() -> uint16_t
    {
        uint16_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    auto read_u32 = [this, INT_BLOCK]() -> uint32_t
    {
        uint32_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    read_chunk_id("RIFF");
    uint32_t file_size = read_u32();
    read_chunk_id("WAVE");

    read_chunk_id("fmt ");
    uint32_t fmt_size = read_u32();
    uint16_t audio_format = read_u16();

    if (audio_format != 1)
    { // Only support PCM
        throw std::runtime_error("Only PCM format supported");
    }

    num_channels_ = read_u16();
    sample_rate_ = read_u32();
    uint32_t byte_rate = read_u32();
    uint32_t block_align = read_u16();
    bits_per_sample_ = read_u16();

    if (fmt_size > 16)
    {
        std::fseek(file_.get(), static_cast<long>(fmt_size - 16), SEEK_CUR);
    }

    while (true)
    {
        char chunk_id[4];
        if (std::fread(chunk_id, sizeof(char), WORD_BLOCK, file_.get()) != 4)
        {
            throw std::runtime_error("Data chunk not found");
        }

        uint32_t chunk_size = read_u32();

        if (std::memcmp(chunk_id, "data", 4) == 0)
        {
            // Found it
            num_samples_ = chunk_size / (num_channels_ * bits_per_sample_ / uint32_t(8));
            data_start_pos_ = static_cast<size_t>(std::ftell(file_.get()));
            break;
        }
        else
        {
            std::fseek(file_.get(), static_cast<long>(chunk_size), SEEK_CUR);
        }
    }
}

/*****************************
 * @brief Copy values into AudioBuffer
 * @tparam SampleType
 * @return Filled buffer
 ****************************/
template <typename SampleType>
AudioBuffer<SampleType> WavTools::Reader::read()
{
    std::fseek(file_.get(), data_start_pos_, SEEK_SET);

    AudioBuffer<SampleType> buffer(num_samples_, num_channels_);

    if (bits_per_sample_ == 32)
    {
        std::vector<float> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(float), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++)
        {
            buffer.data()[i] = WavTools::convert_sample<SampleType>(temp[i]);
        }
    }
    else if (bits_per_sample_ == 16)
    {
        std::vector<int16_t> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(int16_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++)
        {
            buffer.data()[i] = WavTools::convert_sample<SampleType>(temp[i]);
        }
    }
    else if (bits_per_sample_ == 8)
    {
        std::vector<uint8_t> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(uint8_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); i++)
        {
            buffer.data()[i] = WavTools::convert_sample<SampleType>(temp[i]);
        }
    }

    return std::move(buffer);
}

/**************************
 * @brief Explicit template defination
 *************************/
template AudioBuffer<float> WavTools::Reader::read<float>();

template AudioBuffer<int16_t> WavTools::Reader::read<int16_t>();

template AudioBuffer<uint8_t> WavTools::Reader::read<uint8_t>();
