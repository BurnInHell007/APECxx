/// GainEffect Implementation
#include "Effects/GainEffect.hpp"

/// @brief GainEffect Instance to introduce Amplification (both up and down)
/// @tparam SampleType 
/// @param gain GainFactor with which the amplification varies
template <typename SampleType>
GainEffect<SampleType>::GainEffect(float gain)
    : gainFactor_(gain) {}

/// @brief Gain Effect | gain > 1.0 Amplitude gain | gain < 1.0 Amplitude loss
/// @tparam SampleType 
/// @param buffer 
template <typename SampleType>
void GainEffect<SampleType>::process(AudioBuffer<SampleType> &buffer) 
{
    // alternative L and R values
    for (size_t channel = 0; channel < buffer.num_channels(); channel++)
    {
        for (size_t sample = 0; sample < buffer.num_samples(); sample++)
        {
            // channel + 2 * samples to get the left first then the right
            buffer.data()[channel + 2 * sample] = gainFactor_ * buffer.data()[channel + 2 * sample];
        }
    }
}

template class GainEffect<float>;
template class GainEffect<int16_t>;
template class GainEffect<int8_t>;
