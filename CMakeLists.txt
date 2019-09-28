cmake_minimum_required(VERSION 3.1)

project(Audio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Python COMPONENTS Interpreter REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Portaudio REQUIRED)
find_package(FFTW REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set_source_files_properties("${PROJECT_BINARY_DIR}/src/glad.c"          PROPERTIES GENERATED TRUE)
set_source_files_properties("${PROJECT_BINARY_DIR}/include/glad/glad.h" PROPERTIES GENERATED TRUE)
set_source_files_properties("${PROJECT_SOURCE_DIR}/libs/imgui/examples/imgui_impl_opengl3.cpp" PROPERTIES OBJECT_DEPENDS "${PROJECT_BINARY_DIR}/include/glad/glad.h")

add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/src/glad.c" "${PROJECT_BINARY_DIR}/include/glad/glad.h"
                   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/libs/glad"
                   COMMAND "${Python_EXECUTABLE}"
                   ARGS -m glad --profile="core" --api="gl=4.0" --generator="c" --spec="gl" --extensions="" --out "${PROJECT_BINARY_DIR}")

set(Audio_SRCS "${PROJECT_SOURCE_DIR}/src/audio.cpp" "${PROJECT_SOURCE_DIR}/src/audioStream.cpp")

add_library(AudioFile STATIC "${PROJECT_SOURCE_DIR}/libs/AudioFile/AudioFile.cpp")
target_include_directories(AudioFile PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile")

add_library(fftwpp STATIC "${PROJECT_SOURCE_DIR}/libs/fftwpp/Array.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/Complex.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/convolution.cc" "${PROJECT_SOURCE_DIR}/libs/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${PROJECT_SOURCE_DIR}/libs/fftwpp" ${FFTW_INCLUDE_DIRS})

add_library(imgui STATIC "${PROJECT_SOURCE_DIR}/libs/imgui/imgui.cpp" "${PROJECT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp" "${PROJECT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp" "${PROJECT_SOURCE_DIR}/libs/imgui/examples/imgui_impl_glfw.cpp" "${PROJECT_SOURCE_DIR}/libs/imgui/examples/imgui_impl_opengl3.cpp")
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/libs/imgui" "${PROJECT_BINARY_DIR}/include")
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_library(imgui-plot STATIC "${PROJECT_SOURCE_DIR}/libs/imgui-plot/src/imgui_plot.cpp")
target_include_directories(imgui-plot PUBLIC "${PROJECT_SOURCE_DIR}/libs/imgui-plot/include" "${PROJECT_SOURCE_DIR}/libs/imgui")

add_executable(Audio "${PROJECT_BINARY_DIR}/src/glad.c" "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_BINARY_DIR}/include" "${PROJECT_SOURCE_DIR}/libs/AudioFile" "${PROJECT_SOURCE_DIR}/libs" "${PROJECT_SOURCE_DIR}/libs/imgui" "${PROJECT_SOURCE_DIR}/libs/imgui-plot/include" ${GLFW_INCLUDE_DIRS})
target_link_libraries(Audio ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS} AudioFile fftwpp imgui imgui-plot ${PORTAUDIO_LIBRARIES} Threads::Threads ${FFTW_LIBRARIES})
