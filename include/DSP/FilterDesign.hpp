#pragma once

#include "DSP/BiQuadFilter.hpp"

namespace audio
{
    namespace dsp
    {

        // Mathematical constants
        constexpr double PI = 3.14159265358979323846;
        constexpr double TWO_PI = 2.0 * PI;

        /**
         * Filter design utility class
         * Calculates biquad coefficients for various filter types
         * Based on Robert Bristow-Johnson's Audio EQ Cookbook
         */
        class FilterDesign
        {
        public:
            /**
             * Low-pass filter (Butterworth, 2nd order)
             * Allows frequencies below cutoff, attenuates above
             *
             * @param sample_rate Sample rate in Hz
             * @param cutoff_freq Cutoff frequency in Hz (-3dB point)
             * @param q_factor Quality factor (resonance), typically 0.707 for Butterworth
             */
            static BiquadCoefficients lowpass(double sample_rate, double cutoff_freq, double q_factor = 0.707)
            {
                validate_frequency(sample_rate, cutoff_freq);
                validate_q_factor(q_factor);

                double omega = TWO_PI * cutoff_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega / (2.0 * q_factor);

                BiquadCoefficients coeffs;
                coeffs.b0 = (1.0 - cos_omega) / 2.0;
                coeffs.b1 = 1.0 - cos_omega;
                coeffs.b2 = (1.0 - cos_omega) / 2.0;
                coeffs.a0 = 1.0 + alpha;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * High-pass filter (Butterworth, 2nd order)
             * Allows frequencies above cutoff, attenuates below
             *
             * @param sample_rate Sample rate in Hz
             * @param cutoff_freq Cutoff frequency in Hz (-3dB point)
             * @param q_factor Quality factor, typically 0.707 for Butterworth
             */
            static BiquadCoefficients highpass(double sample_rate, double cutoff_freq, double q_factor = 0.707)
            {
                validate_frequency(sample_rate, cutoff_freq);
                validate_q_factor(q_factor);

                double omega = TWO_PI * cutoff_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega / (2.0 * q_factor);

                BiquadCoefficients coeffs;
                coeffs.b0 = (1.0 + cos_omega) / 2.0;
                coeffs.b1 = -(1.0 + cos_omega);
                coeffs.b2 = (1.0 + cos_omega) / 2.0;
                coeffs.a0 = 1.0 + alpha;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * Band-pass filter (constant skirt gain)
             * Allows frequencies within a band, attenuates outside
             *
             * @param sample_rate Sample rate in Hz
             * @param center_freq Center frequency in Hz
             * @param bandwidth Bandwidth in Hz (measured at -3dB points)
             */
            static BiquadCoefficients bandpass(double sample_rate, double center_freq, double bandwidth)
            {
                validate_frequency(sample_rate, center_freq);

                double omega = TWO_PI * center_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega * std::sinh(std::log(2.0) / 2.0 * bandwidth * omega / sin_omega);

                BiquadCoefficients coeffs;
                coeffs.b0 = alpha;
                coeffs.b1 = 0.0;
                coeffs.b2 = -alpha;
                coeffs.a0 = 1.0 + alpha;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * Notch filter (band-stop)
             * Removes a narrow frequency band
             *
             * @param sample_rate Sample rate in Hz
             * @param center_freq Frequency to notch out in Hz
             * @param bandwidth Bandwidth of the notch in Hz
             */
            static BiquadCoefficients notch(double sample_rate, double center_freq, double bandwidth)
            {
                validate_frequency(sample_rate, center_freq);

                double omega = TWO_PI * center_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega * std::sinh(std::log(2.0) / 2.0 * bandwidth * omega / sin_omega);

                BiquadCoefficients coeffs;
                coeffs.b0 = 1.0;
                coeffs.b1 = -2.0 * cos_omega;
                coeffs.b2 = 1.0;
                coeffs.a0 = 1.0 + alpha;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * Peaking EQ (parametric EQ)
             * Boosts or cuts a frequency band
             *
             * @param sample_rate Sample rate in Hz
             * @param center_freq Center frequency in Hz
             * @param gain_db Gain in decibels (positive = boost, negative = cut)
             * @param bandwidth Bandwidth in octaves
             */
            static BiquadCoefficients peaking_eq(double sample_rate, double center_freq,
                                                 double gain_db, double bandwidth)
            {
                validate_frequency(sample_rate, center_freq);

                double A = std::pow(10.0, gain_db / 40.0); // Amplitude from dB
                double omega = TWO_PI * center_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega * std::sinh(std::log(2.0) / 2.0 * bandwidth * omega / sin_omega);

                BiquadCoefficients coeffs;
                coeffs.b0 = 1.0 + alpha * A;
                coeffs.b1 = -2.0 * cos_omega;
                coeffs.b2 = 1.0 - alpha * A;
                coeffs.a0 = 1.0 + alpha / A;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha / A;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * Low-shelf filter
             * Boosts or cuts all frequencies below a cutoff
             *
             * @param sample_rate Sample rate in Hz
             * @param cutoff_freq Cutoff frequency in Hz
             * @param gain_db Gain in decibels
             * @param slope Shelf slope (0.5 = gentle, 1.0 = steep)
             */
            static BiquadCoefficients low_shelf(double sample_rate, double cutoff_freq,
                                                double gain_db, double slope = 1.0)
            {
                validate_frequency(sample_rate, cutoff_freq);

                double A = std::pow(10.0, gain_db / 40.0);
                double omega = TWO_PI * cutoff_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / slope - 1.0) + 2.0);
                double beta = 2.0 * std::sqrt(A) * alpha;

                BiquadCoefficients coeffs;
                coeffs.b0 = A * ((A + 1.0) - (A - 1.0) * cos_omega + beta);
                coeffs.b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cos_omega);
                coeffs.b2 = A * ((A + 1.0) - (A - 1.0) * cos_omega - beta);
                coeffs.a0 = (A + 1.0) + (A - 1.0) * cos_omega + beta;
                coeffs.a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cos_omega);
                coeffs.a2 = (A + 1.0) + (A - 1.0) * cos_omega - beta;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * High-shelf filter
             * Boosts or cuts all frequencies above a cutoff
             *
             * @param sample_rate Sample rate in Hz
             * @param cutoff_freq Cutoff frequency in Hz
             * @param gain_db Gain in decibels
             * @param slope Shelf slope (0.5 = gentle, 1.0 = steep)
             */
            static BiquadCoefficients high_shelf(double sample_rate, double cutoff_freq,
                                                 double gain_db, double slope = 1.0)
            {
                validate_frequency(sample_rate, cutoff_freq);

                double A = std::pow(10.0, gain_db / 40.0);
                double omega = TWO_PI * cutoff_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / slope - 1.0) + 2.0);
                double beta = 2.0 * std::sqrt(A) * alpha;

                BiquadCoefficients coeffs;
                coeffs.b0 = A * ((A + 1.0) + (A - 1.0) * cos_omega + beta);
                coeffs.b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cos_omega);
                coeffs.b2 = A * ((A + 1.0) + (A - 1.0) * cos_omega - beta);
                coeffs.a0 = (A + 1.0) - (A - 1.0) * cos_omega + beta;
                coeffs.a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cos_omega);
                coeffs.a2 = (A + 1.0) - (A - 1.0) * cos_omega - beta;

