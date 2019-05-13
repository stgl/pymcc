import numpy as np
cimport pyicp
cimport numpy as np
from libc.stdlib cimport malloc, free

def pymcc_classification(np.ndarray[double, ndim = 2, mode = 'c'] xyz not None, scaleDomain2Spacing not None, curvatureTreshold not None):

  m_xyz, n_xyz = xyz.shape[0], zyx.shape[1]
  x = xyz[:,0]
  y = xyz[:,1]
  z = xyz[:,2]
  
  assert n_xyz == 3

  with nogil: 
    int * classification = pymcc_classify(&x[0], &y[0], &z[0], m_xyz, scaleDomain2Spacing, curvatureThreshold);

  cdef np.ndarray[np.int32_t, ndim=1] np_classification = np.empty(n_xyz, dtype=np.int32);
  for(int i=0; i < n; i++) {
    np_classification[i] = classification[i]
  }
  return np_classification
  
def pymcc_singlepass(np.ndarray[double, ndim = 2, mode = 'c'] xyz not None, scaleDomain2Spacing not None):

  m_xyz, n_xyz = xyz.shape[0], zyx.shape[1]
  x = xyz[:,0]
  y = xyz[:,1]
  z = xyz[:,2]
  
  assert n_xyz == 3
  
  with nogil: 
    double * h = pymcc_pass(&x[0], &y[0], &z[0], m_xyz, scaleDomain2Spacing);

  cdef np.ndarray[ndim=1] np_h = np.empty(n_xyz);
  for(int i=0; i < n; i++) {
    np_h[i] = h[i]
  }
  return np_h


