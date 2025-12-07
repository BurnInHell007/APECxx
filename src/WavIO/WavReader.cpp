#include "WavIO/WavReader.hpp"

namespace audio {
    WavReader::WavReader(const std::string &filename)
        : file_(std::fopen(filename.c_str(), "rb"), &std::fclose), sample_rate_(0), num_channels_(0), bits_per_sample_(0), num_samples_(0), data_start_pos_(0)
    {
        if (!file_)
        {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        read_header();
    }

    void WavReader::read_header()
    {
        // Read RIFF header
        read_chunk_id("RIFF");
        uint32_t file_size = read_u32();
        (void)file_size; // Not used but part of spec
        read_chunk_id("WAVE");

        // Read fmt chunk
        read_chunk_id("fmt ");
        uint32_t fmt_size = read_u32();

        uint16_t audio_format = read_u16();
        if (audio_format != 1)
        { // 1 = PCM
            throw std::runtime_error("Only PCM format is supported (format code: " + std::to_string(audio_format) + ")");
        }

        num_channels_ = read_u16();
        sample_rate_ = read_u32();
        uint32_t byte_rate = read_u32();
        (void)byte_rate; // Not used
        uint16_t block_align = read_u16();
        (void)block_align; // Not used
        bits_per_sample_ = read_u16();

        // Skip any extra format bytes
        if (fmt_size > 16)
        {
            std::fseek(file_.get(), static_cast<long>(fmt_size - 16), SEEK_CUR);
        }

        // Find data chunk (there might be other chunks like LIST, INFO, etc.)
        while (true)
        {
            char chunk_id[4];
            if (std::fread(chunk_id, 1, 4, file_.get()) != 4)
            {
                throw std::runtime_error("Data chunk not found");
            }

            uint32_t chunk_size = read_u32();

            if (std::memcmp(chunk_id, "data", 4) == 0)
            {
                // Found data chunk
                size_t bytes_per_sample = bits_per_sample_ / 8;
                num_samples_ = chunk_size / (num_channels_ * bytes_per_sample);
                data_start_pos_ = std::ftell(file_.get());
                break;
            }
            else
            {
                // Skip unknown chunk
                std::fseek(file_.get(), static_cast<long>(chunk_size), SEEK_CUR);
            }
        }
    }

    uint16_t WavReader::read_u16()
    {
        uint16_t value;
        if (std::fread(&value, sizeof(value), 1, file_.get()) != 1)
        {
            throw std::runtime_error("Failed to read uint16");
        }
        return value; // Assumes little-endian system
    }

    uint32_t WavReader::read_u32()
    {
        uint32_t value;
        if (std::fread(&value, sizeof(value), 1, file_.get()) != 1)
        {
            throw std::runtime_error("Failed to read uint32");
        }
        return value; // Assumes little-endian system
    }

    void WavReader::read_chunk_id(const char *expected)
    {
        char id[4];
        if (std::fread(id, 1, 4, file_.get()) != 4)
        {
            throw std::runtime_error("Failed to read chunk ID");
        }
        if (std::memcmp(id, expected, 4) != 0)
        {
            throw std::runtime_error(
                std::string("Expected '") + expected + "' chunk, got '" +
                std::string(id, 4) + "'");
        }
    }
} // namespace audio


