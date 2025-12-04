#include "DSP/BiQuadFilter.hpp"

template <typename SampleType>
void BiQuadFilter<SampleType>::setCoefficients(const BiQuadCoefficients& coeffs)
{
    coeffs_ = coeffs;
}

template <typename SampleType>
void BiQuadFilter<SampleType>::reset()
{
    z1_ = 0.0;
    z2_ = 0.0;
}

template <typename SampleType>
SampleType BiQuadFilter<SampleType>::process(SampleType input)
{
    double out = input * coeffs_.b0 + z1_;
    z1_ = input * coeffs_.b1 + z2_ - out * coeffs_.a1;
    z2_ = input * coeffs_.b2 - out * coeffs_.a2;
    return static_cast<SampleType>(out);
}

template class BiQuadFilter<float>;
template class BiQuadFilter<double>;
