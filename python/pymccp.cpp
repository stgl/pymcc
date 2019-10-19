#include "pymccp.h"
#include "mcc.h"
#include <iostream>
#include <boost/cstdint.hpp>

using namespace std;

int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold, double pointDensityScaleFactor) {

  uint8_t *classification = mcc_classify(x, y, z, n, scaleDomain2Spacing, curvatureThreshold, pointDensityScaleFactor);
  int *intClassification = new int[n];
  for(int i = 0; i < n; i++) {
	  intClassification[i] = (int)classification[i];
  }

  return intClassification;

}

double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing, double pointDensityScaleFactor) {

  return mcc_pass(x, y, z, n, scaleDomainSpacing, pointDensityScaleFactor);

}
