import numpy as np
cimport pymcc_lidar
cimport numpy as np
from libc.stdlib cimport malloc, free


def classify(np.ndarray[double, ndim=2, mode='c'] xyz not None,
             scaleDomain2Spacing not None,
             curvatureThreshold not None):

    m_xyz = xyz.shape[0]
    n_xyz = xyz.shape[1]

    assert n_xyz == 3

    cdef int * classification;
    cdef double resolution = scaleDomain2Spacing;
    cdef double thresh = curvatureThreshold;
    cdef int32_t n = m_xyz;
    cdef double *x = <double *> malloc(n * sizeof(double));
    cdef double *y = <double *> malloc(n * sizeof(double));
    cdef double *z = <double *> malloc(n * sizeof(double));

    for i in range(n):
        x[i] = xyz[i, 0]
        y[i] = xyz[i, 1]
        z[i] = xyz[i, 2]

    with nogil:
        classification = pymcc_classify(x, y, z, n, resolution, thresh);
  
    cdef np.ndarray[int32_t, ndim=1] np_classification = np.empty(n, dtype=np.int32);
    for i in range(n):
        np_classification[i] = classification[i]

    return np_classification


def calculate_excess_height(np.ndarray[double, ndim=2, mode='c'] xyz not None,
                            scaleDomainSpacing not None):
  
    m_xyz = xyz.shape[0]
    n_xyz = xyz.shape[1]

    assert n_xyz == 3

    cdef double *h;
    cdef double resolution = scaleDomainSpacing;
    cdef int32_t n = m_xyz;
    cdef double *x = <double *> malloc(n * sizeof(double));
    cdef double *y = <double *> malloc(n * sizeof(double));
    cdef double *z = <double *> malloc(n * sizeof(double));

    for i in range(n):
        x[i] = xyz[i, 0]
        y[i] = xyz[i, 1]
        z[i] = xyz[i, 2]

    with nogil:
        h = pymcc_pass(x, y, z, n, resolution);

    cdef np.ndarray[double, ndim=1] np_h = np.empty(m_xyz);
    for i in range(m_xyz):
        np_h[i] = h[i]

    free(x);
    free(y);
    free(z);

    return np_h
