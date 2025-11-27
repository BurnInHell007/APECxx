#ifndef GAIN_EFFECT_HPP_
#define GAIN_EFFECT_HPP_
/// GAIN_EFFECT INHERITING AUDIO_EFFECT
#include "project.h"
#include "Effects/AudioEffect.hpp"

template <typename SampleType>
class GainEffect : public AudioEffect<SampleType>
{
public:
    GainEffect(float gain);
    ~GainEffect() = default;
    GainEffect(const GainEffect &other) = default;
    GainEffect(GainEffect &&other) = default;
    GainEffect &operator=(const GainEffect &other) = default;
    GainEffect &operator=(GainEffect &&other) = default;

    void process(AudioBuffer<SampleType> &buffer) override;

private:
    float gainFactor_;

};

#endif // GAIN_EFFECT_HPP_
