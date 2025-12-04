/**
 * Class that does the math
 * a0*y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
 */
#ifndef BIQUAD_FILTER_HPP_
#define BIQUAD_FILTER_HPP_

#include "project.h"

/***************************
 * @brief Holds the magic numbers that decides the filter type
 **************************/
struct BiQuadCoefficients
{
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a1 = 0.0, a2 = 0.0; // assuming normalization, a0 = 1.0
};

/************************
 * @brief The blueprint for various filters
 * @tparam SampleType should be float or double
 ***********************/
template <typename SampleType>
class BiQuadFilter
{
public:
    BiQuadFilter() = default;
    ~BiQuadFilter() = default;

    /*********************
     * @brief Set BiQuadCoeffiecents to use these filters
     * @param coeffs 
     ********************/
    void setCoefficients(const BiQuadCoefficients &coeffs);
    
    /**********************
     * @brief Reset history
     *********************/
    void reset();
    
    /**********************
     * @brief Process a single sample (Direct Form II Transposed)
     * @param input 
     * @return transformed output
     *********************/
    SampleType process(SampleType input);

private:
    BiQuadCoefficients coeffs_;
    // State variables
    double z1_ = 0.0;
    double z2_ = 0.0;
};

#endif // BIQUAD_FILTER_HPP_
