#!/bin/bash
git submodule update --init --recursive --progress
mkdir -p build
cmake -H. -Bbuild -GNinja
cmake --build build
