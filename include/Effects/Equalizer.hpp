#pragma once

#include "Effects/AudioEffect.hpp"
#include "Effects/FilterEffects.hpp"

namespace audio
{
    namespace effects
    {

        /**
         * EQ Band configuration
         */
        struct EQBand
        {
            double frequency; // Center frequency in Hz
            double gain_db;   // Gain in dB (positive = boost, negative = cut)
            double bandwidth; // Bandwidth in octaves
            bool enabled = true;

            EQBand(double freq, double gain, double bw = 1.0)
                : frequency(freq), gain_db(gain), bandwidth(bw) {}
        };

        /**
         * Multi-band parametric equalizer
         * Professional-grade EQ with multiple bands
         */
        template <typename SampleType>
        class Equalizer : public AudioEffect<SampleType>
        {
        public:
            explicit Equalizer(double sample_rate)
                : sample_rate_(sample_rate) {}

            /**
             * Add an EQ band
             * @return Index of the added band
             */
            size_t add_band(double frequency, double gain_db, double bandwidth = 1.0)
            {
                bands_.emplace_back(frequency, gain_db, bandwidth);
                filters_.push_back(std::make_unique<ParametricEQBand<SampleType>>(
                    sample_rate_, frequency, gain_db, bandwidth));
                return bands_.size() - 1;
            }

            /**
             * Remove a band by index
             */
            void remove_band(size_t index)
            {
                if (index < bands_.size())
                {
                    bands_.erase(bands_.begin() + index);
                    filters_.erase(filters_.begin() + index);
                }
            }

            /**
             * Update band parameters
             */
            void set_band_frequency(size_t index, double freq)
            {
                if (index < bands_.size())
                {
                    bands_[index].frequency = freq;
                    filters_[index]->set_frequency(freq);
                }
            }

            void set_band_gain(size_t index, double gain_db)
            {
                if (index < bands_.size())
                {
                    bands_[index].gain_db = gain_db;
                    filters_[index]->set_gain(gain_db);
                }
            }

            void set_band_bandwidth(size_t index, double bandwidth)
            {
                if (index < bands_.size())
                {
                    bands_[index].bandwidth = bandwidth;
                    filters_[index]->set_bandwidth(bandwidth);
                }
            }

            void set_band_enabled(size_t index, bool enabled)
            {
                if (index < bands_.size())
                {
                    bands_[index].enabled = enabled;
                }
            }

            /**
             * Process audio through all enabled bands
             */
            void process(AudioBuffer<SampleType> &buffer) override
            {
                for (size_t i = 0; i < filters_.size(); ++i)
                {
                    if (bands_[i].enabled)
                    {
                        filters_[i]->process(buffer);
                    }
                }
            }

            void reset() override
            {
                for (auto &filter : filters_)
                {
                    filter->reset();
                }
            }

            /**
             * Get band information
             */
            size_t num_bands() const { return bands_.size(); }
            const EQBand &get_band(size_t index) const { return bands_.at(index); }

            /**
             * Clear all bands
             */
            void clear()
            {
                bands_.clear();
                filters_.clear();
            }

            /**
             * Create standard 5-band EQ preset
             * Bass, Low-Mid, Mid, High-Mid, Treble
             */
            void create_5band_eq()
            {
                clear();
                add_band(100.0, 0.0, 1.0);   // Bass
                add_band(500.0, 0.0, 1.0);   // Low-Mid
                add_band(1000.0, 0.0, 1.0);  // Mid
                add_band(3000.0, 0.0, 1.0);  // High-Mid
                add_band(10000.0, 0.0, 1.0); // Treble
            }

