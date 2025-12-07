#pragma once

#include "project.h"

namespace audio
{
    /**
     * @brief Generic audio buffer for storing samples
     *
     * Template allows different sample types
     * (int16_t, int32_t, float, double)
     */
    template <typename SampleType>
    class AudioBuffer
    {
    private:
        std::unique_ptr<SampleType[]> buffer_;

        size_t num_samples_;

        size_t num_channels_;

        /**
         * @brief Error handling for out of bound access
         */
        void check_bounds(size_t sample_index, size_t channel) const
        {
            if (sample_index >= num_samples_)
                throw std::out_of_range("Sample index out of range");
            if (channel >= num_channels_)
                throw std::out_of_range("Channel index out of range");
        }

    public:
        /// Default constructor
        AudioBuffer() : num_samples_(0), num_channels_(0) {}

        /// Main constructor
        AudioBuffer(size_t num_samples, size_t num_channels)
            : buffer_(std::make_unique<SampleType[]>(num_samples * num_channels)), num_samples_(num_samples), num_channels_(num_channels)
        {
            if (num_samples_ == 0 || num_channels_ == 0)
            {
                throw std::invalid_argument("Number of samples and channels must be positive");
            }
            clear();
        }

        virtual ~AudioBuffer() = default;

        AudioBuffer(const AudioBuffer &other) = delete;

        AudioBuffer &operator=(const AudioBuffer &other) = delete;

        AudioBuffer(AudioBuffer &&other) noexcept = default;

        AudioBuffer &operator=(AudioBuffer &&other) noexcept = default;

        /**
         * @brief Access sample at (sample_index, channel)
         */
        SampleType &operator()(size_t sample_index, size_t channel)
        {
            check_bounds(sample_index, channel);
            return buffer_[sample_index * num_channels_ + channel];
        }

        /**
         * @brief Access sample at (sample_index, channel)
         */
        const SampleType &operator()(size_t sample_index, size_t channel) const
        {
            check_bounds(sample_index, channel);
            return buffer_[sample_index * num_channels_ + channel];
        }

        /**
         * @brief Get raw pointer to data
         */
        SampleType *data()
        {
            return buffer_.get();
        }

        /**
         * @brief Get raw pointer to data
         */
        const SampleType *data() const
        {
            return buffer_.get();
        }

        /**
         * @brief Check if buffer is initialized or not
         */
        bool empty() const
        {
            return (num_samples_ == 0 || num_channels_ == 0);
        }

        /**
         * @brief Silence the buffer
         */
        void clear()
        {
            if (buffer_)
                std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
        }

        /**
         * @brief Resize buffer (destroys existing data)
         */
        void resize(size_t new_num_samples, size_t new_num_channels)
        {
            if (new_num_samples == 0 || new_num_channels == 0)
            {
                throw std::invalid_argument("Number of samples and channels must be positive");
            }

            buffer_ = std::make_unique<SampleType[]>(new_num_samples * new_num_channels);
            num_samples_ = new_num_samples;
            num_channels_ = new_num_channels;
            clear();
        }

        size_t num_samples() const
        {
            return num_samples_;
        }
        size_t num_channels() const
        {
            return num_channels_;
        }
        size_t total_samples() const
        {
            return num_samples_ * num_channels_;
        }
        size_t size_in_bytes() const
        {
            return num_samples_ * num_channels_ * sizeof(SampleType);
        }

        /**
         * @brief Get a specific channel as a separate buffer
         */
        AudioBuffer get_channel(size_t channel) const
        {
            if (channel >= num_channels_)
            {
                throw std::out_of_range("Channel index out of range");
            }

            AudioBuffer result(num_samples_, 1);
            for (size_t i = 0; i < num_samples_; ++i)
            {
                result(i, 0) = (*this)(i, channel);
            }
            return result;
        }

        /**
         * @brief Set a specific channel from another buffer
         */
        void set_channel(size_t channel, const AudioBuffer &source)
        {
            if (channel >= num_channels_)
            {
                throw std::out_of_range("Channel index out of range");
            }
            if (source.num_channels_ != 1 || source.num_samples_ != num_samples_)
            {
                throw std::invalid_argument("Source must be mono with matching sample count");
            }

            for (size_t i = 0; i < num_samples_; ++i)
            {
                (*this)(i, channel) = source(i, 0);
            }
        }
    };
} // namespace audio
