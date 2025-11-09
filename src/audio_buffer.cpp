#include "../include/audio_buffer.hpp"
#include <stdexcept>
#include <memory>

template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(size_t num_samples, size_t num_channels)
    : buffer_(std::make_unique<SampleType[]>(num_samples * num_channels))
    , num_samples_(num_samples)
    , num_channels_(num_channels)
{
    clear(); // Initialize to silence
}

void assert(bool value);

template<typename SampleType>
SampleType& AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) {
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

void assert(bool value) {
    if(!value) {
        throw std::runtime_error("Assertion failed!");
    }
    return;
}

template<typename SampleType>
void AudioBuffer<SampleType>::clear(){
    std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
}

template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(AudioBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_))
    , num_samples_(other.num_samples_)
    , num_channels_(other.num_channels_)
{
    other.num_samples_ = 0;
    other.num_channels_ = 0;
}
