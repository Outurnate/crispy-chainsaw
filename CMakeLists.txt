cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0079 NEW)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

## TODO REMOVE
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

add_subdirectory("${PROJECT_SOURCE_DIR}/assets")

# vcpkg cmake
find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(FFTW3 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(libsoundio CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

find_package(OpenGL REQUIRED)

# non-cmake vcpkg
find_library(LIBSAMPLERATE_LIBRARY NAMES libsamplerate-0)
find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

find_library(BGFX_LIBRARY NAMES bgfxRelease
             HINTS "${PROJECT_SOURCE_DIR}/../bgfx/.build/linux64_gcc/bin")
find_path(BGFX_INCLUDE_DIR bgfx/bgfx.h
          HINTS "${PROJECT_SOURCE_DIR}/../bgfx/include")

find_path(FFTWPP_SOURCES fftwpp/fftw++.h)

# non-cmake non-vcpkg
find_library(BX_LIBRARY NAMES bxRelease
             HINTS "${PROJECT_SOURCE_DIR}/../bgfx/.build/linux64_gcc/bin")
find_path(BX_INCLUDE_DIR bx/bx.h
          HINTS "${PROJECT_SOURCE_DIR}/../bx/include")

find_library(BIMG_LIBRARY NAMES bimgRelease
             HINTS "${PROJECT_SOURCE_DIR}/../bgfx/.build/linux64_gcc/bin")
find_path(BIMG_INCLUDE_DIR bimg/bimg.h
          HINTS "${PROJECT_SOURCE_DIR}/../bimg/include")

# bundled deps
add_library(AudioFile STATIC "${PROJECT_SOURCE_DIR}/libs/AudioFile/AudioFile.cpp")
target_include_directories(AudioFile PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile")

add_library(fftwpp STATIC
            "${FFTWPP_SOURCES}/fftwpp/Array.cc"
            "${FFTWPP_SOURCES}/fftwpp/Complex.cc"
            "${FFTWPP_SOURCES}/fftwpp/convolution.cc"
            "${FFTWPP_SOURCES}/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${FFTWPP_SOURCES}")

add_subdirectory("${PROJECT_SOURCE_DIR}/libs/bigg")

target_include_directories(bigg PUBLIC ${BGFX_INCLUDE_DIR} ${BX_INCLUDE_DIR} ${BIMG_INCLUDE_DIR})
target_link_libraries(bigg PUBLIC imgui::imgui glfw)

# our source
set(Audio_SRCS "${PROJECT_SOURCE_DIR}/src/main.cpp"
               "${PROJECT_SOURCE_DIR}/src/gfxUtils.cpp"
               "${PROJECT_SOURCE_DIR}/src/sceneManager.cpp"
               "${PROJECT_SOURCE_DIR}/src/resourceManager.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioEngine.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioAnalyzer.cpp"
               "${PROJECT_SOURCE_DIR}/src/circleSpectrumScene.cpp"
               "${PROJECT_SOURCE_DIR}/src/warpScene.cpp"
               "${PROJECT_SOURCE_DIR}/src/soundIO.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioOutput.cpp"
               "${PROJECT_SOURCE_DIR}/src/audioProvider.cpp")

add_executable(Audio "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/libs/AudioFile" "${PROJECT_SOURCE_DIR}/libs/bigg/include" "${FFTWPP_SOURCES}" ${LIBSAMPLERATE_INCLUDE_DIR} ${BGFX_INCLUDE_DIR} ${BX_INCLUDE_DIR} ${BIMG_INCLUDE_DIR})
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} glfw ${BIMG_LIBRARY} ${BGFX_LIBRARY} ${BX_LIBRARY} AudioFile fftwpp glfw glm fftw3 imgui::imgui bigg glfw ${OPENGL_LIBRARIES} libsoundio::libsoundio meta concepts range-v3 spdlog::spdlog fmt::fmt fmt::fmt-header-only)
target_compile_options(Audio PRIVATE -Wall -Wextra)
