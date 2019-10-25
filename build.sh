#!/bin/bash
git submodule update --init --recursive --progress
mkdir -p build
sudo vcpkg install glfw3 glm imgui libsamplerate boost-core boost-circular-buffer boost-lockfree libsoundio fftw3 opengl
cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/vcpkg/scripts/buildsystems/vcpkg.cmake -H. -Bbuild -GNinja
cmake --build build
