/**
 * Functions to calculate the coefficients b0, b1, b2, a1, a2 | assuming a0 = 1
 * to normalize things
 */
#ifndef FILTER_DESIGN_HPP_
#define FILTER_DESIGN_HPP_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "project.h"
#include "DSP/BiQuadFilter.hpp"

/**********************
 * @brief Creates the values for coefficients for the filters
 * @param sampleRate (eg: 48000Hz)
 * @param cutOffFreq
 * @param centerFreq
 * @param Q Quality Factor
 * @param gainDb Gain in decibles (only for Peaking EQ)
 * @param A Linear Gain (for EQ only)
 * @param alpha Controls width/slope
 * @param w0 Angular Frequency
 *********************/
class FilterDesign
{
public:
    /*********************
     * @brief LPF, Removes frequencies above the cutoff
     ********************/
    static BiQuadCoefficients makeLowPass(double sampleRate, double cutOffFreq, double Q);
    /*********************
     * @brief HPF, Removes frequencies below the cutoff
     ********************/
    static BiQuadCoefficients makeHighPass(double sampleRate, double cutOffFreq, double Q);
    /*********************
    * @brief Peaking EQ (Bell Curve), boosts or cutoff a specific freq band
     ********************/
    static BiQuadCoefficients makePeaking(double sampleRate, double centerFreq, double Q, double gainDb);
    /*********************
     * @brief BPF - constant 0dB peak gain, useful to isolate a specific range of freq
     ********************/
    static BiQuadCoefficients makeBandPass(double sampleRate, double centerFreq, double Q);

private:
    /*********************
     * @brief To normalize a0 to 1.0
     ********************/
    static BiQuadCoefficients normalize(double b0, double b1, double b2, double a0, double a1, double a2);
};

#endif // FILTER_DESIGN_HPP_
