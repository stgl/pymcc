#include "pymccp.h"
#include <iostream>
#include <boost/cstdint.hpp>

using namespace std;

int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold, int maxSplinePoints, subSamplingType sampling) {

  uint8_t *classification = mcc_classify(x, y, z, n, scaleDomain2Spacing, curvatureThreshold, maxSplinePoints, sampling);
  int *intClassification = new int[n];
  for(int i = 0; i < n; i++) {
	  intClassification[i] = (int)classification[i];
  }

  return intClassification;

}

double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing, int maxSplinePoints, subSamplingType sampling) {

  return mcc_pass(x, y, z, n, scaleDomainSpacing, maxSplinePoints, sampling);

}
