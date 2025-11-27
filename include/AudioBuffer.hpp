#ifndef AUDIO_BUFFER_HPP_
#define AUDIO_BUFFER_HPP_
/// AUDIO BUFFER CONTAINER IMPLEMENTATION
#include "project.h"

template <typename SampleType>
class AudioBuffer
{
private:
    std::unique_ptr<SampleType[]> buffer_;
    size_t num_samples_;
    size_t num_channels_;

public:
    AudioBuffer(size_t num_samples, size_t num_channels);
    ~AudioBuffer() = default;
    AudioBuffer(const AudioBuffer &other) = delete;
    AudioBuffer &operator=(const AudioBuffer &other) = delete;
    AudioBuffer(AudioBuffer &&other) noexcept = default;
    AudioBuffer &operator=(AudioBuffer &&other) noexcept = default;

    const SampleType &operator()(size_t sample, size_t channel) const;
    SampleType &operator()(size_t sample, size_t channel);
    const SampleType *data() const { return buffer_.get(); }
    SampleType *data() { return buffer_.get(); }

    void clear();

    size_t num_samples() const { return num_samples_; }
    size_t num_channels() const { return num_channels_; }
    size_t size_in_bytes() const { return num_samples_ * num_channels_ * sizeof(SampleType); }
};

#endif // AUDIO_BUFFER_HPP_
