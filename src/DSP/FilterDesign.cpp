#include "DSP/FilterDesign.hpp"

BiQuadCoefficients FilterDesign::makeLowPass(double sampleRate, double cutOffFreq, double Q)
{
    // 1. Calculate intermediate variables
    double w0 = 2.0 * M_PI * cutOffFreq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * Q);
    double cosW0 = std::cos(w0);

    // 2. Calculate raw coefficients (Audio EQ Cookbook)
    double b0 = (1.0 - cosW0) / 2.0;
    double b1 = 1.0 - cosW0;
    double b2 = (1.0 - cosW0) / 2.0;
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosW0;
    double a2 = 1.0 - alpha;

    // 3. Normalize and return
    return normalize(b0, b1, b2, a0, a1, a2);
}

BiQuadCoefficients FilterDesign::makeHighPass(double sampleRate, double cutOffFreq, double Q)
{
    double w0 = 2.0 * M_PI * cutOffFreq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * Q);
    double cosW0 = std::cos(w0);

    double b0 = (1.0 + cosW0) / 2.0;
    double b1 = -(1.0 + cosW0);
    double b2 = (1.0 + cosW0) / 2.0;
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosW0;
    double a2 = 1.0 - alpha;

    return normalize(b0, b1, b2, a0, a1, a2);
}

BiQuadCoefficients FilterDesign::makePeaking(double sampleRate, double centerFreq, double Q, double gainDb)
{
    double w0 = 2.0 * M_PI * centerFreq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * Q);
    double cosW0 = std::cos(w0);

    // A is the linear amplitude factor (10^(dB/40))
    double A = std::pow(10.0, gainDb / 40.0);

    double b0 = 1.0 + alpha * A;
    double b1 = -2.0 * cosW0;
    double b2 = 1.0 - alpha * A;
    double a0 = 1.0 + alpha / A;
    double a1 = -2.0 * cosW0;
    double a2 = 1.0 - alpha / A;

    return normalize(b0, b1, b2, a0, a1, a2);
}

BiQuadCoefficients FilterDesign::makeBandPass(double sampleRate, double centerFreq, double Q)
{
    double w0 = 2.0 * M_PI * centerFreq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * Q);
    double cosW0 = std::cos(w0);

    double b0 = alpha;
    double b1 = 0.0;
    double b2 = -alpha;
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosW0;
    double a2 = 1.0 - alpha;

    return normalize(b0, b1, b2, a0, a1, a2);
}

BiQuadCoefficients FilterDesign::normalize(double b0, double b1, double b2, double a0, double a1, double a2)
{
    BiQuadCoefficients coeffs;
    // Pre-calculate inverse for speed (multiplication is faster than division)
    double invA0 = 1.0 / a0;

    coeffs.b0 = b0 * invA0;
    coeffs.b1 = b1 * invA0;
    coeffs.b2 = b2 * invA0;
    coeffs.a1 = a1 * invA0;
    coeffs.a2 = a2 * invA0;

    return coeffs;
}
