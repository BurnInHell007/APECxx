#ifndef WAV_WRITER_HPP_
#define WAV_WRITER_HPP_

#include "AudioBuffer.hpp"
#include "SampleConversion.hpp"

namespace audio
{
    class WavWriter
    {
    public:
        WavWriter(const std::string &filename, uint32_t sample_rate,
                  uint16_t num_channels, uint16_t bits_per_sample);
        ~WavWriter() = default;

        // Write buffer to file
        template <typename SampleType>
        void write(const AudioBuffer<SampleType> &buffer);

    private:
        void write_header(uint32_t data_size);
        void write_u16(uint16_t value);
        void write_u32(uint32_t value);
        void write_id(const char *id);

        std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
        uint32_t sample_rate_;
        uint16_t num_channels_;
        uint16_t bits_per_sample_;
    };

    // Template implementation
    template <typename SampleType>
    void WavWriter::write(const AudioBuffer<SampleType> &buffer)
    {
        size_t bytes_per_sample = bits_per_sample_ / 8;
        uint32_t data_size = static_cast<uint32_t>(
            buffer.num_samples() * buffer.num_channels() * bytes_per_sample);

        write_header(data_size);

        size_t total_samples = buffer.num_samples() * buffer.num_channels();

        if (bits_per_sample_ == 8)
        {
            // 8-bit samples (unsigned)
            std::vector<uint8_t> temp(total_samples);
            for (size_t i = 0; i < total_samples; ++i)
            {
                int16_t signed_sample = convert_sample<int16_t>(buffer.data()[i]);
                temp[i] = static_cast<uint8_t>((signed_sample / 256) + 128);
            }
            std::fwrite(temp.data(), sizeof(uint8_t), total_samples, file_.get());
        }
        else if (bits_per_sample_ == 16)
        {
            // 16-bit samples
            std::vector<int16_t> temp(total_samples);
            for (size_t i = 0; i < total_samples; ++i)
            {
                temp[i] = convert_sample<int16_t>(buffer.data()[i]);
            }
            std::fwrite(temp.data(), sizeof(int16_t), total_samples, file_.get());
        }
        else if (bits_per_sample_ == 24)
        {
            // 24-bit samples (3 bytes each)
            std::vector<uint8_t> raw_data(total_samples * 3);

            for (size_t i = 0; i < total_samples; ++i)
            {
                int32_t sample_24bit = convert_sample<int32_t>(buffer.data()[i]);
                int24::write(sample_24bit, &raw_data[i * 3]);
            }

            std::fwrite(raw_data.data(), 1, raw_data.size(), file_.get());
        }
        else if (bits_per_sample_ == 32)
        {
            // 32-bit samples
            std::vector<int32_t> temp(total_samples);
            for (size_t i = 0; i < total_samples; ++i)
            {
                temp[i] = convert_sample<int32_t>(buffer.data()[i]);
            }
            std::fwrite(temp.data(), sizeof(int32_t), total_samples, file_.get());
        }
        else
        {
            throw std::runtime_error(
                "Unsupported bit depth for writing: " + std::to_string(bits_per_sample_));
        }

        std::fflush(file_.get());
    }
} // namespace audio

#endif
