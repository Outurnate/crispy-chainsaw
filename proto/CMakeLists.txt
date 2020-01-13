find_file(_GRPC_CPP_PLUGIN_EXECUTABLE
  NAMES grpc_cpp_plugin
#  PATHS ${_VCPKG_ROOT_DIR}/packages/grpc_${VCPKG_TARGET_TRIPLET}/tools/grpc
#  NO_DEFAULT_PATH
)

find_file(_PROTOBUF_PROTOC
  NAMES protoc
#  PATHS ${_VCPKG_ROOT_DIR}/packages/protobuf_${VCPKG_TARGET_TRIPLET}/tools/protobuf
#  NO_DEFAULT_PATH
)

set(proto "${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationServer.proto")
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationServer.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationServer.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationServer.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationServer.grpc.pb.h")
add_custom_command(
  OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
  COMMAND ${_PROTOBUF_PROTOC}
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
       --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
       -I "${CMAKE_CURRENT_SOURCE_DIR}"
       --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
       "${proto}"
  DEPENDS "${proto}")

add_library(proto STATIC "${proto_srcs}" "${grpc_srcs}")
target_include_directories(proto PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
