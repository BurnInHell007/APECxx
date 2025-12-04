#include "Effects/GainEffect.hpp"

/*************************************
 * @brief GainEffect Instance to introduce Amplification (both up and down)
 * @tparam SampleType
 * @param gain GainFactor with which the amplification varies
 ************************************/
template <typename SampleType>
GainEffect<SampleType>::GainEffect(float gain)
    : gainFactor_(gain) {}

/************************************
 * @brief Increases/Decreases the amplitude of buffer input, with safety clipping enabled
 * @tparam SampleType float, int16_t, int8_t
 * @param buffer Reference buffer
 ***********************************/
template <typename SampleType>
void GainEffect<SampleType>::process(AudioBuffer<SampleType> &buffer) 
{
    size_t total_samples = buffer.num_channels() * buffer.num_samples();

    for (size_t sample = 0; sample < total_samples; sample++)
    {
        buffer.data()[sample] = static_cast<SampleType>(buffer.data()[sample] * gainFactor_);
        /// Safety Clipping
        if constexpr (std::is_floating_point<SampleType>::value)
        {
            if (buffer.data()[sample] > 1.0f)
                buffer.data()[sample] = 1.0f;
            if (buffer.data()[sample] < -1.0f)
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

template class GainEffect<float>;

template class GainEffect<int16_t>;

template class GainEffect<uint8_t>;
