# pymcc_lidar.pxd

cdef extern from "pymccp.h" nogil:
    ctypedef signed int int32_t;
    int * pymcc_classify(double *x, double *y, double *z, int32_t n, double scaleDomain2Spacing, double curvatureThreshold);
    double * pymcc_pass(double *x, double *y, double *z, int32_t n, double scaleDomainSpacing);

