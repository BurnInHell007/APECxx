#pragma once

#include "project.h"
#include "AudioBuffer.hpp"
#include "Effects/AudioEffect.hpp"

namespace audio
{
    namespace effects
    {

        /**
         * Gain effect - multiply all samples by a constant
         * Used for volume control
         */
        template <typename SampleType>
        class GainEffect : public AudioEffect<SampleType>
        {
        public:
            explicit GainEffect(float gain_linear = 1.0f)
                : gain_(gain_linear) {}

            void process(AudioBuffer<SampleType> &buffer) override
            {
                if (!this->is_enabled() || std::abs(gain_ - 1.0f) < 1e-6f)
                {
                    return; // No-op if gain is 1.0
                }

                SampleType *data = buffer.data();
                size_t total = buffer.total_samples();

                for (size_t i = 0; i < total; ++i)
                {
                    data[i] = static_cast<SampleType>(data[i] * gain_);
                }
            }

            void reset() override
            {
                // No state to reset
            }

            const char *name() const override { return "Gain"; }

            // Set gain in linear scale (1.0 = unity gain)
            void set_gain_linear(float gain)
            {
                gain_ = std::max(0.0f, gain); // Prevent negative gain
            }

            // Set gain in decibels
            void set_gain_db(float gain_db)
            {
                gain_ = std::pow(10.0f, gain_db / 20.0f);
            }

            float gain_linear() const { return gain_; }
            float gain_db() const { return 20.0f * std::log10(gain_); }

        private:
            float gain_;
        };

        /**
         * Fade effect - gradual volume change over time
         * Supports fade in, fade out, and crossfades
         */
        template <typename SampleType>
        class FadeEffect : public AudioEffect<SampleType>
        {
        public:
            enum class Type
            {
                FadeIn,  // 0 -> 1
                FadeOut, // 1 -> 0
                Custom   // Custom start/end
            };

            /**
             * @param sample_rate Sample rate in Hz
             * @param duration_seconds Duration of fade in seconds
             * @param type Fade type
             */
            FadeEffect(double sample_rate, double duration_seconds, Type type = Type::FadeIn)
                : sample_rate_(sample_rate), duration_seconds_(duration_seconds), type_(type)
            {
                update_parameters();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                if (!this->is_enabled())
                    return;

                size_t num_samples = buffer.num_samples();
                size_t num_channels = buffer.num_channels();

                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    float gain = calculate_gain_at_sample(sample);

                    for (size_t ch = 0; ch < num_channels; ++ch)
                    {
                        buffer(sample, ch) = static_cast<SampleType>(
                            buffer(sample, ch) * gain);
                    }
                }
            }

            void reset() override
            {
                // No state to reset (stateless per-buffer processing)
            }

            const char *name() const override { return "Fade"; }

            // Set fade duration
            void set_duration(double seconds)
            {
                duration_seconds_ = seconds;
                update_parameters();
            }

            // Set fade type
            void set_type(Type type)
            {
                type_ = type;
                update_parameters();
            }

            // Set custom start/end gain (for Type::Custom)
            void set_custom_range(float start_gain, float end_gain)
            {
                start_gain_ = start_gain;
                end_gain_ = end_gain;
                type_ = Type::Custom;
            }

            double duration() const { return duration_seconds_; }
            Type type() const { return type_; }

        private:
            void update_parameters()
            {
                fade_samples_ = static_cast<size_t>(duration_seconds_ * sample_rate_);

                switch (type_)
                {
                case Type::FadeIn:
                    start_gain_ = 0.0f;
                    end_gain_ = 1.0f;
                    break;
                case Type::FadeOut:
                    start_gain_ = 1.0f;
                    end_gain_ = 0.0f;
                    break;
                case Type::Custom:
                    // Use already set start_gain_ and end_gain_
                    break;
                }
            }

            float calculate_gain_at_sample(size_t sample_index) const
            {
                if (fade_samples_ == 0)
                    return end_gain_;

                // Linear interpolation
                float t = static_cast<float>(sample_index) / static_cast<float>(fade_samples_);
                t = std::min(1.0f, t); // Clamp to [0, 1]

                // Optional: Apply curve (linear, exponential, etc.)
                // For now, using linear fade
                return start_gain_ + (end_gain_ - start_gain_) * t;
            }

            double sample_rate_;
            double duration_seconds_;
            Type type_;
            size_t fade_samples_;
            float start_gain_;
            float end_gain_;
        };

        /**
         * Mix effect - combine (add) multiple audio sources
         * Can mix stereo to mono, or add background music, etc.
         */
        template <typename SampleType>
        class MixEffect : public AudioEffect<SampleType>
        {
        public:
            enum class MixMode
            {
                Add,          // Simple addition
                Average,      // Average (prevents clipping better)
                StereoToMono, // Convert stereo to mono
                MonoToStereo  // Duplicate mono to stereo
            };

