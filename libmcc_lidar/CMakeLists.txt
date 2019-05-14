cmake_minimum_required(VERSION 2.6)

project (MCC_LIDAR CXX)

#  Get the Subversion revision number for the project's source code.  The
#  VersionControl.cmake file tests the variable WINDOWS for the Windows
#  platform, so it needs to be set before including the file.
if (WIN32)
  set (WINDOWS ${WIN32})
endif()

set (CONFIG_HEADER "./config.h")
configure_file ("./config.h.in"
                "${CONFIG_HEADER}")
include_directories (".")
include_directories ("./include")

if (WIN32)
  #  The FindBoost module looks for dynamic libraries by default.  But Boost's
  #  auto-linking on Windows uses static libraries by default.  These two
  #  defaults will cause linker errors about multiple definitions of symbols.
  #  So tell FindBoost to look for static libraries.
  set (Boost_USE_STATIC_LIBS ON)
endif()
find_package (Boost 1.51 COMPONENTS filesystem
                                    program_options
                                    system)
if (NOT Boost_FOUND AND WIN32 AND NOT DEFINED ENV{BOOST_ROOT})
  #  The FindBoost module doesn't work on Windows with BoostPro's binary
  #  distributions when the subminor version is 0 (e.g., 1.39.0):
  #
  #    http://www.cmake.org/pipermail/cmake/2009-June/030376.html
  #
  #  The BOOST_ROOT environment needs to be set.
  message("Error: Boost was not found."
          "  Set the BOOST_ROOT environment variable to the folder where Boost is installed.")
endif()

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})
find_package (LibLAS)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${LIBLAS_INCLUDE_DIRS} )

# FUSION's code for LDA format not integrated yet
#add_subdirectory (fusion)

file (GLOB SRC_FILES "*.cpp" "src/*.cpp")
file (GLOB HDR_FILES "*.h" "include/*.h")
list (INSERT HDR_FILES 0 "${CONFIG_HEADER}")

# Have TPSDemo spline code use double-precision for vector coordinates
add_definitions(-DVEC_WITH_DOUBLE_COORDINATES)

set (TPSDEMO_FILENAMES linalg3d-double.h
                       ludecomposition.h
                       spline.cpp
                       spline.h )
foreach (name ${TPSDEMO_FILENAMES})
  list (APPEND TPSDEMO_SRCS tpsdemo/${name})
endforeach()
source_group("TPS Demo" FILES ${TPSDEMO_SRCS})

if (MSVC)
  #  Disable warning C4996 about deprecated functions
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)

  #  Enable Win32 structured exceptions along with C++ exceptions
  string(REGEX REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# determine if OpenMP can/should be used
option(USE_OPENMP "Enable OpenMP?" ON) # set to OFF to disable
if(USE_OPENMP)
 FIND_PACKAGE(OpenMP)
 if(OPENMP_FOUND OR OpenMP_FOUND)
  message(STATUS "OpenMP flags = ${OpenMP_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 else(OPENMP_FOUND OR OpenMP_FOUND)
  message(STATUS "OpenMP not supported")
 endif(OPENMP_FOUND OR OpenMP_FOUND)
endif(USE_OPENMP)

# make release version
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

set (PROGRAM_NAME mcc_lidar)
add_library (${PROGRAM_NAME} STATIC ${SRC_FILES} ${HDR_FILES} ${TPSDEMO_SRCS})
target_link_libraries (${PROGRAM_NAME}
                       ${Boost_FILESYSTEM_LIBRARY}
                       ${Boost_PROGRAM_OPTIONS_LIBRARY}
                       ${Boost_SYSTEM_LIBRARY}
                       ${LIBLAS_LIBRARY} )

if (MSVC)
	set (LIBLAS_AUX_DLL_NAMES comerr32
							gdal18
							geotiff
							geos_c
							gssapi32
							hdf_fw
							hdf5dll
							jpeg_osgeo
							jpeg12_osgeo
							ogdi_32b1
							krb5_32
							k5sprt32
							laszip
							libcurl
							libexpat
							liblas
							libeay32
							libintl-8
							libiconv-2
							libmysql
							libtiff
							libpq
							mfhdf_fw
							netcdf
							proj
							ssleay32
							szlibdll
							sqlite3
							xerces-c_3_1
							zlib1
							zlib_osgeo)
  string (REGEX REPLACE "\\\\" "/" LIBLAS_ROOT_DIR "$ENV{LIBLAS_ROOT}")
  foreach (name ${LIBLAS_AUX_DLL_NAMES})
    #list (APPEND LIBLAS_AUX_DLLS "${LIBLAS_ROOT_DIR}/${name}.dll")
	list (APPEND LIBLAS_AUX_DLLS "C:/OSGeo4W/bin/${name}.dll")
  endforeach()
  install (FILES ${LIBLAS_AUX_DLLS} DESTINATION bin)
  install (PROGRAMS package/win/set_path.bat DESTINATION bin)
 
  include (InstallRequiredSystemLibraries)

  set (DOC_FILE "Documentation.txt")
  set (PROGRAM_DOC "${PROJECT_BINARY_DIR}/${DOC_FILE}")
  configure_file("${PROJECT_SOURCE_DIR}/package/win/${DOC_FILE}" "${PROGRAM_DOC}")
  install (FILES "${PROGRAM_DOC}" DESTINATION doc)
endif()

get_filename_component(MCC_LIDAR_ROOT "${PROJECT_SOURCE_DIR}" PATH)
set (LICENSE_FILE "${MCC_LIDAR_ROOT}/LICENSE.txt")
set (NOTICE_FILE  "${MCC_LIDAR_ROOT}/NOTICE.txt")
if (MSVC)
  #  Make sure the license & notice have Windows line endings for Notepad.
  set (LICENSE_FILE_WINDOWS "${PROJECT_BINARY_DIR}/LICENSE.txt")
  configure_file("${LICENSE_FILE}" "${LICENSE_FILE_WINDOWS}")
  install (FILES "${LICENSE_FILE_WINDOWS}" DESTINATION .)

  set (NOTICE_FILE_WINDOWS "${PROJECT_BINARY_DIR}/NOTICE.txt")
  configure_file("${NOTICE_FILE}" "${NOTICE_FILE_WINDOWS}")
  install (FILES "${NOTICE_FILE_WINDOWS}" DESTINATION .)
endif()

set (APP_NAME MCC-LIDAR)
set (CPACK_PACKAGE_VERSION              ${VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR        ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MAJOR        ${VERSION_MINOR})
set (CPACK_PACKAGE_FILE_NAME            "${APP_NAME}-${VERSION_RELEASE_ABBR}-setup")
set (CPACK_PACKAGE_INSTALL_DIRECTORY    "${APP_NAME} ${VERSION_RELEASE_ABBR}")
set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${APP_NAME} ${VERSION_RELEASE_ABBR}")
set (CPACK_RESOURCE_FILE_LICENSE        "${LICENSE_FILE}")

set (CPACK_OPTIONS_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")
configure_file ("${PROJECT_SOURCE_DIR}/package/CPackOptions.cmake.in"
                "${CPACK_OPTIONS_FILE}"
                @ONLY)
set (CPACK_PROJECT_CONFIG_FILE "${CPACK_OPTIONS_FILE}")

include(CPack)
