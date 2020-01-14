cmake_minimum_required(VERSION 3.5)

project(Audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

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
#              "${PROJECT_SOURCE_DIR}/scenes/warpScene.cpp"
)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_executable(Audio "${Audio_SRCS}")
add_dependencies(Audio fftwpp_static cpptoml_static range-v3_static)
target_include_directories(Audio PUBLIC "${PROJECT_SOURCE_DIR}/core" "${PROJECT_SOURCE_DIR}/audio" "${PROJECT_SOURCE_DIR}/scenes")
target_include_directories(Audio SYSTEM PUBLIC ${SOUNDIO_INCLUDE_DIR} ${OGRE_INCLUDE_DIRS})
target_compile_definitions(Audio PUBLIC SPDLOG_FMT_EXTERNAL)
target_link_libraries(Audio ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} fmt::fmt fmt::fmt-header-only OgreMain OgreBites gRPC::grpc gRPC::grpc++ ${SOUNDIO_LIBRARY} proto ${FFTW_LIBRARIES})
target_link_libraries(Audio range-v3_static cpptoml_static fftwpp_static)
target_compile_options(Audio PRIVATE -Wall -Wextra)
