/// FadeEffect Implementation
#include "Effects/FadeEffect.hpp"

/************************************
 * @brief FadeEffect Instance to introduce FadeIn or FadeOut effects
 * @tparam SampleType
 * @param start float (0.0 <= start <= 1.0)
 * @param end   float (0.0 <= end <= 1.0)
 * @param samples Number of samples which undergo this effect
 ***********************************/
template <typename SampleType>
FadeEffect<SampleType>::FadeEffect(float start, float end, size_t samples)
    : startFactor_(start)
    , endFactor_(end)
    , no_of_samples(samples)
{}

/************************************
 * TODO:
 * FIXME: Effects can only be applied at the beginning or the audio buffer
 * @brief FadeIn or FadeOut Effects | s=0,e=1 (FadeIn) | s=1,e=0 (FadeOut)
 * 
 * @details
 * alternative L and R values
 * Gain[n] = start + (end - start)* n / N
 * y[n] = x[n].Gain[n]
 * FadeIn  : start = 0.0, end = 1.0
 * FadeOut : start = 1.0, end = 0.0
 * 
 * @tparam SampleType
 * @param buffer
 ***********************************/
template <typename SampleType>
void FadeEffect<SampleType>::process(AudioBuffer<SampleType> &buffer)
{

    for (size_t channel = 0; channel < buffer.num_channels(); channel++)
    {
        for (size_t sample = 0; sample < std::min(buffer.num_samples(), no_of_samples); sample++)
        {
            buffer.data()[channel + buffer.num_channels() * sample] = buffer.data()[channel + buffer.num_channels() * sample] * (startFactor_ + (endFactor_ - startFactor_) * (static_cast<float>(std::min(no_of_samples, buffer.num_samples())) / buffer.num_samples()));
        }
    }
}

template class FadeEffect<float>;
template class FadeEffect<int16_t>;
template class FadeEffect<uint8_t>;
