include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

externalproject_add(fftwpp_sources
  URL https://github.com/dealias/fftwpp/archive/d05a2812995a52a3834140fd3ddd2e80bf8fcd42.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

set(FFTWPP_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/fftwpp_sources-prefix/src/fftwpp_sources")
#add_library(fftwpp STATIC
#            "${FFTWPP_SOURCES}/Array.cc"
#            "${FFTWPP_SOURCES}/Complex.cc"
#            "${FFTWPP_SOURCES}/convolution.cc"
#            "${FFTWPP_SOURCES}/fftw++.cc"
#            DEPENDS fftwpp_sources)
#target_include_directories(fftwpp PUBLIC "${FFTWPP_SOURCES}")

externalproject_add(cpptoml_sources
  URL https://github.com/skystrife/cpptoml/archive/v0.1.1.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

add_library(cpptoml INTERFACE)
target_include_directories(cpptoml INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/cpptoml_sources-prefix/src/cpptoml_sources/include")
add_dependencies(cpptoml cpptoml_sources)
