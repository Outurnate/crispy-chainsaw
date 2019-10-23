#!/bin/bash
git submodule update --init --recursive --progress
mkdir -p build
pushd build
conan install --build missing ..
popd
cmake -H. -Bbuild -GNinja
cmake --build build
