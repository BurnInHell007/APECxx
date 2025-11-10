#pragma once
#ifndef WAV_READER_HPP_
#define WAV_READER_HPP_

#include <cstdint>
#include <memory>
#include "audio_buffer.hpp"

class WavReader {
    public:
    // RAII: Open file in constructor
    explicit WavReader(const std::string& filename);

    // Read entire file into an AudioBuffer
    template<typename SampleType>
    AudioBuffer<SampleType> read();

    // getters for file properties
    uint32_t sample_rate() const { return sample_rate_; }
    uint16_t num_channels() const { return num_channels_; }
    uint16_t bits_per_sample() const { return bits_per_sample_; }
    uint32_t num_samples() const { return num_samples_; }

    private:
    // Parse RIFF/fmt /data chunks from header
    void read_header();

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
    uint32_t num_samples_;
    size_t data_start_pos_;
};

#endif
