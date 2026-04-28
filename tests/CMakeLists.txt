# CMakeLists.txt for uProbeX Tests
#
# Copyright (c) 2024, UChicago Argonne, LLC. All rights reserved.

cmake_minimum_required(VERSION 3.5)

project(uProbeX_Tests)

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Find Qt6
find_package(Qt6 COMPONENTS Core REQUIRED)

# Find ZeroMQ
find_package(cppzmq CONFIG REQUIRED)
find_package(XRF_Maps CONFIG REQUIRED COMPONENTS libxrf_io libxrf_fit)
# Include directories from main project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

include_directories(${XRF_Maps_DIR}/../src)
include_directories(${XRF_Maps_DIR}/../src/support/eigen-git-mirror)


# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# BlueskyComm Test
add_executable(BlueskyCommTest
    BlueskyCommTest.cpp
)

target_link_libraries(BlueskyCommTest
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    GTest::gmock
    GTest::gmock_main
    Qt6::Core
    cppzmq
    cppzmq-static
    pthread
)

# Add test to CTest
add_test(NAME BlueskyCommTest COMMAND BlueskyCommTest)

# Set test properties
set_target_properties(BlueskyCommTest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
