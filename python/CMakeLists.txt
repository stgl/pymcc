cmake_minimum_required(VERSION 2.6)
project(pyicp)

find_package(PythonInterp)
find_package(PythonLibs)

find_path(BOOST_DIR "boost")

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )

include(UseCython)
include(FindNumPy)

link_directories(../libmcc-lidar/lib/)
include_directories(../libmcc-lidar/include/)
include_directories("${BOOST_DIR}")
include_directories("${PYTHON_NUMPY_INCLUDE_DIR}")
include_directories("./")

set_source_files_properties(pymcc-lidar.pyx PROPERTIES CYTHON_IS_CXX 1)
cython_add_module(pymcc-lidar pymcc-lidar.pyx pymccp-ldar.cpp)
target_link_libraries(pymcc-lidar mcc-lidar)

