# pymcc_lidar.pxd

cdef extern from "pymccp.h" nogil:
    ctypedef signed int int32_t;
    ctypedef enum subSamplingType: NONE, EQUAL_INTERVAL, RANDOM
    int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold, double pointDensityScaleFactor, subSamplingType sampling);
    double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing, double pointDensityScaleFactor, subSamplingType sampling);