            explicit MixEffect(MixMode mode = MixMode::Add, float mix_gain = 1.0f)
                : mode_(mode), mix_gain_(mix_gain)
            {
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                if (!this->is_enabled())
                    return;

                switch (mode_)
                {
                case MixMode::Add:
                case MixMode::Average:
                    // These require a second buffer to mix
                    // Usually handled by external code calling mix_with()
                    break;

                case MixMode::StereoToMono:
                    if (buffer.num_channels() == 2)
                    {
                        convert_stereo_to_mono(buffer);
                    }
                    break;

                case MixMode::MonoToStereo:
                    if (buffer.num_channels() == 1)
                    {
                        convert_mono_to_stereo(buffer);
                    }
                    break;
                }
            }

            /**
             * Mix another buffer into the current buffer
             * Both buffers must have same dimensions
             */
            void mix_with(AudioBuffer<SampleType> &dest,
                          const AudioBuffer<SampleType> &source)
            {
                if (dest.num_samples() != source.num_samples() ||
                    dest.num_channels() != source.num_channels())
                {
                    throw std::invalid_argument("Buffer dimensions must match for mixing");
                }

                size_t total = dest.total_samples();
                SampleType *dest_data = dest.data();
                const SampleType *src_data = source.data();

                switch (mode_)
                {
                case MixMode::Add:
                    for (size_t i = 0; i < total; ++i)
                    {
                        dest_data[i] = static_cast<SampleType>(
                            dest_data[i] + src_data[i] * mix_gain_);
                    }
                    break;

                case MixMode::Average:
                    for (size_t i = 0; i < total; ++i)
                    {
                        dest_data[i] = static_cast<SampleType>(
                            (dest_data[i] + src_data[i] * mix_gain_) * 0.5f);
                    }
                    break;

                default:
                    // Other modes don't use second buffer
                    break;
                }
            }

            void reset() override
            {
                // No state
            }

            const char *name() const override { return "Mix"; }

            void set_mode(MixMode mode) { mode_ = mode; }
            void set_mix_gain(float gain) { mix_gain_ = gain; }

            MixMode mode() const { return mode_; }
            float mix_gain() const { return mix_gain_; }

        private:
            void convert_stereo_to_mono(AudioBuffer<SampleType> &buffer)
            {
                // Average left and right channels
                size_t num_samples = buffer.num_samples();

                for (size_t i = 0; i < num_samples; ++i)
                {
                    SampleType left = buffer(i, 0);
                    SampleType right = buffer(i, 1);
                    SampleType mono = static_cast<SampleType>((left + right) * 0.5f);

                    buffer(i, 0) = mono;
                    buffer(i, 1) = mono; // Keep stereo format but with identical channels
                }
            }

            void convert_mono_to_stereo(AudioBuffer<SampleType> &buffer)
            {
                std::cout << buffer.num_channels() << std::endl;
                return;
                // This would require resizing the buffer, which is complex
                // For now, just duplicate the mono channel
                // (In practice, you'd create a new buffer with 2 channels)

                // Note: This is a simplified version that assumes buffer structure allows it
                // A full implementation would need to handle buffer resizing
            }

            MixMode mode_;
            float mix_gain_;
        };

        /**
         * Pan effect - control stereo positioning
         * -1.0 = full left, 0.0 = center, +1.0 = full right
         */
        template <typename SampleType>
        class PanEffect : public AudioEffect<SampleType>
        {
        public:
            explicit PanEffect(float pan = 0.0f)
                : pan_(std::max(-1.0f, std::min(1.0f, pan)))
            {
                update_gains();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                if (!this->is_enabled() || buffer.num_channels() != 2)
                {
                    return; // Only works on stereo
                }

                size_t num_samples = buffer.num_samples();

                for (size_t i = 0; i < num_samples; ++i)
                {
                    SampleType left = buffer(i, 0);
                    SampleType right = buffer(i, 1);

                    buffer(i, 0) = static_cast<SampleType>(left * left_gain_);
                    buffer(i, 1) = static_cast<SampleType>(right * right_gain_);
                }
            }

            void reset() override
            {
                // No state
            }

            const char *name() const override { return "Pan"; }

            void set_pan(float pan)
            {
                pan_ = std::max(-1.0f, std::min(1.0f, pan));
                update_gains();
            }

            float pan() const { return pan_; }

        private:
            void update_gains()
            {
                // Constant power panning (maintains perceived volume)
                float angle = (pan_ + 1.0f) * 0.25f * 3.14159265359f; // Map to [0, π/2]
                left_gain_ = std::cos(angle);
                right_gain_ = std::sin(angle);
            }

            float pan_;
            float left_gain_;
            float right_gain_;
        };

    } // namespace effects
} // namespace audio
