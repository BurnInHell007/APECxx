#ifndef WAV_WRITER_HPP_
#define WAV_WRITER_HPP_
#include "AudioBuffer.hpp"
#include "WavIO/WavCommon.hpp"

namespace WavTools
{
    /// @brief Writer class to write down a wav filre
    class Writer
    {
    public:
        Writer (uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample);

        /// @brief Copies metadata from buffer and save it to given path
        /// @param filepath file location
        /// @param buffer modified buffer
        template <typename SampleType>
        void save(const std::string &filepath, const AudioBuffer<SampleType> &buffer);

    private:
        void write_header(std::FILE* file_, uint32_t data_size);

        uint32_t sample_rate_;
        uint16_t num_channels_;
        uint16_t bits_per_sample_;
    };
};

#endif
