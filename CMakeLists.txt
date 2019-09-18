cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)

include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(./libmcc_lidar/)
add_subdirectory(./python/)

