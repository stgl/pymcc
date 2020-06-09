#ifndef PYMCCP_H
#define PYMCCP_H

#include "mcc.h"

typedef signed int int32_t;

int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold, int maxSplinePoints, subSamplingType sampling);
double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing, int maxSplinePoints, subSamplingType sampling);

#endif // PYMCCP_H
