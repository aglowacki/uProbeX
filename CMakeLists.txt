# CMakeLists.txt -- Build system for the XRF-Maps
#
#Copyright (c) 2019, UChicago Argonne, LLC. All rights reserved.
#
#Copyright 2019. UChicago Argonne, LLC. This software was produced
#under U.S. Government contract DE-AC02-06CH11357 for Argonne National
#Laboratory (ANL), which is operated by UChicago Argonne, LLC for the
#U.S. Department of Energy. The U.S. Government has rights to use,
#reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR
#UChicago Argonne, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
#ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is
#modified to produce derivative works, such modified software should
#be clearly marked, so as not to confuse it with the version available
#from ANL.
#
#Additionally, redistribution and use in source and binary forms, with
#or without modification, are permitted provided that the following
#conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in
#      the documentation and/or other materials provided with the
#      distribution.
#
#    * Neither the name of UChicago Argonne, LLC, Argonne National
#      Laboratory, ANL, the U.S. Government, nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY UChicago Argonne, LLC AND CONTRIBUTORS
#"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UChicago
#Argonne, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#POSSIBILITY OF SUCH DAMAGE.


cmake_minimum_required(VERSION 3.5)

project(uProbeX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${PROJECT_SOURCE_DIR}/cmake "c:/Qt/5.11.1/msvc2017_64/lib/cmake/Qt5")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(XRF_Maps REQUIRED)
find_package(NetCDF REQUIRED)
find_package(Threads)
find_package(HDF5 REQUIRED COMPONENTS C)

IF(MSVC)
  # set(HDF5_LIBRARIES "" CACHE FILEPATH "HDF5 library")
ELSE()
  set(GCC_INCLUDE_DIR "/usr/include" CACHE PATH "Aditional GCC include folder")
  include_directories(GCC_INCLUDE_DIR)
ENDIF()

# Opitons for getting YouCompleteMe plugin working with this project
SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
ENDIF()


add_definitions(-D_BUILD_WITH_ZMQ)
IF( NOT ZeroMQ_INCLUDE_DIR AND NOT ZeroMQ_LIBRARY)
  find_package(ZeroMQ)
ENDIF()

#IF( NOT EIGEN3_INCLUDES )
#    MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDES to the include directory of your Eigen3 installation.")
#ENDIF()

set(PREC _REAL_FLOAT)


IF(NOT DEFINED CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH CACHE PATH "")
  MESSAGE( "IF qt5-config.cmake is not found, set CMAKE_PREFIX_PATH = Qt root directory")
ENDIF()
add_definitions(-D_BUILD_WITH_QT)
set(CMAKE_CXX_STANDARD 14)
find_package(Qt5 COMPONENTS Core Charts REQUIRED)
include_directories(${Qt5Charts_INCLUDES})
set(CMAKE_CXX_FLAGS "${Qt5Charts_EXECUTABLE_COMPILE_FLAGS}")

# Set a default build configuration if none is specified. 'Release' produces the smallest binaries
IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
ENDIF()

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

include(CheckCXXCompilerFlag)

IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Enable C++11 mode on C++ / Clang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -D__Unix__ -D${PREC} -fPIC -g -fopenmp")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__Unix__ -D${PREC} -fPIC")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

ENDIF()

# Compile with compiler warnings turned on
IF(MSVC)
  IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  ENDIF()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DHDF5CPP_USEDLL /D_USE_MATH_DEFINES /DH5_BUILT_AS_DYNAMIC_LIB /D_HDF5USEDLL_ /DBUILDING_DLL /DXDR_HACK /DXDR_LE /D${PREC} /DNOMINMAX /openmp")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  IF (${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Waddress -Warray-bounds=1 -Wbool-compare -Wbool-operation -Wc++11-compat -Wc++14-compat -Wcatch-value -Wchar-subscripts -Wcomment -Wduplicate-decl-specifier -Wenum-compare -Wformat -Wimplicit -Wimplicit-int -Wimplicit-function-declaration -Winit-self -Wlogical-not-parentheses -Wmain -Wmaybe-uninitialized -Wmemset-elt-size -Wmemset-transposed-args -Wmisleading-indentation -Wmissing-attributes -Wmissing-braces -Wmultistatement-macros -Wnarrowing -Wnonnull -Wnonnull-compare -Wopenmp-simd -Wparentheses -Wpointer-sign -Wreorder -Wrestrict -Wreturn-type -Wsequence-point -Wsign-compare -Wsizeof-pointer-div -Wsizeof-pointer-memaccess -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtautological-compare -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var")
  ENDIF()
