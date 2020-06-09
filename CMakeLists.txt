cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(./libmcc_lidar/)
add_subdirectory(./python/)
