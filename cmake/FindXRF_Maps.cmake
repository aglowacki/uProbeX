
find_path(XRF_MAPS_ROOT_DIR
  NAMES build/XRF_MAPS.sln
  )

#find_path(XRF_MAPS_INCLUDE_DIR
#  NAMES defines.h main.cpp
#  HINTS $${XRF_MAPS_ROOT_DIR}/src/core/
#) 

find_library (LIB_XRF_IO_LIBRARY NAMES libxrf_io
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Release/" "${XRF_MAPS_ROOT_DIR}/build/Debug/")

  
find_library (LIB_XRF_FIT_LIBRARY NAMES libxrf_fit
  PATHS "${XRF_MAPS_ROOT_DIR}/build/Release/" "${XRF_MAPS_ROOT_DIR}/build/Debug/")

mark_as_advanced (XRF_MAPS_LIBRARIES
				  EIGEN3_INCLUDES
			      LIB_XRF_IO_LIBRARY
                  LIB_XRF_FIT_LIBRARY)

set (XRF_MAPS_LIBRARIES ${LIB_XRF_IO_LIBRARY} ${LIB_XRF_FIT_LIBRARY})
set (XRF_MAPS_INCLUDE_DIR ${XRF_MAPS_ROOT_DIR}/src CACHE PATH "XRF Maps Include Directory")
set (EIGEN3_INCLUDES ${XRF_MAPS_ROOT_DIR}/src/support/eigen-git-mirror CACHE PATH "Eigen3 Include Directory")
