# Compiling

## Requires

Visual Studio 2022 or greater (Windows build)

GCC 11 or greater (Linux build)

Cmake 3.21 or greater

Python with meson and jinja2 installed

* conda install meson jinja2

## Compile Default

* git clone --recurse-submodules https://github.com/AdvancedPhotonSource/XRF-Maps.git

* git clone https://github.com/AdvancedPhotonSource/uProbeX.git

* cd XRF-Maps

* git submodule update --remote

* mkdir build

* cd vcpkg

 Note that if you have Qt installed though the QT installer, you don't need to have qtbase or qtcharts, you will have to point cmake to them.

 Note: Building on RedHat 9 you will need to checkout an older tag of vcpkg. 
   * git checkout tags/2022.11.14

* vcpkg set Linux

   * ./bootstrap-vcpkg.sh

   * ./vcpkg install hdf5 netcdf-c yaml-cpp zeromq opencv tiff jsoncpp qtbase qtcharts

* vcpkg setup windows

   * .\bootstrap-vcpkg.bat

   * .\vcpkg install hdf5 netcdf-c yaml-cpp zeromq opencv tiff jsoncpp qtbase qtcharts --triplet x64-windows

* cd ../build

* cmake `-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_WITH_ZMQ=ON ..`

* make -j 8

* cd ../../uProbeX

* mkdir build

* cd build

* cmake `-DCMAKE_TOOLCHAIN_FILE=../../XRF-Maps/vcpkg/scripts/buildsystems/vcpkg.cmake ..`

* make -j 8


# Upgrading

If you already performed the Compile defualt steps and want to upgrade the software you can run the following

* cd XRF-Maps

* git pull

* cd build

* make -j 8

* cd ../../uProbeX

* git pull

* cd build

* make -j 8
