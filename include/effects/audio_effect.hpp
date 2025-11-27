#ifndef AUDIO_EFFECT_HPP_
#define AUDIO_EFFECT_HPP_
/// AUDIO EFFECT INTERFACE
#include "project.h"
#include "audio_buffer.hpp"

template <typename SampleType>
class AudioEffect 
{
public:
    AudioEffect();
    ~AudioEffect() = default;

    virtual void process(AudioBuffer<SampleType> &buffer) = 0;
};
#endif // AUDIO_EFFECT_HPP_
