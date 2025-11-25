#ifndef AUDIO_EFFECT_HPP_
#define AUDIO_EFFECT_HPP_
/// AUDIO EFFECT INTERFACE
#include "project.h"
#include "audio_buffer.hpp"

class AudioEffect 
{
private:

public:
    AudioEffect();
    ~AudioEffect() = default;
    AudioEffect(const AudioEffect &other) = delete;
    AudioEffect &operator=(const AudioEffect &other) = delete;
    AudioEffect(AudioEffect &&other) noexcept = default;
    AudioEffect &operator=(AudioEffect &&other) noexcept = default;
    
    void process(AudioBuffer& input);
};

#endif // AUDIO_EFFECT_HPP_