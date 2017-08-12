# Copyright (c) 2014, UChicago Argonne, LLC
# See LICENSE file.

# Qt template
TEMPLATE = app

# Project name
TARGET = uProbeX

# Qt components
QT += core \
    gui \
    xml \
    widgets \
    charts

QMAKE_EXT_CPP += .c

###################################################	
#                             						  #
# Modify paths for 3rd party libraries below.     #
#                             						  #
###################################################

GSTAR_INCLUDE = "${GSTAR_BASE}/include"
GSTAR_LIBS = "${GSTAR_BASE}/lib"
EIGEN_INCLUDE = "${EIGEN3_BASE}"
XRF_MAPS_INCLUDE = "${XRF_MAPS_BASE}/src/"
XRF_MAPS_LIBS = "${XRF_MAPS_BASE}/bin"
HDF5_INCLUDE = "${HDF5_BASE}/include"
HDF5_LIBS = "${HDF5_BASE}/lib"
NETCDF_INCLUDE = "${NETCDF_BASE}/include"
NETCDF_LIBS = "${NETCDF_BASE}/lib"

win32 {
GSTAR_INCLUDE = "$(GSTAR_BASE)\\include"
GSTAR_LIBS = "$(GSTAR_BASE)\\lib"
EIGEN_INCLUDE = "$(EIGEN3_BASE)"
XRF_MAPS_INCLUDE = "$(XRF_MAPS_BASE)\\src"
XRF_MAPS_LIBS = "$(XRF_MAPS_BASE)\\build\\Debug"
HDF5_INCLUDE = $(HDF5_BASE)\\include
HDF5_LIBS = $(HDF5_BASE)\\lib
NETCDF_INCLUDE = $(NETCDF_BASE)\\include
NETCDF_LIBS = $(NETCDF_BASE)\\lib
WINDOWS_DEV_KIT_INCLUDE = "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.10586.0"
WINDOWS_DEV_KIT_LIBS = "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.10586.0"
}

#######################################################

# Destination build directory
DESTDIR = ./bin

# Release or Debug
#CONFIG += release
CONFIG += debug
CONFIG += ordered
CONFIG += console 
CONFIG += c++14
DEFINES += WITH_DOM
DEFINES += _REAL_FLOAT
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

# Include paths
INCLUDEPATH += src
INCLUDEPATH += $${EIGEN_INCLUDE}
INCLUDEPATH += $${XRF_MAPS_INCLUDE}
INCLUDEPATH += $${GSTAR_INCLUDE}
INCLUDEPATH += $${HDF5_INCLUDE}
INCLUDEPATH += $${NETCDF_INCLUDE}

# System dependent library paths
macx {
LIBS += -L$${XRF_MAPS_LIBS} -lxrf_io -lxrf_fit
}
unix:!macx {
LIBS += -L$${XRF_MAPS_LIBS} -lxrf_io -lxrf_fit
}
win32 {
#QMAKE_CXXFLAGS += /D__STDC__=0 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE
#QMAKE_LFLAGS += /STACK:41943040,41943040

#flag for qlibrary
#QMAKE_LFLAGS -= /NXCOMPAT
#QMAKE_LFLAGS += /NXCOMPAT:NO

INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\shared
INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\um
INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\winrt
INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\ucrt

LIBS += -L$${WINDOWS_DEV_KIT_LIBS}\\ucrt\\x64

#debug
QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=2
#release
#QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=0

LIBS += -L$${XRF_MAPS_LIBS} -llibxrf_io -llibxrf_fit
}


LIBS += -L$${GSTAR_LIBS} -lGStar
LIBS += -L$${HDF5_LIBS}  -lhdf5
LIBS += -L$${NETCDF_LIBS}  -lnetcdf


# Location for intermediate files
MOC_DIR += ./generated
OBJECTS_DIR += ./generated

# Resource file
RESOURCES = uProbeX.qrc

# Include actual project
include(uProbeX.pri)
