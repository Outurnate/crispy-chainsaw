cmake_minimum_required(VERSION 3.1)

project(Audio)

set(OpenGL_GL_Preference "GLVND")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Portaudio REQUIRED)
find_package(FFTW REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/libs/bigg")

set(Audio_SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp" "${PROJECT_SOURCE_DIR}/src/audioEngine.cpp" "${PROJECT_SOURCE_DIR}/src/circleSpectrumScene.cpp" "${PROJECT_SOURCE_DIR}/src/audioAnalyzer.cpp" "${PROJECT_SOURCE_DIR}/src/gfxUtils.cpp")

add_library(AudioFile STATIC "${PROJECT_SOURCE_DIR}/libs/AudioFile/AudioFile.cpp")
target_include_directories(AudioFile PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile")

add_library(fftwpp STATIC "${PROJECT_SOURCE_DIR}/libs/fftwpp/Array.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/Complex.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/convolution.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${PROJECT_SOURCE_DIR}/libs/fftwpp" ${FFTW_INCLUDE_DIRS})

add_library(imgui-plot STATIC "${PROJECT_SOURCE_DIR}/libs/imgui-plot/src/imgui_plot.cpp")
target_include_directories(imgui-plot PUBLIC "${PROJECT_SOURCE_DIR}/libs/imgui-plot/include" "${PROJECT_SOURCE_DIR}/libs/bigg/deps/imgui")

add_executable(Audio "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_BINARY_DIR}/include" "${PROJECT_SOURCE_DIR}/libs/AudioFile" "${PROJECT_SOURCE_DIR}/libs" "${PROJECT_SOURCE_DIR}/libs/imgui" "${PROJECT_SOURCE_DIR}/libs/imgui-plot/include" ${GLFW_INCLUDE_DIRS})
target_link_libraries(Audio ${CMAKE_DL_LIBS} AudioFile fftwpp imgui imgui-plot glfw glm Threads::Threads bigg ${PORTAUDIO_LIBRARIES} ${FFTW_LIBRARIES})

file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${PROJECT_BINARY_DIR}")
