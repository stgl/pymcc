#ifndef MCC_H
#define MCC_H

#include <boost/cstdint.hpp>
// Interface to MCC library for Python (or C++) bindings

uint8_t * mcc_classify(double *x, double *y, double *z, int n, double scaleDomain2Spacing, double curvatureThreshold);
double * mcc_pass(double *x, double *y, double *z, int n, double scaleDomainSpacing);

#endif // MCC_H
