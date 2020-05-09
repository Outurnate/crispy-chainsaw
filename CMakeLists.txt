cmake_minimum_required(VERSION 3.16)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(soundio REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OGRE REQUIRED)
find_package(ZLIB REQUIRED)
find_package(gRPC REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/assets")
add_subdirectory("${PROJECT_SOURCE_DIR}/proto")
add_subdirectory("${PROJECT_SOURCE_DIR}/libs")

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
               "${PROJECT_SOURCE_DIR}/scenes/WarpScene.cpp"
               "${PROJECT_SOURCE_DIR}/scenes/SolarSpiralScene.cpp"
)
set(Audio_HDRS "${PROJECT_SOURCE_DIR}/core/SceneManager.hpp"
               "${PROJECT_SOURCE_DIR}/core/ConfigurationManager.hpp"
               "${PROJECT_SOURCE_DIR}/core/Application.hpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioEngine.hpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioAnalyzer.hpp"
               "${PROJECT_SOURCE_DIR}/audio/SoundIO.hpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioOutput.hpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioProvider.hpp"
               "${PROJECT_SOURCE_DIR}/scenes/BlackHoleScene.hpp"
               "${PROJECT_SOURCE_DIR}/scenes/WarpScene.hpp"
               "${PROJECT_SOURCE_DIR}/scenes/SolarSpiralScene.hpp"
               "${PROJECT_SOURCE_DIR}/audio/AudioSystem.hpp"
               "${PROJECT_SOURCE_DIR}/core/Math.hpp"
)

add_executable(Audio "${Audio_SRCS}")
target_precompile_headers(Audio PRIVATE "${Audio_HDRS}")
add_dependencies(Audio fftwpp_static cpptoml_static range-v3_static)
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/core" "${PROJECT_SOURCE_DIR}/audio" "${PROJECT_SOURCE_DIR}/scenes")
target_include_directories(Audio SYSTEM PUBLIC ${SOUNDIO_INCLUDE_DIR} ${OGRE_INCLUDE_DIRS} ${FFTWPP_INCLUDE_DIRECTORY})
target_compile_definitions(Audio PUBLIC SPDLOG_FMT_EXTERNAL)
if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  target_compile_definitions(Audio PUBLIC DEBUG NRELEASE)
else()
  set_property(TARGET Audio PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  target_compile_definitions(Audio PUBLIC NDEBUG RELEASE)
endif()
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} fmt::fmt fmt::fmt-header-only OgreMain OgreBites gRPC::grpc gRPC::grpc++ ${SOUNDIO_LIBRARY} proto ${FFTW_LIBRARIES})
target_link_libraries(Audio range-v3_static cpptoml_static fftwpp_static)
target_compile_options(Audio PRIVATE -Wall -Wextra)
