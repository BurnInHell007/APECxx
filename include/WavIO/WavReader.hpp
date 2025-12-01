#ifndef WAV_READER_HPP_ 
#define WAV_READER_HPP_
/// WAV READER HEADER FILE
#include "AudioBuffer.hpp"
#include "WavIO/WavCommon.hpp"

namespace WavTools
{
    class Reader
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
        explicit Reader(const std::string &filename); // to avoid implict type conversion when single argument is passed it would typecast to that class type

        template <typename SampleType>
        AudioBuffer<SampleType> read();

        uint32_t sample_rate() const { return sample_rate_; }
        uint16_t num_channels() const { return num_channels_; }
        uint16_t bits_per_sample() const { return bits_per_sample_; }
        uint32_t num_samples() const { return num_samples_; }
    };
}

#endif // WAV_READER_HPP_