ENDIF()

#zeromq include
include_directories(${ZeroMQ_INCLUDE_DIR})

#hdf5 include
include_directories(${HDF5_INCLUDE_DIRS})

# Include Eigen 
include_directories(${EIGEN3_INCLUDES})

# Include NetCDF 
include_directories(${NETCDF_INCLUDE_DIRS})

# Include XRF_Maps
include_directories(${XRF_MAPS_INCLUDE_DIR})

# Include header files
include_directories(src)

#--------------- start uprobex exec -----------------
add_executable(uProbeX
    src/gstar/CoordinateWidget.h
    src/gstar/CoordinateModel.h
    src/gstar/ImageViewToolBar.h
    src/gstar/RangeWidget.h
    src/gstar/CheckBoxDelegate.h
    src/gstar/ROI.h
    src/gstar/ROITableModel.h
    src/gstar/RectItem.h
    src/gstar/Array.h
    src/gstar/ImageViewScene.h
    src/gstar/ImageViewWidget.h
    src/gstar/HistogramPlot.h
    src/gstar/Splash.h
    src/gstar/AnnotationProperty.h
    src/gstar/Annotation/AbstractGraphicsItem.h
    src/gstar/Annotation/LineGraphicsItem.h
    src/gstar/Annotation/PieGraphicsItem.h
    src/gstar/Annotation/MarkerGraphicsItem.h
    src/gstar/Annotation/UProbeMarkerGraphicsItem.h
    src/gstar/Annotation/UProbeRegionGraphicsItem.h
    src/gstar/Annotation/RulerGraphicsItem.h
    src/gstar/Annotation/HotSpotMaskGraphicsItem.h
    src/gstar/Annotation/IntensityPieGraphicsItem.h
    src/gstar/Annotation/IntensityLineGraphicsItem.h
    src/gstar/Annotation/EmptyGraphicsItem.h
    src/gstar/AnnotationToolBarWidget.h
    src/gstar/AnnotationTreeModel.h
    src/gstar/RulerUnitsDialog.h
    src/gstar/ITransformer.h
    src/gstar/LinearTransformer.h
    src/gstar/CountsLookupTransformer.h
    src/gstar/AbstractImageWidget.h
    src/gstar/GStarResource.h
    src/mvc/DeselectableTreeView.h
    src/solver/asa047.hpp
    src/solver/AbstractSolver.h
    src/solver/Solver.h
    src/solver/CoordinateTransformer.h
    src/solver/PythonTransformer.h
    src/solver/NelderMeadSolver.h
    src/solver/PythonSolver.h
    src/mvc/PerPixelFitWidget.h
    src/mvc/SolverWidget.h
    src/mvc/ComboBoxDelegate.h
    src/mvc/AbstractWindowModel.h
    src/mvc/AbstractWindowController.h
    src/mvc/SWSModel.h
    src/mvc/TIFFModel.h
    src/mvc/TIFFWidget.h
    src/mvc/TIFFController.h
    src/mvc/SWSWidget.h
    src/mvc/SpectraWidget.h
    src/mvc/FitParamsTableModel.h
    src/mvc/FitElementsTableModel.h
    src/mvc/NetStreamWorker.h
    src/mvc/MapsH5Model.h
    src/mvc/MDA_Model.h
    src/mvc/MDA_Widget.h
    src/mvc/FitSpectraWidget.h
    src/mvc/MapsElementsWidget.h
    src/mvc/LiveMapsElementsWidget.h
    src/mvc/MapsWorkspaceModel.h
    src/mvc/MapsWorkspaceFilesWidget.h
    src/mvc/ImageStackControlWidget.h
    src/preferences/Preferences.h
    src/preferences/PreferencesDisplay.h
    src/preferences/PreferencesMicroPv.h
    src/preferences/PreferencesDialog.h
    src/preferences/Attribute.h
    src/preferences/AttributeGroup.h
    src/preferences/AttributeGroupModel.h
    src/preferences/PreferencesPythonFunc.h
    src/preferences/SolverTable.h
    src/preferences/SolverParameterWidget.h
    src/preferences/PreferencesSolverOption.h
    src/preferences/CoordinateTransformGlobals.h
    src/preferences/SolverParameterParse.h
    src/preferences/ProfileTable.h
    src/preferences/AttributeTableModel.h
    src/preferences/PythonSolverProfileWidget.h
    src/preferences/Profile.h
    src/preferences/PreferencesAutoSave.h
    src/preferences/PreferencesExport.h
    src/core/PythonLoader.h
    src/core/RegionCaller.h
    src/core/ShellRegionCaller.h
    src/core/PythonRegionCaller.h
    src/core/SubWindow.h
    src/core/uProbeX.h
    src/gstar/CoordinateWidget.cpp
    src/gstar/CoordinateModel.cpp
    src/gstar/ImageViewToolBar.cpp
    src/gstar/RangeWidget.cpp
    src/gstar/CheckBoxDelegate.cpp
    src/gstar/ROI.cpp
    src/gstar/ROITableModel.cpp
    src/gstar/RectItem.cpp
    src/gstar/Array.cpp
    src/gstar/ImageViewScene.cpp
    src/gstar/ImageViewWidget.cpp
    src/gstar/HistogramPlot.cpp
    src/gstar/Splash.cpp
    src/gstar/AnnotationProperty.cpp
    src/gstar/Annotation/AbstractGraphicsItem.cpp
    src/gstar/Annotation/LineGraphicsItem.cpp
    src/gstar/Annotation/PieGraphicsItem.cpp
    src/gstar/Annotation/MarkerGraphicsItem.cpp
    src/gstar/Annotation/UProbeMarkerGraphicsItem.cpp
    src/gstar/Annotation/UProbeRegionGraphicsItem.cpp
    src/gstar/Annotation/RulerGraphicsItem.cpp
    src/gstar/Annotation/HotSpotMaskGraphicsItem.cpp
    src/gstar/Annotation/IntensityPieGraphicsItem.cpp
    src/gstar/Annotation/IntensityLineGraphicsItem.cpp
    src/gstar/Annotation/EmptyGraphicsItem.cpp
    src/gstar/AnnotationToolBarWidget.cpp
    src/gstar/AnnotationTreeModel.cpp
    src/gstar/RulerUnitsDialog.cpp
    src/gstar/ITransformer.cpp
    src/gstar/LinearTransformer.cpp
    src/gstar/CountsLookupTransformer.cpp
    src/gstar/AbstractImageWidget.cpp
    src/mvc/DeselectableTreeView.cpp
    src/solver/asa047.cpp
    src/solver/AbstractSolver.cpp
    src/solver/Solver.cpp
    src/solver/CoordinateTransformer.cpp
    src/solver/PythonTransformer.cpp
    src/solver/NelderMeadSolver.cpp
    src/solver/PythonSolver.cpp
    src/mvc/PerPixelFitWidget.cpp
    src/mvc/SolverWidget.cpp
    src/mvc/ComboBoxDelegate.cpp
    src/mvc/AbstractWindowModel.cpp
    src/mvc/AbstractWindowController.cpp
    src/mvc/TIFFModel.cpp
    src/mvc/TIFFWidget.cpp
    src/mvc/TIFFController.cpp
    src/mvc/SWSWidget.cpp
    src/mvc/SWSModel.cpp
    src/mvc/SpectraWidget.cpp
    src/mvc/FitParamsTableModel.cpp
    src/mvc/FitElementsTableModel.cpp
    src/mvc/NetStreamWorker.cpp
    src/mvc/MapsH5Model.cpp
    src/mvc/MDA_Model.cpp
    src/mvc/MDA_Widget.cpp
    src/mvc/FitSpectraWidget.cpp
    src/mvc/MapsElementsWidget.cpp
    src/mvc/LiveMapsElementsWidget.cpp
    src/mvc/MapsWorkspaceModel.cpp
    src/mvc/MapsWorkspaceFilesWidget.cpp
    src/mvc/ImageStackControlWidget.cpp
    src/preferences/Preferences.cpp
    src/preferences/Attribute.cpp
    src/preferences/AttributeGroup.cpp
    src/preferences/AttributeGroupModel.cpp
    src/preferences/PreferencesPythonFunc.cpp
    src/preferences/PreferencesDisplay.cpp
    src/preferences/PreferencesMicroPv.cpp
    src/preferences/PreferencesDialog.cpp
    src/preferences/CoordinateTransformGlobals.cpp
    src/preferences/SolverTable.cpp
    src/preferences/SolverParameterWidget.cpp
    src/preferences/PreferencesSolverOption.cpp
    src/preferences/SolverParameterParse.cpp
    src/preferences/ProfileTable.cpp
    src/preferences/AttributeTableModel.cpp
    src/preferences/PythonSolverProfileWidget.cpp
    src/preferences/Profile.cpp
    src/preferences/PreferencesAutoSave.cpp
    src/preferences/PreferencesExport.cpp
    src/core/PythonLoader.cpp
    src/core/RegionCaller.cpp
    src/core/ShellRegionCaller.cpp
    src/core/PythonRegionCaller.cpp
    src/core/SubWindow.cpp
    src/core/uProbeX.cpp
    src/main.cpp
    uProbeX.qrc
)

