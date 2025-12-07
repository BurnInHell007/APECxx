#pragma once

#include "Effects/AudioEffect.hpp"
#include "DSP/BiQuadFilter.hpp"
#include "DSP/FilterDesign.hpp"

namespace audio
{
    namespace effects
    {

        /**
         * Low-pass filter effect
         * Removes high frequencies (makes sound warmer/darker)
         */
        template <typename SampleType>
        class LowpassEffect : public AudioEffect<SampleType>
        {
        public:
            LowpassEffect(double sample_rate, double cutoff_freq, double q_factor = 0.707)
                : sample_rate_(sample_rate), cutoff_freq_(cutoff_freq), q_factor_(q_factor)
            {
                update_coefficients();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void reset() override
            {
                filter_.reset();
            }

            // Parameter setters
            void set_cutoff(double freq)
            {
                cutoff_freq_ = freq;
                update_coefficients();
            }

            void set_q_factor(double q)
            {
                q_factor_ = q;
                update_coefficients();
            }

            // Getters
            double cutoff() const { return cutoff_freq_; }
            double q_factor() const { return q_factor_; }

        private:
            void update_coefficients()
            {
                auto coeffs = dsp::FilterDesign::lowpass(sample_rate_, cutoff_freq_, q_factor_);
                filter_.set_coefficients(coeffs);
            }

            double sample_rate_;
            double cutoff_freq_;
            double q_factor_;
            dsp::BiquadFilter<SampleType> filter_;
        };

        /**
         * High-pass filter effect
         * Removes low frequencies (removes rumble/bass)
         */
        template <typename SampleType>
        class HighpassEffect : public AudioEffect<SampleType>
        {
        public:
            HighpassEffect(double sample_rate, double cutoff_freq, double q_factor = 0.707)
                : sample_rate_(sample_rate), cutoff_freq_(cutoff_freq), q_factor_(q_factor)
            {
                update_coefficients();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void reset() override
            {
                filter_.reset();
            }

            void set_cutoff(double freq)
            {
                cutoff_freq_ = freq;
                update_coefficients();
            }

            void set_q_factor(double q)
            {
                q_factor_ = q;
                update_coefficients();
            }

            double cutoff() const { return cutoff_freq_; }
            double q_factor() const { return q_factor_; }

        private:
            void update_coefficients()
            {
                auto coeffs = dsp::FilterDesign::highpass(sample_rate_, cutoff_freq_, q_factor_);
                filter_.set_coefficients(coeffs);
            }

            double sample_rate_;
            double cutoff_freq_;
            double q_factor_;
            dsp::BiquadFilter<SampleType> filter_;
        };

        /**
         * Band-pass filter effect
         * Only allows a specific frequency range
         */
        template <typename SampleType>
        class BandpassEffect : public AudioEffect<SampleType>
        {
        public:
            BandpassEffect(double sample_rate, double center_freq, double bandwidth)
                : sample_rate_(sample_rate), center_freq_(center_freq), bandwidth_(bandwidth)
            {
                update_coefficients();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void reset() override
            {
                filter_.reset();
            }

            void set_center_frequency(double freq)
            {
                center_freq_ = freq;
                update_coefficients();
            }

            void set_bandwidth(double bw)
            {
                bandwidth_ = bw;
                update_coefficients();
            }

            double center_frequency() const { return center_freq_; }
            double bandwidth() const { return bandwidth_; }

        private:
            void update_coefficients()
            {
                auto coeffs = dsp::FilterDesign::bandpass(sample_rate_, center_freq_, bandwidth_);
                filter_.set_coefficients(coeffs);
            }

            double sample_rate_;
            double center_freq_;
            double bandwidth_;
            dsp::BiquadFilter<SampleType> filter_;
        };

        /**
         * Parametric EQ band
         * Boost or cut a specific frequency range
         */
        template <typename SampleType>
        class ParametricEQBand : public AudioEffect<SampleType>
        {
        public:
            ParametricEQBand(double sample_rate, double center_freq,
                             double gain_db, double bandwidth)
                : sample_rate_(sample_rate), center_freq_(center_freq), gain_db_(gain_db), bandwidth_(bandwidth)
            {
                update_coefficients();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void reset() override
            {
                filter_.reset();
            }

            void set_frequency(double freq)
            {
                center_freq_ = freq;
                update_coefficients();
            }

            void set_gain(double gain_db)
            {
                gain_db_ = gain_db;
                update_coefficients();
            }

            void set_bandwidth(double bw)
            {
                bandwidth_ = bw;
                update_coefficients();
            }

            double frequency() const { return center_freq_; }
            double gain() const { return gain_db_; }
            double bandwidth() const { return bandwidth_; }

        private:
            void update_coefficients()
            {
                auto coeffs = dsp::FilterDesign::peaking_eq(
                    sample_rate_, center_freq_, gain_db_, bandwidth_);
                filter_.set_coefficients(coeffs);
            }

            double sample_rate_;
            double center_freq_;
            double gain_db_;
            double bandwidth_;
            dsp::BiquadFilter<SampleType> filter_;
        };

    } // namespace effects
} // namespace audio