                coeffs.normalize();
                return coeffs;
            }

            /**
             * All-pass filter (changes phase but not magnitude)
             * Used for phase correction or creating delays
             *
             * @param sample_rate Sample rate in Hz
             * @param center_freq Center frequency in Hz
             * @param q_factor Quality factor
             */
            static BiquadCoefficients allpass(double sample_rate, double center_freq, double q_factor = 0.707)
            {
                validate_frequency(sample_rate, center_freq);
                validate_q_factor(q_factor);

                double omega = TWO_PI * center_freq / sample_rate;
                double cos_omega = std::cos(omega);
                double sin_omega = std::sin(omega);
                double alpha = sin_omega / (2.0 * q_factor);

                BiquadCoefficients coeffs;
                coeffs.b0 = 1.0 - alpha;
                coeffs.b1 = -2.0 * cos_omega;
                coeffs.b2 = 1.0 + alpha;
                coeffs.a0 = 1.0 + alpha;
                coeffs.a1 = -2.0 * cos_omega;
                coeffs.a2 = 1.0 - alpha;

                coeffs.normalize();
                return coeffs;
            }

        private:
            static void validate_frequency(double sample_rate, double freq)
            {
                if (freq <= 0.0 || freq >= sample_rate / 2.0)
                {
                    throw std::invalid_argument(
                        "Frequency must be between 0 and Nyquist frequency (" + std::to_string(sample_rate / 2.0) + " Hz)");
                }
            }

            static void validate_q_factor(double q)
            {
                if (q <= 0.0)
                {
                    throw std::invalid_argument("Q factor must be positive");
                }
            }
        };

    } // namespace dsp
} // namespace audio
