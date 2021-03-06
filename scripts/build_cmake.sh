#!/bin/bash

COMPILER=$1

# Clone CMake codebase.
git clone https://github.com/stryku/cmake_for_cmakesl cmake
cd cmake
# CMakeSL bases on this particular branch.
git checkout cmakesl

# Clone CMakeSL in the `Source` directory.
cd Source
git clone https://github.com/stryku/cmakesl

# Build CMake. C++17 compiler is required.
cd ../..
mkdir build install
cd build
cmake ../cmake -DCMAKE_CXX_COMPILER=$COMPILER -DCMAKE_INSTALL_PREFIX=../install
make install -j$(nproc)
cd ..
# Now, in install/bin you have the `cmake` binary that supports CMakeSL.

# Build the cmsl_example
cd install/bin
cp -r ../../cmake/Source/cmakesl/examples/cmsl_example .
mkdir build && cd build
./../cmake ../cmsl_example
make -j$(nproc)
./hw_exec
