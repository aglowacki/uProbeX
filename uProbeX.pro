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

BOOST_INCLUDE = "${BOOST_BASE}/include"
BOOST_LIBS = "${BOOST_BASE}/lib"
EPICS_BASE = "${EPICS_FOLDER}"
EPICS_HOST_ARCH = "${EPICS_HOST_ARCH}"
EPICS_TARGET_OS = "${EPICS_TARGET_OS}"
GSTAR_INCLUDE = "${GSTAR_BASE}/include"
GSTAR_LIBS = "${GSTAR_BASE}/lib"
SSTAR_INCLUDE = "${SSTAR_BASE}/include"
SSTAR_LIBS = "${SSTAR_BASE}/bin/shared"
DSTAR_INCLUDE="${DSTAR_BASE}/include"
DSTAR_LIBS="${DSTAR_BASE}/lib/shared"
HDF5_INCLUDE="${HDF5_BASE}/include"
HDF5_LIBS="${HDF5_BASE}/lib"
TIFF_INCLUDE="${TIFF_BASE}/include"
TIFF_LIBS="${TIFF_BASE}/lib"

win32 {
BOOST_INCLUDE = "$$(BOOST_BASE)"
BOOST_LIBS = "$$(BOOST_BASE)\\lib"
EPICS_BASE = "$$(EPICS_FOLDER)"
EPICS_HOST_ARCH = "$$(EPICS_HOST_ARCH)"
EPICS_TARGET_OS = "$$(EPICS_TARGET_OS)"
GSTAR_INCLUDE = "$$(GSTAR_BASE)\\include"
GSTAR_LIBS = "$$(GSTAR_BASE)\\lib"
SSTAR_INCLUDE = "$$(SSTAR_BASE)\\include"
SSTAR_LIBS = "$$(SSTAR_BASE)\\bin\\static"
DSTAR_INCLUDE="$$(DSTAR_BASE)\\include"
DSTAR_LIBS="$$(DSTAR_BASE)\\lib\\static"
HDF5_INCLUDE="$$(HDF5_BASE)\\include"
HDF5_LIBS="$$(HDF5_BASE)\\lib"
TIFF_INCLUDE="$$(TIFF_BASE)\\include"
TIFF_LIBS="$$(TIFF_BASE)\\lib"
BOOST_VER="$$(BOOST_VER)"
}

#######################################################

# Destination build directory
DESTDIR = ./bin

# Release or Debug
CONFIG += release
#CONFIG += debug
CONFIG += ordered
CONFIG += console 
DEFINES += WITH_DOM
QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

# Include paths
INCLUDEPATH += include
INCLUDEPATH += $${BOOST_INCLUDE}
INCLUDEPATH += $${EPICS_BASE}/include
INCLUDEPATH += $${EPICS_BASE}/include/os/$${EPICS_TARGET_OS}
INCLUDEPATH += $${GSTAR_INCLUDE}
INCLUDEPATH += $${SSTAR_INCLUDE}
INCLUDEPATH += $${DSTAR_INCLUDE}
INCLUDEPATH += $${HDF5_INCLUDE}
INCLUDEPATH += $${TIFF_INCLUDE}

# System dependent library paths
macx {
LIBS += -L$${BOOST_LIBS} -lboost_thread -lboost_system
LIBS += -L$${TIFF_LIBS}  -ltiff
}
unix:!macx {
LIBS += -L$${BOOST_LIBS} -lboost_thread -lboost_system
LIBS += -L$${TIFF_LIBS}  -ltiff
}
win32 {
INCLUDEPATH += $${EPICS_BASE}\\include\\compiler\\msvc

#debug
#LIBS += -L$${BOOST_LIBS} -llibboost_thread-vc100-mt-gd-$${BOOST_VER} -llibboost_system-vc100-mt-gd-$${BOOST_VER} -llibboost_chrono-vc100-mt-gd-$${BOOST_VER}

#release
LIBS += -L$${BOOST_LIBS} -llibboost_thread-vc100-mt-$${BOOST_VER} -llibboost_system-vc100-mt-$${BOOST_VER} -llibboost_chrono-vc100-mt-$${BOOST_VER}
LIBS += -L$${TIFF_LIBS}  -llibtiff

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


LIBS += -L$${EPICS_BASE}/lib/$${EPICS_HOST_ARCH} -lca
LIBS += -L$${DSTAR_LIBS} -ldstar
LIBS += -L$${GSTAR_LIBS} -lGStar
LIBS += -L$${SSTAR_LIBS} -lsstar
LIBS += -L$${HDF5_LIBS}  -lhdf5


# Location for intermediate files
MOC_DIR += ./generated
OBJECTS_DIR += ./generated

# Resource file
RESOURCES = uProbeX.qrc

# Include actual project
include(uProbeX.pri)
