include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

externalproject_add(fftwpp_sources
  URL https://github.com/dealias/fftwpp/archive/d05a2812995a52a3834140fd3ddd2e80bf8fcd42.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

add_library(fftwpp_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/fftwpp.cpp)
target_include_directories(fftwpp_static PRIVATE SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/fftwpp_sources-prefix/src/fftwpp_sources")
add_dependencies(fftwpp_static fftwpp_sources)
set(FFTWPP_INCLUDE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fftwpp_sources-prefix/src/fftwpp_sources" PARENT_SCOPE) 

externalproject_add(cpptoml_sources
  URL https://github.com/skystrife/cpptoml/archive/v0.1.1.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

add_library(cpptoml_static INTERFACE)
target_include_directories(cpptoml_static INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/cpptoml_sources-prefix/src/cpptoml_sources/include")
add_dependencies(cpptoml_static cpptoml_sources)

externalproject_add(range-v3_sources
  URL https://github.com/ericniebler/range-v3/archive/0.10.0.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

add_library(range-v3_static INTERFACE)
target_include_directories(range-v3_static INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/range-v3_sources-prefix/src/range-v3_sources/include")
add_dependencies(range-v3_static range-v3_sources)
