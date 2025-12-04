/// FadeEffect Implementation
#include "Effects/FadeEffect.hpp"

/************************************
 * @brief FadeEffect Instance to introduce FadeIn or FadeOut effects
 * @tparam SampleType
 * @param start float (0.0 <= start <= 1.0)
 * @param end   float (0.0 <= end <= 1.0)
 * @param startPos Position to start effect from
 * @param samples Number of samples which undergo this effect
 ***********************************/
template <typename SampleType>
FadeEffect<SampleType>::FadeEffect(float start, float end, size_t startPos, size_t samples)
    : startFactor_(start), endFactor_(end), startPos_(startPos), no_of_samples(samples)
{
}

/************************************
 * @brief FadeIn or FadeOut Effects | s=0,e=1 (FadeIn) | s=1,e=0 (FadeOut)
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
    size_t num_samples = std::min(buffer.num_samples(), no_of_samples);
    size_t newStartPos = buffer.num_channels() * startPos_;
    size_t total_samples = std::min(buffer.num_samples() * buffer.num_channels(), buffer.num_channels() * num_samples + newStartPos);

    assert(newStartPos <= total_samples);
    float gainFactor = startFactor_ +
                       ((endFactor_ - startFactor_) *
                        static_cast<float>(num_samples) /
                        buffer.num_samples());

    for (size_t sample = newStartPos; sample < total_samples; sample++)
    {
        buffer.data()[sample] = static_cast<SampleType>(buffer.data()[sample] * gainFactor);

        /// Safety clipping
        if constexpr (std::is_floating_point<SampleType>::value)
        {
            if(buffer.data()[sample] > 1.0f)
                buffer.data()[sample] = 1.0f;
            if(buffer.data()[sample] < -1.0f)
                buffer.data()[sample] = -1.0f;
        }
        else if constexpr (std::is_integral<SampleType>::value)
        {
            float max_val = std::numeric_limits<SampleType>::max();
            float min_val = std::numeric_limits<SampleType>::lowest();
            buffer.data()[sample] = (buffer.data()[sample] > max_val) ? max_val : buffer.data()[sample];
            buffer.data()[sample] = (buffer.data()[sample] < min_val) ? min_val : buffer.data()[sample];
        }
    }
}

template class FadeEffect<float>;

template class FadeEffect<int16_t>;

template class FadeEffect<uint8_t>;
