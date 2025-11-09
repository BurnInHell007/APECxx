#pragma once
#include <string>
#include <cstdint>
#include "audio_buffer.hpp"

class WavWriter {
public:
    WavWriter(const std::string& filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample);

    template<typename SampleType>
    void write(const AudioBuffer<SampleType>& buffer);

private:
    void write_header(uint32_t data_size);

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
};
