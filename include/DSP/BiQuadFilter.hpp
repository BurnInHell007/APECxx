#pragma once

#include "project.h"

namespace audio
{
    namespace dsp
    {

        /**
         * Biquad filter coefficients
         * These define the filter's frequency response
         */
        struct BiquadCoefficients
        {
            double b0 = 1.0; // Feedforward coefficients
            double b1 = 0.0;
            double b2 = 0.0;
            double a0 = 1.0; // Feedback coefficients
            double a1 = 0.0;
            double a2 = 0.0;

            // Normalize coefficients (divide by a0)
            void normalize()
            {
                if (std::abs(a0) < 1e-10)
                {
                    a0 = 1.0; // Prevent division by zero
                }

                double inv_a0 = 1.0 / a0;
                b0 *= inv_a0;
                b1 *= inv_a0;
                b2 *= inv_a0;
                a1 *= inv_a0;
                a2 *= inv_a0;
                a0 = 1.0;
            }
        };

        /**
         * Biquad filter state for one channel
         * Stores previous input/output samples
         */
        struct BiquadState
        {
            double x1 = 0.0; // x[n-1]
            double x2 = 0.0; // x[n-2]
            double y1 = 0.0; // y[n-1]
            double y2 = 0.0; // y[n-2]

            void reset()
            {
                x1 = x2 = y1 = y2 = 0.0;
            }
        };

        /**
         * Biquad filter processor
         * Template allows different sample types (float, double)
         */
        template <typename SampleType>
        class BiquadFilter
        {
        public:
            BiquadFilter() = default;

            explicit BiquadFilter(const BiquadCoefficients &coeffs)
                : coeffs_(coeffs)
            {
                coeffs_.normalize();
            }

            // Set new coefficients
            void set_coefficients(const BiquadCoefficients &coeffs)
            {
                coeffs_ = coeffs;
                coeffs_.normalize();
            }

            // Process single sample for one channel
            SampleType process_sample(SampleType input, size_t channel = 0)
            {
                // Ensure we have state for this channel
                if (channel >= states_.size())
                {
                    states_.resize(channel + 1);
                }

                auto &state = states_[channel];

                // Apply biquad difference equation
                // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
                double x = static_cast<double>(input);
                double y = coeffs_.b0 * x + coeffs_.b1 * state.x1 + coeffs_.b2 * state.x2 - coeffs_.a1 * state.y1 - coeffs_.a2 * state.y2;

                // Update state (shift samples)
                state.x2 = state.x1;
                state.x1 = x;
                state.y2 = state.y1;
                state.y1 = y;

                return static_cast<SampleType>(y);
            }

            // Process entire buffer (interleaved stereo/mono)
            void process_buffer(SampleType *buffer, size_t num_samples, size_t num_channels)
            {
                for (size_t sample = 0; sample < num_samples; ++sample)
                {
                    for (size_t ch = 0; ch < num_channels; ++ch)
                    {
                        size_t index = sample * num_channels + ch;
                        buffer[index] = process_sample(buffer[index], ch);
                    }
                }
            }

            // Reset filter state (clear history)
            void reset()
            {
                for (auto &state : states_)
                {
                    state.reset();
                }
            }

            // Get current coefficients
            const BiquadCoefficients &coefficients() const
            {
                return coeffs_;
            }

        private:
            BiquadCoefficients coeffs_;
            std::vector<BiquadState> states_; // One state per channel
        };

    } // namespace dsp
} // namespace audio
