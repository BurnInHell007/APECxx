#include "AudioBuffer.hpp"
#include "WavIO/WavWriter.hpp"

namespace audio {
    WavWriter::WavWriter(const std::string &filename, uint32_t sample_rate,
                         uint16_t num_channels, uint16_t bits_per_sample)
        : file_(std::fopen(filename.c_str(), "wb"), &std::fclose), sample_rate_(sample_rate), num_channels_(num_channels), bits_per_sample_(bits_per_sample)
    {
        if (!file_)
        {
            throw std::runtime_error("Cannot create file: " + filename);
        }

        if (bits_per_sample != 8 && bits_per_sample != 16 &&
            bits_per_sample != 24 && bits_per_sample != 32)
        {
            throw std::invalid_argument("Bit depth must be 8, 16, 24, or 32");
        }
    }

    void WavWriter::write_header(uint32_t data_size)
    {
        // RIFF header
        write_id("RIFF");
        write_u32(36 + data_size); // File size - 8
        write_id("WAVE");

        // fmt chunk
        write_id("fmt ");
        write_u32(16); // fmt chunk size (PCM)
        write_u16(1);  // Audio format (1 = PCM)
        write_u16(num_channels_);
        write_u32(sample_rate_);

        uint32_t byte_rate = sample_rate_ * num_channels_ * bits_per_sample_ / 8;
        write_u32(byte_rate);

        uint16_t block_align = num_channels_ * bits_per_sample_ / 8;
        write_u16(block_align);

        write_u16(bits_per_sample_);

        // data chunk header
        write_id("data");
        write_u32(data_size);
    }

    void WavWriter::write_u16(uint16_t value)
    {
        std::fwrite(&value, sizeof(value), 1, file_.get());
    }

    void WavWriter::write_u32(uint32_t value)
    {
        std::fwrite(&value, sizeof(value), 1, file_.get());
    }

    void WavWriter::write_id(const char *id)
    {
        std::fwrite(id, 1, 4, file_.get());
    }
} // namespace audio
