cmake_minimum_required(VERSION 3.5)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_subdirectory("${PROJECT_SOURCE_DIR}/assets")

# vcpkg cmake
find_package(Threads REQUIRED)
find_package(FFTW3 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(libsoundio CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

find_path(FFTWPP_SOURCES fftwpp/fftw++.h)
add_library(fftwpp STATIC
            "${FFTWPP_SOURCES}/fftwpp/Array.cc"
            "${FFTWPP_SOURCES}/fftwpp/Complex.cc"
            "${FFTWPP_SOURCES}/fftwpp/convolution.cc"
            "${FFTWPP_SOURCES}/fftwpp/fftw++.cc")
target_include_directories(fftwpp PUBLIC "${FFTWPP_SOURCES}")

# our source
set(Audio_SRCS "${PROJECT_SOURCE_DIR}/core/main.cpp"
               "${PROJECT_SOURCE_DIR}/core/SceneManager.cpp"
               "${PROJECT_SOURCE_DIR}/core/ConfigurationManager.cpp"
               "${PROJECT_SOURCE_DIR}/core/Application.cpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioEngine.cpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioAnalyzer.cpp"
               "${PROJECT_SOURCE_DIR}/audio/SoundIO.cpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioOutput.cpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioProvider.cpp"
               "${PROJECT_SOURCE_DIR}/scenes/BlackHoleScene.cpp"
#              "${PROJECT_SOURCE_DIR}/scenes/warpScene.cpp"
)

add_executable(Audio "${Audio_SRCS}")
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/core" "${PROJECT_SOURCE_DIR}/audio" "${PROJECT_SOURCE_DIR}/scenes")
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} fftwpp fftw3 libsoundio::libsoundio meta concepts range-v3 spdlog::spdlog fmt::fmt fmt::fmt-header-only OgreMain OgreBites ${OGRE_RenderSystem_GL3Plus_LIBRARIES})
target_compile_options(Audio PRIVATE -Wall -Wextra)
