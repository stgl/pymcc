# - Try to find libLAS
# These variables control the search:
#
#  LIBLAS_ROOT - if defined, then
#                  LIBLAS_ROOT/include is searched for libLAS headers, and
#                  LIBLAS_ROOT/lib is searched for libLAS libraries
#
#  $ENV{LIBLAS_ROOT} - if LIBLAS_ROOT not defined and this environment
#                      variable is, then
#                  $ENV{LIBLAS_ROOT}/include is searched for libLAS headers, and
#                  $ENV{LIBLAS_ROOT}/lib is searched for libLAS libraries
#
# Once done this will define
#
#  LIBLAS_FOUND - system has libLAS
#  LIBLAS_INCLUDE_DIRS - the libLAS include directory
#  LIBLAS_LIBRARIES - Link these to use libLAS
#  LIBLAS_DEFINITIONS - Compiler switches required for using libLAS
#
#  Copyright 2009 Green Code LLC
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.


if (LIBLAS_LIBRARIES AND LIBLAS_INCLUDE_DIRS)
  # in cache already
  set (LIBLAS_FOUND TRUE)
else ()
  #  Search paths
  if (DEFINED LIBLAS_ROOT)
    set (INCLUDE_PATHS_TO_SEARCH  ${LIBLAS_ROOT}/include)
    set (LIBRARY_PATHS_TO_SEARCH  ${LIBLAS_ROOT}/lib)
  elseif (DEFINED ENV{LIBLAS_ROOT})
    set (INCLUDE_PATHS_TO_SEARCH  $ENV{LIBLAS_ROOT}/include)
    set (LIBRARY_PATHS_TO_SEARCH  $ENV{LIBLAS_ROOT}/lib)
  elseif (WIN32)
    set (INCLUDE_PATHS_TO_SEARCH  C:/OSGeo4W/include)
    set (LIBRARY_PATHS_TO_SEARCH  C:/OSGeo4W/lib)
  else ()
    set (INCLUDE_PATHS_TO_SEARCH /usr/include
                                 /usr/local/include
                                 /opt/local/include
                                 /sw/include )
    set (LIBRARY_PATHS_TO_SEARCH /usr/lib
                                 /usr/local/lib
                                 /opt/local/lib
                                 /sw/lib )
  endif ()

  # Library name to search for
  if (WIN32)
    if (DEFINED CMAKE_CXX_COMPILER)
      # C++ API is only in static library
      set (LIBRARY_NAME_TO_LOOK_FOR liblas)
    else ()
      set (LIBRARY_NAME_TO_LOOK_FOR liblas_i)
    endif ()
  else ()
    set (LIBRARY_NAME_TO_LOOK_FOR las)
  endif ()

  find_path (LIBLAS_INCLUDE_DIR NAMES         liblas/liblas.hpp
                                PATHS         ${INCLUDE_PATHS_TO_SEARCH}
                                PATH_SUFFIXES liblas)

  find_library (LIBLAS_LIBRARY NAMES ${LIBRARY_NAME_TO_LOOK_FOR}
                               PATHS ${LIBRARY_PATHS_TO_SEARCH})

  set (LIBLAS_INCLUDE_DIRS ${LIBLAS_INCLUDE_DIR})
  set (LIBLAS_LIBRARIES    ${LIBLAS_LIBRARY})

  if (LIBLAS_INCLUDE_DIRS AND LIBLAS_LIBRARIES)
     set (LIBLAS_FOUND TRUE)
  endif ()

  if (LIBLAS_FOUND)
    if (NOT LibLAS_FIND_QUIETLY)
      message (STATUS "Found libLAS: ${LIBLAS_LIBRARIES}")
    endif ()
  else ()
    if (LibLAS_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find libLAS")
    endif ()
  endif ()

  #  Show the LIBLAS_INCLUDE_DIRS and LIBLAS_LIBRARIES variables only
  #  in the advanced view
  mark_as_advanced(LIBLAS_INCLUDE_DIRS LIBLAS_LIBRARIES)

endif ()
