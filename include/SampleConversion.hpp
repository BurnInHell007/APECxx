#pragma once

#include "project.h"

namespace audio {

// 24-bit audio utilities
namespace int24 {
    // Read 24-bit sample from 3 bytes (little-endian) into int32_t
    inline int32_t read(const uint8_t *bytes)
    {
        // Combine 3 bytes into 32-bit integer
        int32_t value = (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];

        // Sign extend: if bit 23 is set, extend sign to bits 24-31
        if (value & 0x00800000)
        {
            value |= 0xFF000000;
        }

        return value;
    }

    // Write int32_t as 24-bit sample to 3 bytes (little-endian)
    inline void write(int32_t value, uint8_t *bytes)
    {
        // Clamp to 24-bit range
        value = std::max(-8388608, std::min(8388607, value));

        bytes[0] = value & 0xFF;
        bytes[1] = (value >> 8) & 0xFF;
        bytes[2] = (value >> 16) & 0xFF;
    }

    // Convert 24-bit int32 to float [-1.0, 1.0]
    inline float to_float(int32_t sample)
    {
        return sample / 8388608.0f;
    }

    // Convert float to 24-bit int32
    inline int32_t from_float(float sample)
    {
        // Clamp to [-1.0, 1.0]
        sample = std::max(-1.0f, std::min(1.0f, sample));

        int32_t value = static_cast<int32_t>(sample * 8388607.0f);

        // Clamp to 24-bit range
        return std::max(-8388608, std::min(8388607, value));
    }
} // namespace int24

// Generic sample conversion template
template <typename ToType, typename FromType>
inline ToType convert_sample(FromType sample);

// Conversion specialization

// int16 to float
template <>
inline float convert_sample<float, int16_t> (int16_t sample) {
    return sample / 32768.0f;
}

// int16 to double
template <>
inline double convert_sample<double, int16_t> (int16_t sample) {
    return sample / 32768.0;
}

// int32 to float (24 bit stored in int32)
template <>
inline float convert_sample<float, int32_t>(int32_t sample) {
    return int24::to_float(sample);
}

// int32 to double (24-bit stored in int32)
template <>
inline double convert_sample<double, int32_t>(int32_t sample)
{
    return sample / 8388608.0;
}

// float to int16
template <>
inline int16_t convert_sample<int16_t, float>(float sample)
{
    sample = std::max(-1.0f, std::min(1.0f, sample));
    return static_cast<int16_t>(sample * 32767.0f);
}

// double to int16
template <>
inline int16_t convert_sample<int16_t, double>(double sample)
{
    sample = std::max(-1.0, std::min(1.0, sample));
    return static_cast<int16_t>(sample * 32767.0);
}

// float to int32 (24-bit)
template <>
inline int32_t convert_sample<int32_t, float>(float sample)
{
    return int24::from_float(sample);
}

// double to int32 (24-bit)
template <>
inline int32_t convert_sample<int32_t, double>(double sample)
{
    sample = std::max(-1.0, std::min(1.0, sample));
    int32_t value = static_cast<int32_t>(sample * 8388607.0);
    return std::max(-8388608, std::min(8388607, value));
}

// Identity conversions (same type)
template <>
inline int16_t convert_sample<int16_t, int16_t>(int16_t sample)
{
    return sample;
}

template <>
inline int32_t convert_sample<int32_t, int32_t>(int32_t sample)
{
    return sample;
}

template <>
inline float convert_sample<float, float>(float sample)
{
    return sample;
}

template <>
inline double convert_sample<double, double>(double sample)
{
    return sample;
}

// Cross conversions between int types
template <>
inline int32_t convert_sample<int32_t, int16_t>(int16_t sample)
{
    return static_cast<int32_t>(sample) << 8; // Shift left 8 bits for 24-bit
}

template <>
inline int16_t convert_sample<int16_t, int32_t>(int32_t sample)
{
    return static_cast<int16_t>(sample >> 8); // Shift right 8 bits
}

// float to double
template <>
inline double convert_sample<double, float>(float sample)
{
    return static_cast<double>(sample);
}

// double to float
template <>
inline float convert_sample<float, double>(double sample)
{
    return static_cast<float>(sample);
}

} // namespace audio
