##=============================================================================
##
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##
##=============================================================================
# - Try to find ZeroMQ headers and libraries
#
# Usage of this module as follows:
#
#     find_package(ZeroMQ)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  ZeroMQ_ROOT_DIR  Set this variable to the root installation of
#                            ZeroMQ if the module has problems finding
#                            the proper installation path.
#
# Variables defined by this module:
#
#  ZeroMQ_FOUND              System has ZeroMQ libs/headers
#  ZeroMQ_LIBRARIES          The ZeroMQ libraries
#  ZeroMQ_INCLUDE_DIR        The location of ZeroMQ headers
#  ZeroMQ_VERSION            The version of ZeroMQ

find_path(ZeroMQ_ROOT_DIR
  NAMES include/zmq.h
  )

if(MSVC)

  set(_zmq_TOOLSET)
  list(APPEND _zmq_TOOLSET "-v150")
  list(APPEND _zmq_TOOLSET "-v141")
  list(APPEND _zmq_TOOLSET "-v140")
 

  set(_zmq_versions
     "4_2_3"
     "4_1_5" "4_1_4" "4_1_3" "4_1_2" "4_1_1" "4_1_0")

  set(_zmq_release_names)
  set(_zmq_debug_names)
  foreach( toolset ${_zmq_TOOLSET})
    foreach( ver ${_zmq_versions})
	  find_library(ZeroMQ_LIBRARY_RELEASE
        NAMES "libzmq${toolset}-mt-${ver}"
        PATHS ${ZeroMQ_ROOT_DIR}/bin/Release
          ${ZeroMQ_ROOT_DIR}/lib/Release
		  ${ZeroMQ_ROOT_DIR}/build/lib/Release
      )
	  IF(ZeroMQ_LIBRARY_RELEASE)
		break()
	  ENDIF()
    endforeach()
  endforeach()
  foreach( toolset ${_zmq_TOOLSET})
    foreach( ver ${_zmq_versions})
	  find_library(ZeroMQ_LIBRARY_DEBUG
        NAMES "libzmq${toolset}-mt-gd-${ver}"
        PATHS ${ZeroMQ_ROOT_DIR}/bin/Debug
          ${ZeroMQ_ROOT_DIR}/lib/Debug
		  ${ZeroMQ_ROOT_DIR}/build/lib/Debug
      )
	  IF(ZeroMQ_LIBRARY_DEBUG)
		break()
	  ENDIF()
	endforeach()
  endforeach()

  if(ZeroMQ_LIBRARY_RELEASE AND ZeroMQ_LIBRARY_DEBUG)
    set(ZeroMQ_LIBRARY
        debug ${ZeroMQ_LIBRARY_DEBUG}
        optimized ${ZeroMQ_LIBRARY_RELEASE}
        )
  elseif(ZeroMQ_LIBRARY_RELEASE)
    set(ZeroMQ_LIBRARY ${ZeroMQ_LIBRARY_RELEASE})
  elseif(ZeroMQ_LIBRARY_DEBUG)
    set(ZeroMQ_LIBRARY ${ZeroMQ_LIBRARY_DEBUG})
  endif()

else()
  find_library(ZeroMQ_LIBRARY
    NAMES zmq libzmq
    HINTS ${ZeroMQ_ROOT_DIR}/lib
    )
endif()

find_path(ZeroMQ_INCLUDE_DIR
  NAMES zmq.h
  HINTS ${ZeroMQ_ROOT_DIR}/include
  )

function(extract_version_value value_name file_name value)
  file(STRINGS ${file_name} val REGEX "${value_name} .")
  string(FIND ${val} " " last REVERSE)
  string(SUBSTRING ${val} ${last} -1 val)
  string(STRIP ${val} val)
  set(${value} ${val} PARENT_SCOPE)
endfunction(extract_version_value)
if(ZeroMQ_INCLUDE_DIR)
  extract_version_value("ZMQ_VERSION_MAJOR" ${ZeroMQ_INCLUDE_DIR}/zmq.h MAJOR)
  extract_version_value("ZMQ_VERSION_MINOR" ${ZeroMQ_INCLUDE_DIR}/zmq.h MINOR)
  extract_version_value("ZMQ_VERSION_PATCH" ${ZeroMQ_INCLUDE_DIR}/zmq.h PATCH)
endif()
set(ZeroMQ_VER "${MAJOR}.${MINOR}.${PATCH}")

#We are using the 2.8.10 signature of find_package_handle_standard_args,
#as that is the version that ParaView 5.1 && VTK 6/7 ship, and inject
#into the CMake module path. This allows our FindModule to work with
#projects that include VTK/ParaView before searching for Remus
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  ZeroMQ
  REQUIRED_VARS ZeroMQ_LIBRARY ZeroMQ_INCLUDE_DIR
  VERSION_VAR ZeroMQ_VER
  )

set(ZeroMQ_FOUND ${ZEROMQ_FOUND})
set(ZeroMQ_INCLUDE_DIRS ${ZeroMQ_INCLUDE_DIR})
set(ZeroMQ_LIBRARIES ${ZeroMQ_LIBRARY})
set(ZeroMQ_VERSION ${ZeroMQ_VER})

mark_as_advanced(
#  ZeroMQ_ROOT_DIR
  ZeroMQ_LIBRARY
  ZeroMQ_LIBRARY_DEBUG
  ZeroMQ_LIBRARY_RELEASE
  ZeroMQ_INCLUDE_DIR
  ZeroMQ_VERSION
  )