            /**
             * Create standard 10-band graphic EQ
             */
            void create_10band_eq()
            {
                clear();
                // ISO standard center frequencies
                add_band(31.25, 0.0, 1.0);
                add_band(62.5, 0.0, 1.0);
                add_band(125.0, 0.0, 1.0);
                add_band(250.0, 0.0, 1.0);
                add_band(500.0, 0.0, 1.0);
                add_band(1000.0, 0.0, 1.0);
                add_band(2000.0, 0.0, 1.0);
                add_band(4000.0, 0.0, 1.0);
                add_band(8000.0, 0.0, 1.0);
                add_band(16000.0, 0.0, 1.0);
            }

        private:
            double sample_rate_;
            std::vector<EQBand> bands_;
            std::vector<std::unique_ptr<ParametricEQBand<SampleType>>> filters_;
        };

        /**
         * Simple 3-band EQ (Bass, Mid, Treble)
         * Easy to use for quick tone shaping
         */
        template <typename SampleType>
        class ThreeBandEQ : public AudioEffect<SampleType>
        {
        public:
            explicit ThreeBandEQ(double sample_rate)
                : sample_rate_(sample_rate), low_shelf_{200.0, 0.0}, mid_peak_{1000.0, 0.0, 1.0}, high_shelf_{5000.0, 0.0}
            {
                update_low_shelf();
                update_mid_peak();
                update_high_shelf();
            }

            void process(AudioBuffer<SampleType> &buffer) override
            {
                // Process through shelving filters and mid peak
                process_low_shelf(buffer);
                process_mid_peak(buffer);
                process_high_shelf(buffer);
            }

            void reset() override
            {
                low_shelf_filter_.reset();
                mid_peak_filter_.reset();
                high_shelf_filter_.reset();
            }

            // Simple interface: set bass, mid, treble in dB
            void set_bass(double gain_db)
            {
                bass_gain_ = gain_db;
                update_low_shelf();
            }

            void set_mid(double gain_db)
            {
                mid_gain_ = gain_db;
                update_mid_peak();
            }

            void set_treble(double gain_db)
            {
                treble_gain_ = gain_db;
                update_high_shelf();
            }

            double bass() const { return bass_gain_; }
            double mid() const { return mid_gain_; }
            double treble() const { return treble_gain_; }

        private:
            void update_low_shelf()
            {
                auto coeffs = dsp::FilterDesign::low_shelf(sample_rate_, 200.0, bass_gain_);
                low_shelf_filter_.set_coefficients(coeffs);
            }

            void update_mid_peak()
            {
                auto coeffs = dsp::FilterDesign::peaking_eq(sample_rate_, 1000.0, mid_gain_, 1.0);
                mid_peak_filter_.set_coefficients(coeffs);
            }

            void update_high_shelf()
            {
                auto coeffs = dsp::FilterDesign::high_shelf(sample_rate_, 5000.0, treble_gain_);
                high_shelf_filter_.set_coefficients(coeffs);
            }

            void process_low_shelf(AudioBuffer<SampleType> &buffer)
            {
                low_shelf_filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void process_mid_peak(AudioBuffer<SampleType> &buffer)
            {
                mid_peak_filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            void process_high_shelf(AudioBuffer<SampleType> &buffer)
            {
                high_shelf_filter_.process_buffer(buffer.data(), buffer.num_samples(), buffer.num_channels());
            }

            double sample_rate_;
            double bass_gain_ = 0.0;
            double mid_gain_ = 0.0;
            double treble_gain_ = 0.0;

            dsp::BiquadFilter<SampleType> low_shelf_filter_;
            dsp::BiquadFilter<SampleType> mid_peak_filter_;
            dsp::BiquadFilter<SampleType> high_shelf_filter_;

            // Store design parameters
            struct
            {
                double frequency = 200.0;
                double gain_db = 0.0;
            } low_shelf_;

            struct
            {
                double frequency = 1000.0;
                double gain_db = 0.0;
                double bandwidth = 1.0;
            } mid_peak_;

            struct
            {
                double frequency = 5000.0;
                double gain_db = 0.0;
            } high_shelf_;
        };

    } // namespace effects
} // namespace audio
