#include <memory>

template<typename SampleType>
class AudioBuffer {
public:
    AudioBuffer(size_t num_samples, size_t num_channels);
    
    // Rule of 5 (C++ resource management)
    ~AudioBuffer();                                       // Destructor
    AudioBuffer(const AudioBuffer& other);                // Copy constructor
    AudioBuffer& operator=(const AudioBuffer& other);     // Copy assignment
    AudioBuffer(AudioBuffer&& other) noexcept;            // Move constructor
    AudioBuffer& operator=(AudioBuffer&& other) noexcept; // Move assignment
    
    // Access Samples
    SampleType& operator()(size_t sample_index, size_t channel);
    const SampleType& operator()(size_t sample_index, size_t channel) const;

    // Get raw pointers (for interop with C libraries)
    SampleType* data() { return buffer_.get(); }
    const SampleType* data() const { return buffer_.get(); }
    
    // Metadata
    size_t num_samples() const { return num_samples_; }
    size_t num_channels() const { return num_channels_; }
    size_t size_in_byte() const { return num_samples_ * num_channels_ * sizeof(SampleType); }
    
    void clear();

private:
    std::unique_ptr<SampleType[]> buffer_; // actual data
    size_t num_samples_;                   // samples per channel
    size_t num_channels_;                  // mono / stereo
};

template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(size_t num_samples, size_t num_channels)
    : buffer_{std::make_unique<SampleType[]>(num_samples * num_channels)}
    , num_samples_{num_samples}
    , num_channels_{num_channels}
{
    clear(); // Initialize to silence
}

template<typename SampleType>
SampleType& AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) {
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

template<typename SampleType>
void AudioBuffer<SampleType>::clear(){
    std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
}

template<typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(AudioBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_))
    , num_samples_{other.num_samples_}
    , num_channels_{other.num_channels_}
{
    other.num_samples_ = 0;
    other.num_channels_ = 0;
}
