#ifndef WAV_WRITER_HPP_
#define WAV_WRITER_HPP_
#include "audio_buffer.hpp"

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
}

class WavWriter
{
public:
    WavWriter(const std::string &filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample);

    template <typename SampleType>
    void write(const AudioBuffer<SampleType> &buffer);

private:
    void write_header(uint32_t data_size);

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
};

#endif
