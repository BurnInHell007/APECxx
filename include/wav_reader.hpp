#ifndef WAV_READER_HPP_
#define WAV_READER_HPP_
/// WAV READER HEADER FILE
#include "audio_buffer.hpp"

namespace WReader
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

class WavReader
{
private:
    void read_header(); // Parse RIFF/fmt /data chunk

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
    uint32_t num_samples_;
    size_t data_start_pos_;

public:
    explicit WavReader(const std::string &filename); // to avoid implict type conversion when single argument is passed it would typecast to that class type

    template <typename SampleType>
    AudioBuffer<SampleType> read();

    uint32_t sample_rate() const { return sample_rate_; }
    uint16_t num_channels() const { return num_channels_; }
    uint16_t bits_per_sample() const { return bits_per_sample_; }
    uint32_t num_samples() const { return num_samples_; }
};

#endif // WAV_READER_HPP_
