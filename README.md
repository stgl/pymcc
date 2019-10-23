# pymcc

Python bindings for multiscale curvature classification (MCC) of point cloud data

See [**pymccrgb**](https://pymccrgb.readthedocs.io/en/latest/) for a Python interface to MCC-RGB, an extension of MCC to points with color attributes.

## Installation

This package requires CMake, Boost, at least the C++11 standard library, and the LibLAS C API.

```bash
git clone https://github.com/stgl/pymcc
cd pymcc
# Activate virtual environment as needed 
pip install .
```

## Reference

Implements method described in

Evans, J. S., & Hudak, A. T. 2007. A multiscale curvature algorithm for classifying discrete return LiDAR in forested environments. IEEE Transactions on Geoscience and Remote Sensing, 45(4), 1029-1038 [doi](https://doi.org/10.1109/TGRS.2006.890412)

## MCC-lidar

This package provides Cython/Python wrappers to functions in version 2.1 of `mcc-lidar`:

- [mcc-lidar project](https://sourceforge.net/p/mcclidar/wiki/Home/)
- [mcc-lidar source](https://sourceforge.net/p/mcclidar/code/HEAD/tree/tags/2.1/)
