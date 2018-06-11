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

EIGEN_INCLUDE = "${EIGEN3_DIR}"
HDF5_INCLUDE = "${HDF5_DIR}/include"
HDF5_LIBS = "${HDF5_DIR}/lib"
XRF_MAPS_INCLUDE = "${XRF_MAPS_DIR}/src"
XRF_MAPS_LIBS = "${XRF_MAPS_DIR}/bin"
NETCDF_INCLUDE = "${NETCDF_DIR}/include"
NETCDF_LIBS = "${NETCDF_DIR}/lib"
ZMQ_INCLUDE = "${ZMQ_DIR}/include"
ZMQ_LIBS = "${ZMQ_DIR}/lib"


win32 {
    EIGEN_INCLUDE = "$(EIGEN3_DIR)"
    XRF_MAPS_INCLUDE = "$(XRF_MAPS_DIR)\\src"
    XRF_MAPS_LIBS = "$(XRF_MAPS_DIR)\\build\\Release"
    HDF5_INCLUDE = "$(HDF5_DIR)\\include"
    HDF5_LIBS = "$(HDF5_DIR)\\lib"
    NETCDF_INCLUDE = "$(NETCDF_DIR)\\include"
    NETCDF_LIBS = "$(NETCDF_DIR)\\lib"
    ZMQ_INCLUDE = "$(ZMQ_DIR)\\include"
    ZMQ_LIBS = "$(ZMQ_DIR)\\build\\lib\\Release"
    WINDOWS_DEV_KIT_INCLUDE = "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.10586.0"
    WINDOWS_DEV_KIT_LIBS = "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.10586.0"
    DEFINES += NOMINMAX
    DEFINES += WIN32_LEAN_AND_MEAN
    #DEFINES += ZMQ_STATIC
    LIBS += ws2_32.lib rpcrt4.lib iphlpapi.lib
}

#######################################################

# Destination build directory
DESTDIR = ./bin

# Release or Debug
CONFIG += release
#CONFIG += debug
CONFIG += ordered
CONFIG += console 
CONFIG += c++14
DEFINES += WITH_DOM
DEFINES += _REAL_FLOAT
DEFINES += H5_BUILT_AS_DYNAMIC_LIB
QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

# Include paths
INCLUDEPATH += $$PWD
INCLUDEPATH += src
INCLUDEPATH += $${EIGEN_INCLUDE}
INCLUDEPATH += $${HDF5_INCLUDE}
INCLUDEPATH += $${NETCDF_INCLUDE}
INCLUDEPATH += $${ZMQ_INCLUDE}
INCLUDEPATH += $${XRF_MAPS_INCLUDE}

LIBS += -L$${HDF5_LIBS}
LIBS += -L$${NETCDF_LIBS}
LIBS += -L$${ZMQ_LIBS}
LIBS += -L$${XRF_MAPS_LIBS}

# Location for intermediate files
MOC_DIR += ./generated
OBJECTS_DIR += ./generated

# Resource file
RESOURCES = uProbeX.qrc

# Include actual project
include(uProbeX.pri)


LIBS += -lhdf5 -lnetcdf

# System dependent library paths
macx {
    LIBS += -lxrf_fit -lxrf_io
}
unix:!macx {
    LIBS += -lxrf_fit -lxrf_io
    LIBS += -L$$PWD/../../libs/hdf5/lib/ -lhdf5 -lzmq

    INCLUDEPATH += $$PWD/../../libs/hdf5/include
    DEPENDPATH += $$PWD/../../libs/hdf5/include

    INCLUDEPATH += $$PWD/../XRF-Maps/src
    DEPENDPATH += $$PWD/../XRF-Maps/src
}
win32 {
    #QMAKE_CXXFLAGS += /D__STDC__=0 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE
    #QMAKE_LFLAGS += /STACK:41943040,41943040

    #flag for qlibrary
    #QMAKE_LFLAGS -= /NXCOMPAT
    #QMAKE_LFLAGS += /NXCOMPAT:NO
    QMAKE_CXXFLAGS += /Gs999999

    INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\shared
    INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\um
    INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\winrt
    INCLUDEPATH += $${WINDOWS_DEV_KIT_INCLUDE}\\ucrt

    LIBS += -L$${WINDOWS_DEV_KIT_LIBS}\\ucrt\\x64

    #debug
    #Debug:QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=2
    #release
    #Release:QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=0

    LIBS += -llibxrf_io -llibxrf_fit -llibzmq-v140-mt-4_2_3
}
