#ifndef MCC_H
#define MCC_H

#include <boost/cstdint.hpp>
#include "enums.h"
// Interface to MCC library for Python (or C++) bindings

uint8_t * mcc_classify(double *x, double *y, double *z, int n, double scaleDomain2Spacing, double curvatureThreshold, double pointDensityScaleFactor, subSamplingType sampling);
double * mcc_pass(double *x, double *y, double *z, int n, double scaleDomainSpacing, double pointDensityScaleFactor, subSamplingType sampling);

#endif // MCC_H
