#include <stdexcept>
#include <memory>
#include "audio_buffer.hpp"

/// @brief Clear elements from the buffer
template<typename SampleType>
void AudioBuffer<SampleType>::clear() {
    std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
}

/// Constructor
template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(size_t num_samples, size_t num_channels)
    : buffer_(std::make_unique<SampleType[]>(num_samples * num_channels))
    , num_samples_(num_samples)
    , num_channels_(num_channels) 
{
    clear(); // Init to silence audio
}

/// @brief Deafult Destructor
template<typename SampleType>
AudioBuffer<SampleType>::~AudioBuffer() = default;

/// @brief Move Assignment
template<typename SampleType>
AudioBuffer<SampleType>& AudioBuffer<SampleType>::operator=( AudioBuffer&& other) noexcept {
    if(this == other)
        return *this;
    buffer_ = std::move(other.buffer_);
    num_samples_ = other.num_samples_;
    num_channels_ = other.num_channels_;

    other.num_channels_ = 0;
    other.num_samples_ = 0;
    return *this;
} 

/// @brief Move Constructor
template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(AudioBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_))
    , num_channels_(other.num_channels_)
    , num_samples_(other.num_samples_)
{
    other.num_channels_ = 0;
    other.num_samples_ = 0;
}

/// @brief Get a particular sample 
///
/// Stereo :[ L0 R0 L1 R1 ... ] | Mono : [ D0 D1 ... ]
/// @return Music data sample at given index on given channel
template<typename SampleType>
SampleType& AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) {
    static_assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

/// @brief Get a particular sample
template<typename SampleType>
const SampleType& AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) const {
    static_assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}
