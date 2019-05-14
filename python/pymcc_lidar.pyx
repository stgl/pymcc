import numpy as np
cimport pymcc_lidar
cimport numpy as np
from libc.stdlib cimport malloc, free

def pymcc_classification(np.ndarray[double, ndim = 1, mode = 'c'] x not None, np.ndarray[double, ndim = 1, mode = 'c'] y not None, np.ndarray[double, ndim = 1, mode = 'c'] z not None, scaleDomain2Spacing not None, curvatureThreshold not None):

  m_x = x.size
  m_y = y.size
  m_z = z.size
  
  assert m_x == m_y
  assert m_y == m_z
  
  cdef int * classification;
  cdef double resolution = scaleDomain2Spacing;
  cdef double thresh = curvatureThreshold;
  
  cdef double *xd = &x[0];
  cdef double *yd = &y[0];
  cdef double *zd = &z[0];
  cdef int32_t n = m_x;
  
  with nogil: 
    classification = pymcc_classify(xd, yd, zd, n, resolution, thresh);

  cdef np.ndarray[np.int32_t, ndim=1] np_classification = np.empty(m_x, dtype=np.int32);
  for i in range(m_x):
    np_classification[i] = classification[i]
  
  return np_classification
  
def pymcc_singlepass(np.ndarray[double, ndim = 1, mode = 'c'] x not None, np.ndarray[double, ndim = 1, mode = 'c'] y not None, np.ndarray[double, ndim = 1, mode = 'c'] z not None, scaleDomainSpacing not None):

  m_x = x.size
  m_y = y.size
  m_z = z.size
  
  assert m_x == m_y
  assert m_y == m_z
  
  cdef double *h;
  cdef double resolution = scaleDomainSpacing;
  cdef int32_t n = m_x;
  
  with nogil: 
    h = pymcc_pass(&x[0], &y[0], &z[0], n, resolution);

  cdef np.ndarray[ndim=1, dtype=np.float] np_h = np.empty(m_x);
  for i in range(m_x):
    np_h[i] = h[i]
  return np_h


