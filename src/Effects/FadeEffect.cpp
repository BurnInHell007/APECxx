/// FadeEffect Implementation
#include "Effects/FadeEffect.hpp"

/// @brief FadeEffect Instance to introduce FadeIn or FadeOut effects
/// @tparam SampleType 
/// @param start FadeIn: start = 0.0 | FadeOut: start = 1.0
/// @param end FadeIn: end = 1.0 | FadeOut : end = 0.0
/// @param samples Number of samples which undergo this effect
template <typename SampleType>
FadeEffect<SampleType>::FadeEffect(float start, float end, size_t samples)
    : startFactor_(start)
    , endFactor_(end)
    , no_of_samples(samples)
{}

/// TODO:
/// FIXME: Effects are only applied to the begining of the audio Buffer
/// @brief FadeIn or FadeOut Effect | s=0,e=1 (FadeIn) | s=1,e=0 (FadeOut)
template <typename SampleType>
void FadeEffect<SampleType>::process(AudioBuffer<SampleType> &buffer)
{
    // alternative L and R values
    // Gain[n] = start + (end - start)* n / N
    // y[n] = x[n].Gain[n]
    // FadeIn  : start = 0.0, end = 1.0
    // FadeOut : start = 1.0, end = 0.0

    for (size_t channel = 0; channel < buffer.num_channels(); channel++)
    {
        for (size_t sample = 0; sample < std::min(buffer.num_samples(), no_of_samples); sample++)
        {
            buffer.data()[channel + 2 * sample] = buffer.data()[channel + 2 * sample] * (startFactor_ + (endFactor_ - startFactor_) * (static_cast<float>(std::min(no_of_samples, buffer.num_samples())) / buffer.num_samples()));
        }
    }
}

template class FadeEffect<float>;
template class FadeEffect<int16_t>;
template class FadeEffect<int8_t>;