set(CompilerFlags
  LIBRARY_OUTPUT_DIRECTORY LIBRARY_OUTPUT_DIRECTORY_RELEASE LIBRARY_OUTPUT_DIRECTORY_DEBUG
  RUNTIME_OUTPUT_DIRECTORY RUNTIME_OUTPUT_DIRECTORY_RELEASE RUNTIME_OUTPUT_DIRECTORY_DEBUG)

foreach(CompilerFlag ${CompilerFlags})
  set_target_properties(uProbeX PROPERTIES ${CompilerFlag} ${PROJECT_SOURCE_DIR}/bin)
endforeach()


#qt5_use_modules(uProbeX Core Widgets Gui)

IF (WIN32)
  IF (MSVC)
    # /bigobj is needed for bigger binding projects due to the limit to 64k addressable sections
    # /MP enables multithreaded builds (relevant when there are many files).
    set_target_properties(uProbeX PROPERTIES COMPILE_FLAGS "/D_WINSOCKAPI_")
    IF (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        set_target_properties(uProbeX PROPERTIES COMPILE_FLAGS "/Ox /arch:SSE2 /Qvec-report:2 /GL /MP /bigobj /Qansi-alias")
        set_target_properties(uProbeX PROPERTIES LINK_FLAGS "/LTCG")
    ENDIF()
  ENDIF()

  IF (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    string(REGEX REPLACE "mt-s" "mt" ZeroMQ_LIBRARY "${ZeroMQ_LIBRARY}")
  ELSE()
    string(REGEX REPLACE "sgd" "gd" ZeroMQ_LIBRARY "${ZeroMQ_LIBRARY}")
  ENDIF()
  target_link_libraries(uProbeX LINK_PUBLIC optimized ${ZeroMQ_LIBRARY_RELEASE} optimized ws2_32.lib )
  target_link_libraries(uProbeX LINK_PUBLIC debug ${ZeroMQ_LIBRARY_DEBUG} debug ws2_32.lib )
ELSEIF (UNIX)
  # It's quite common to have multiple copies of the same Python version
  # installed on one's system. E.g.: one copy from the OS and another copy
  # that's statically linked into an application like Blender or Maya.
  # If we link our plugin library against the OS Python here and import it
  # into Blender or Maya later on, this will cause segfaults when multiple
  # conflicting Python instances are active at the same time.

  # Windows is not affected by this issue since it handles DLL imports 
  # differently. The solution for Linux and Mac OS is simple: we just don't
  # link against the Python library. The resulting shared library will have
  # missing symbols, but that's perfectly fine -- they will be resolved at
  # import time.
  IF (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    set_target_properties(uProbeX PROPERTIES COMPILE_FLAGS "-O3")
  ENDIF()

  target_link_libraries(uProbeX LINK_PUBLIC optimized ${ZeroMQ_LIBRARY} )
  target_link_libraries(uProbeX LINK_PUBLIC debug ${ZeroMQ_LIBRARY} )

ENDIF()

target_link_libraries (uProbeX LINK_PUBLIC 
						optimized ${XRF_MAPS_LIBRARIES}
						optimized ${NETCDF_LIBRARIES}
						optimized ${HDF5_LIBRARIES}
						optimized ${Qt5Core_LIBRARIES}
						optimized ${Qt5Charts_LIBRARIES}
						${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries (uProbeX LINK_PUBLIC 
						debug ${XRF_MAPS_LIBRARIES}
						debug ${NETCDF_LIBRARIES}
						debug ${HDF5_LIBRARIES}
						debug ${Qt5Core_LIBRARIES}
						debug ${Qt5Charts_LIBRARIES}
						${CMAKE_THREAD_LIBS_INIT} )
