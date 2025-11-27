#ifndef WAVTOOLS_COMMON_HPP_
#define WAVTOOLS_COMMON_HPP_
#include "project.h"

namespace WavTools
{
    /// Inlining these functions so that the conflict caused when 2 objects forms (i.e wavreader and wavwriter) are removed by
    // relaxing ODR one defination rule
    /// Common Functions to convert the sample type
    template <typename T>
    inline T convert_sample(int16_t sample)
    {
        return sample;
    }

    template <>
    inline float convert_sample<float>(int16_t sample)
    {
        return static_cast<float>(sample) / 32768.0f;
    }

    template <>
    inline int8_t convert_sample<int8_t>(int16_t sample)
    {
        return static_cast<int8_t>(sample / 256);
    }

    template <typename T>
    inline T convert_sample(float sample)
    {
        return sample;
    }

    template <>
    inline int16_t convert_sample<int16_t>(float sample)
    {
        return static_cast<int16_t>(sample * 32768);
    }

    template <>
    inline int8_t convert_sample<int8_t>(float sample)
    {
        return static_cast<int8_t>(sample * 256);
    }

    template <typename T>
    inline T convert_sample(int8_t sample)
    {
        return sample;
    }
    /// Common Functions to convert the sample type
};

#endif // WAVTOOLS_COMMON_HPP_
