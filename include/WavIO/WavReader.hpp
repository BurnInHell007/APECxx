#ifndef WAV_READER_HPP_ 
#define WAV_READER_HPP_

#include "AudioBuffer.hpp"
#include "SampleConversion.hpp"

namespace audio
{
    class WavReader
    {
    public:
        explicit WavReader(const std::string &filename);
        ~WavReader() = default;

        // Read entire file into buffer
        template <typename SampleType>
        AudioBuffer<SampleType> read();

        // Getters
        uint32_t sample_rate() const { return sample_rate_; }
        uint16_t num_channels() const { return num_channels_; }
        uint16_t bits_per_sample() const { return bits_per_sample_; }
        uint32_t num_samples() const { return num_samples_; }
        float duration() const { return num_samples_ / static_cast<float>(sample_rate_); }

    private:
        void read_header();
        uint16_t read_u16();
        uint32_t read_u32();
        void read_chunk_id(const char *expected);

        std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
        uint32_t sample_rate_;
        uint16_t num_channels_;
        uint16_t bits_per_sample_;
        uint32_t num_samples_;
        size_t data_start_pos_;
    };

    // Template implementation
    // This removes explicit template declartion
    template <typename SampleType>
    AudioBuffer<SampleType> WavReader::read()
    {
        // Seek to start of audio data
        std::fseek(file_.get(), static_cast<long>(data_start_pos_), SEEK_SET);

        AudioBuffer<SampleType> buffer(num_samples_, num_channels_);
        size_t total_samples = num_samples_ * num_channels_;

        if (bits_per_sample_ == 8)
        {
            // 8-bit samples (unsigned, 128 = silence)
            std::vector<uint8_t> temp(total_samples);
            std::fread(temp.data(), sizeof(uint8_t), total_samples, file_.get());

            for (size_t i = 0; i < total_samples; ++i)
            {
                // Convert unsigned 8-bit to signed 16-bit first
                int16_t signed_sample = (static_cast<int16_t>(temp[i]) - 128) * 256;
                buffer.data()[i] = convert_sample<SampleType>(signed_sample);
            }
        }
        else if (bits_per_sample_ == 16)
        {
            // 16-bit samples
            std::vector<int16_t> temp(total_samples);
            std::fread(temp.data(), sizeof(int16_t), total_samples, file_.get());

            for (size_t i = 0; i < total_samples; ++i)
            {
                buffer.data()[i] = convert_sample<SampleType>(temp[i]);
            }
        }
        else if (bits_per_sample_ == 24)
        {
            // 24-bit samples (3 bytes each)
            std::vector<uint8_t> raw_data(total_samples * 3);
            std::fread(raw_data.data(), 1, raw_data.size(), file_.get());

            for (size_t i = 0; i < total_samples; ++i)
            {
                int32_t sample_24bit = int24::read(&raw_data[i * 3]);
                buffer.data()[i] = convert_sample<SampleType>(sample_24bit);
            }
        }
        else if (bits_per_sample_ == 32)
        {
            // 32-bit samples (could be int or float)
            std::vector<int32_t> temp(total_samples);
            std::fread(temp.data(), sizeof(int32_t), total_samples, file_.get());

            for (size_t i = 0; i < total_samples; ++i)
            {
                buffer.data()[i] = convert_sample<SampleType>(temp[i]);
            }
        }
        else
        {
            throw std::runtime_error("Unsupported bit depth: " + std::to_string(bits_per_sample_));
        }

        return buffer;
    }
} // namespace audio

#endif // WAV_READER_HPP_
