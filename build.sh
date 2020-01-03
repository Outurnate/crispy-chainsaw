#!/bin/bash
git submodule update --init --recursive --progress
mkdir -p build
sudo vcpkg install boost-core boost-circular-buffer boost-lockfree libsoundio fftw3 range-v3 spdlog fftwpp fmt cpptoml ogre
cmake -DCMAKE_CXX_FLAGS="-ggdb" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/usr/share/vcpkg/scripts/buildsystems/vcpkg.cmake -H. -Bbuild
cmake --build build
