#include "AudioBuffer.hpp"

/*****************************
 * @brief Creates an AudioBuffer with silence
 * @tparam SampleType 
 * @param num_samples 
 * @param num_channels 
 ****************************/
template <typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(size_t num_samples, size_t num_channels)
    : buffer_(std::make_unique<SampleType[]>(num_samples * num_channels)), num_samples_(num_samples), num_channels_(num_channels)
{
    clear();
}

/*****************************
 * @brief Functor to obtain a given sample in given channel
 * @tparam SampleType 
 * @param sample 
 * @param channel 
 * @return value at given sample in given channel
 ****************************/
template <typename SampleType>
SampleType &AudioBuffer<SampleType>::operator()(size_t sample, size_t channel)
{
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

template <typename SampleType>
const SampleType &AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) const
{
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

/***************************
 * @brief Silence the AudioBuffer
 * @tparam SampleType 
 **************************/
template <typename SampleType>
void AudioBuffer<SampleType>::clear()
{
    std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
}

/// @brief Explicit template calls to avoid undefined refrence
template class AudioBuffer<float>;

template class AudioBuffer<int16_t>;

template class AudioBuffer<uint8_t>;
