/// MixEffect Implemenation
#include "Effects/MixEffect.hpp"

template <typename SampleType>
MixEffect<SampleType>::MixEffect(bool activatePan, float panValue)
    : activatePan(activatePan)
    , panValue(panValue)
{}

/// @brief Apply MixEffect on buffer:
///
///> PaneEffect then applies Left and Right pane 
/// 
///> MixEffect then change the buffer totally.
/// @tparam SampleType 
/// @param buffer reference to buffer to change
template <typename SampleType>
void MixEffect<SampleType>::process(AudioBuffer<SampleType> &buffer)
{
    /// Averaging the stereo channel will result in mixed mono audio
    /// Or can pan to left or right with a ratio p
    /// L[n] = L[n]*(1-p) | R[n] = R[n]*p
    if(activatePan)
    {
        /// Pan to right with ratio p and left with ratio 1-p
        for (size_t channel = 0; channel < buffer.num_channels(); channel++)
        {
            for (size_t sample = 0; sample < buffer.num_samples(); sample++)
            {
                buffer.data()[channel + 2 * sample] = static_cast<SampleType>(buffer.data()[channel + 2 * sample] * (channel ? panValue : (1.0f - panValue)));
            }
        }
    }
    else
    {
        size_t MONO_CHANNEL = 1;
        /// Left Channel comes first
        auto LEFT_CHANNEL = [](const size_t &sample){
            return 2 * sample;
        };
        /// Right Channel come later
        auto RIGHT_CHANNEL = [](const size_t &sample){
            return 2 * sample + 1;
        };
        /// Stereo to Mono
        /// num_samples / 2 becuse L and R channels will be merged
        AudioBuffer<SampleType> temp(buffer.num_samples()/2, MONO_CHANNEL);

        for (size_t sample = 0; sample < buffer.num_samples(); sample++)
        {
            temp.data()[sample] = (buffer.data()[LEFT_CHANNEL(sample)] + buffer.data()[RIGHT_CHANNEL(sample)])/2;
        }
            buffer = std::move(temp);
    }
}

template class MixEffect<float>;
template class MixEffect<int16_t>;
template class MixEffect<uint8_t>;

