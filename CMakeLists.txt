cmake_minimum_required(VERSION 3.1)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(libsoundio CONFIG REQUIRED)

# non-cmake
find_library(LIBSAMPLERATE_LIBRARY NAMES libsamplerate-0)
find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

set(Audio_SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp"
               "${PROJECT_SOURCE_DIR}/src/gfxUtils.cpp"
               "${PROJECT_SOURCE_DIR}/src/sceneManager.cpp"
               "${PROJECT_SOURCE_DIR}/src/resourceManager.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioEngine.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioAnalyzer.cpp"
               "${PROJECT_SOURCE_DIR}/src/circleSpectrumScene.cpp"
               "${PROJECT_SOURCE_DIR}/src/warpScene.cpp")

add_library(AudioFile STATIC "${PROJECT_SOURCE_DIR}/libs/AudioFile/AudioFile.cpp")
target_include_directories(AudioFile PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile")

add_library(fftwpp STATIC "${PROJECT_SOURCE_DIR}/libs/fftwpp/Array.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/Complex.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/convolution.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${PROJECT_SOURCE_DIR}/libs/fftwpp")

add_executable(Audio "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile" "${PROJECT_SOURCE_DIR}/libs/bigg/include" "${PROJECT_SOURCE_DIR}/libs")
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} AudioFile fftwpp bigg)
