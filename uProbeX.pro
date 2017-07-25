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
    widgets

QMAKE_EXT_CPP += .c

###################################################	
#                             						  #
# Modify paths for 3rd party libraries below.     #
#                             						  #
###################################################

GSTAR_INCLUDE = "${GSTAR_BASE}/include"
GSTAR_LIBS = "${GSTAR_BASE}/lib"
HDF5_INCLUDE="${HDF5_BASE}/include"
HDF5_LIBS="${HDF5_BASE}/lib"


win32 {
GSTAR_INCLUDE = "$$(GSTAR_BASE)\\include"
GSTAR_LIBS = "$$(GSTAR_BASE)\\lib"
HDF5_INCLUDE="$$(HDF5_BASE)\\include"
HDF5_LIBS="$$(HDF5_BASE)\\lib"
}

#######################################################

# Destination build directory
DESTDIR = ./bin

# Release or Debug
#CONFIG += release
CONFIG += debug
CONFIG += ordered
CONFIG += console 
DEFINES += WITH_DOM
QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

# Include paths
INCLUDEPATH += include
INCLUDEPATH += $${GSTAR_INCLUDE}
INCLUDEPATH += $${HDF5_INCLUDE}

# System dependent library paths
macx {
}
unix:!macx {
}
win32 {
INCLUDEPATH += $${EPICS_BASE}\\include\\compiler\\msvc

QMAKE_CXXFLAGS += /D__STDC__=0 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /DEPICS_BUILD_DLL /DEPICS_CALL_DLL
QMAKE_LFLAGS += /STACK:41943040,41943040

#flag for qlibrary
QMAKE_LFLAGS -= /NXCOMPAT
QMAKE_LFLAGS += /NXCOMPAT:NO

#debug
#QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=2
#release
QMAKE_CXXFLAGS += /D_ITERATOR_DEBUG_LEVEL=0
}


LIBS += -L$${GSTAR_LIBS} -lGStar
LIBS += -L$${HDF5_LIBS}  -lhdf5


# Location for intermediate files
MOC_DIR += ./generated
OBJECTS_DIR += ./generated

# Resource file
RESOURCES = uProbeX.qrc

# Include actual project
include(uProbeX.pri)
