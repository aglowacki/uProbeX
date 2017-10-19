#export EPICS_FOLDER=/local/user/src/EPICS-3.14.14.2
#export EPICS_BASE=$EPICS_FOLDER
#export EPICS_TARGET_OS=Linux
#export EPICS_HOST_ARCH=linux-x86_64
export HDF5_BASE=/local/aglowacki/libs/libHDF
export QT_BASE=/local/aglowacki/libs/Qt_5.8
export QTDIR=$QT_BASE
export QTLIB=$QT_BASE/lib
export QTINC=$QT_BASE/include
export GSTAR_BASE=/local/aglowacki/src/gstar14
export XRF_MAPS_BASE=/local/aglowacki/src/XRF-Maps
export EIGEN3_BASE=/local/aglowacki/libs/eigen3/include/eigen3
export ZMQ_BASE=/local/aglowacki/libs/zmq
#export SSTAR_BASE=/local/user/src/sstar
#export LD_LIBRARY_PATH=$HDF5_BASE/lib:$QT_BASE/lib:$EPICS_FOLDER/lib/$EPICS_HOST_ARCH:$GSTAR_BASE/lib:$SSTAR_BASE/bin/shared
export LD_LIBRARY_PATH=$HDF5_BASE/lib:$NETCDF_BASE/lib:$QT_BASE/lib:$GSTAR_BASE/lib:$XRF_MAPS_BASE/bin:$LD_LIBRARY_PATH:$ZMQ_BASE/lib
