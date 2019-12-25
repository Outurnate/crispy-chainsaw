cmake_minimum_required(VERSION 3.13)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# vcpkg cmake
find_package(Threads REQUIRED)
find_package(FFTW3 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(libsoundio CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

find_path(FFTWPP_SOURCES fftwpp/fftw++.h)
add_library(fftwpp STATIC
            "${FFTWPP_SOURCES}/fftwpp/Array.cc"
            "${FFTWPP_SOURCES}/fftwpp/Complex.cc"
            "${FFTWPP_SOURCES}/fftwpp/convolution.cc"
            "${FFTWPP_SOURCES}/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${FFTWPP_SOURCES}")

# our source
set(Audio_SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp"
               "${PROJECT_SOURCE_DIR}/src/sceneManager.cpp"
#              "${PROJECT_SOURCE_DIR}/src/resourceManager.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioEngine.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioAnalyzer.cpp"
               "${PROJECT_SOURCE_DIR}/src/circleSpectrumScene.cpp"
#              "${PROJECT_SOURCE_DIR}/src/warpScene.cpp"
               "${PROJECT_SOURCE_DIR}/src/soundIO.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioOutput.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioProvider.cpp")

add_executable(Audio "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile" "${PROJECT_SOURCE_DIR}/libs/bigg/include" "${FFTWPP_SOURCES}" ${LIBSAMPLERATE_INCLUDE_DIR} ${BGFX_INCLUDE_DIR} ${BX_INCLUDE_DIR} ${BIMG_INCLUDE_DIR})
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} glfw ${BIMG_LIBRARY} ${BGFX_LIBRARY} ${BX_LIBRARY} fftwpp glfw glm fftw3 imgui::imgui glfw ${OPENGL_LIBRARIES} libsoundio::libsoundio meta concepts range-v3 spdlog::spdlog fmt::fmt fmt::fmt-header-only)
target_compile_options(Audio PRIVATE -Wall -Wextra)
