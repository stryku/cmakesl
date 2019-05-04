#!/bin/bash

COMPILER=$1

# Clone CMake codebase
git clone https://gitlab.kitware.com/cmake/cmake.git
cd cmake

# CMakeSL bases on this particular release
git checkout v3.14.3

# Clone CMakeSL in the `Source` directory
cd Source
git clone https://github.com/stryku/cmakesl

# Apply needed changes to CMake code
cd ..
git apply Source/cmakesl/cmake_integration/cmake_changes.patch

# Build CMake. C++17 compiler is required
cd ..
mkdir build
mkdir install
cd build
cmake ../cmake -DCMAKE_CXX_COMPILER=$COMPILER -DCMAKE_INSTALL_PREFIX=../install
make install -j
cd ..
