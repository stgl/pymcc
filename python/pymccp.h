#ifndef PYMCCP_H
#define PYMCCP_H

typedef signed int int32_t;

int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold);
double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing);

#endif // PYMCCP_H
