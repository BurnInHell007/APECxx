#ifndef FADE_EFFECT_HPP_
#define FADE_EFFECT_HPP_
/// FADE_EFFECT INHERITING AUDIO_EFFECT
#include "project.h"
#include "Effects/AudioEffect.hpp"

template <typename SampleType>
class FadeEffect : public AudioEffect<SampleType>
{
public:
    FadeEffect(float start, float end, size_t samples);
    ~FadeEffect() = default;
    FadeEffect(const FadeEffect &other) = default;
    FadeEffect(FadeEffect &&other) = default;
    FadeEffect &operator=(const FadeEffect &other) = default;
    FadeEffect &operator=(FadeEffect &&other) = default;

    void process(AudioBuffer<SampleType> &buffer) override;

private:
    float startFactor_;
    float endFactor_;
    size_t no_of_samples;
};
#endif // FADE_EFFECT_HPP_
