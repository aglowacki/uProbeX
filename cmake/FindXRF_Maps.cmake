#set (XRF_MAPS_ROOT_DIR "" CACHE PATH "XRF-Maps root dir")

find_path(XRF_MAPS_ROOT_DIR
  NAMES "build/XRF_MAPS.sln" "${PROJECT_SOURCE_DIR}/../XRF-Maps/build/Makefile")

#find_path(XRF_MAPS_INCLUDE_DIR
#  NAMES defines.h main.cpp
#  HINTS $${XRF_MAPS_ROOT_DIR}/src/core/
#)

find_library (LIB_XRF_IO_LIBRARY_DEBUG NAMES libxrf_io
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Debug/" "${XRF_MAPS_ROOT_DIR}/bin")

find_library (LIB_XRF_IO_LIBRARY NAMES libxrf_io
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Release/" "${XRF_MAPS_ROOT_DIR}/bin")
  
find_library (LIB_XRF_FIT_LIBRARY_DEBUG NAMES libxrf_fit
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Debug/" "${XRF_MAPS_ROOT_DIR}/bin")

find_library (LIB_XRF_FIT_LIBRARY NAMES libxrf_fit
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Release/" "${XRF_MAPS_ROOT_DIR}/bin")

set (XRF_MAPS_LIBRARIES_DEBUG ${LIB_XRF_IO_LIBRARY_DEBUG} ${LIB_XRF_FIT_LIBRARY_DEBUG} CACHE STRING "XRF maps debug libraries")
set (XRF_MAPS_LIBRARIES ${LIB_XRF_IO_LIBRARY} ${LIB_XRF_FIT_LIBRARY} CACHE STRING "XRF Maps release libraries")
set (XRF_MAPS_INCLUDE_DIR ${XRF_MAPS_ROOT_DIR}/src CACHE PATH "XRF Maps Include Directory")
set (EIGEN3_INCLUDES ${XRF_MAPS_ROOT_DIR}/src/support/eigen-git-mirror CACHE PATH "Eigen3 Include Directory")


mark_as_advanced (XRF_MAPS_LIBRARIES
                  XRF_MAPS_LIBRARIES_DEBUG
				  EIGEN3_INCLUDES
			      LIB_XRF_IO_LIBRARY
                  LIB_XRF_FIT_LIBRARY
				  LIB_XRF_IO_LIBRARY_DEBUG
                  LIB_XRF_FIT_LIBRARY_DEBUG)

