#ifndef MCC_H
#define MCC_H

// Interface to MCC library for Python (or C++) bindings

unsigned short int * mcc(double *x, double *y, double *z, int n, double scaleDomain2Spacing, double curvatureThreshold);
double * mcc_pass(double *x, double *y, double *z, int n, double scaleDomainSpacing);

#endif // MCC_H
