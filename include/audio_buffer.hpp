#pragma once
#include <stdexcept>
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
