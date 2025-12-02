#ifndef MIX_EFFECT_HPP_
#define MIX_EFFECT_HPP_
/// MIX_EFFECT INHERITING AUDIO_EFFECT
#include "project.h"
#include "Effects/AudioEffect.hpp"

template <typename SampleType>
class MixEffect : public AudioEffect<SampleType>
{
public:
    MixEffect(bool activatePan = false, float panValue = 0.0f);
    ~MixEffect() = default;
    MixEffect(const MixEffect &other) = default;
    MixEffect(MixEffect &&other) = default;
    MixEffect &operator=(const MixEffect &other) = default;
    MixEffect &operator=(MixEffect &&other) = default;

    void process(AudioBuffer<SampleType> &buffer) override;
private:
    bool activatePan;
    float panValue;
};
#endif // MIX_EFFECT_HPP_
