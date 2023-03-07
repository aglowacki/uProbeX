[![Actions Status](https://github.com/aglowacki/XRF-Maps/workflows/CMake/badge.svg)](https://github.com/aglowacki/XRF-Maps/actions)
[![Build Status](https://dev.azure.com/aglow/XRF-Maps/_apis/build/status/aglowacki.XRF-Maps?branchName=master)](https://dev.azure.com/aglow/XRF-Maps/_build/latest?definitionId=2&branchName=master)

# uProbeX

uProbeX is a GUI for  X-ray fluorescence (XRF) : https://github.com/AdvancedPhotonSource/XRF-Maps

# Compiling

## Requires

Visual Studio 2015 or greater (Windows build)
GCC 6.0 or greater (Linux build)
Cmake 3.5 or greater

## Libraries

HDF5 : https://www.hdfgroup.org/downloads/
NetCDF : http://www.unidata.ucar.edu/downloads/netcdf/index.jsp (Download http://www.unidata.ucar.edu/software/netcdf/docs/winbin.html)
Eigen : http://eigen.tuxfamily.org/index.php?title=Main_Page (submodule in src/support)
OpenCV
Tiff
Qt
ZeroMQ

## Compile Default

1) git clone --recurse-submodules https://github.com/AdvancedPhotonSource/XRF-Maps.git
1) git clone https://github.com/AdvancedPhotonSource/uProbeX.git
2) cd XRF-Maps
3) mkdir build
4) cd vcpkg
 Note that if you have Qt installed though the QT installer, you don't need to have qt5-base or qt5-charts, you will have to point cmake to them.
5) vcpkg set Linux
   1) ./bootstrap-vcpkg.sh
   2) ./vcpkg install hdf5 netcdf-c yaml-cpp zeromq opencv tiff jsoncpp qt5-base qt5-charts
6) vcpkg setup windows
   1) .\bootstrap-vcpkg.bat
   2) .\vcpkg install hdf5 netcdf-c yaml-cpp zeromq opencv tiff jsoncpp qt5-base qt5-charts --triplet x64-windows
7) cd ../build
8) cmake `-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_WITH_ZMQ=ON ..`
9) make
10) cd ../../uProbeX
11) mkdir build
12) cd build
13) cmake `-DCMAKE_TOOLCHAIN_FILE=../../XRF-Maps/vcpkg/scripts/buildsystems/vcpkg.cmake ..`
14) make

