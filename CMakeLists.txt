cmake_minimum_required(VERSION 3.5)
project(proto_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
# =========================
# Protobuf
# =========================

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/system.proto)
set(GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

set(PROTO_SRC
    ${GENERATED_SRC_DIR}/system.pb.cc
    ${GENERATED_SRC_DIR}/system.grpc.pb.cc
)

set(PROTO_HDR
    ${GENERATED_SRC_DIR}/system.pb.h
    ${GENERATED_SRC_DIR}/system.grpc.pb.h
)

add_custom_command(
    OUTPUT ${PROTO_SRC}
    COMMAND ${PROTOC}
    ARGS --grpc_out=${GENERATED_SRC_DIR}
    	 --cpp_out=${GENERATED_SRC_DIR}
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_library(system_proto ${PROTO_SRC})

target_include_directories(system_proto PUBLIC
    ${GENERATED_SRC_DIR}
)

target_link_libraries(system_proto
    PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

# =========================
# Executable
# =========================

add_executable(system_server src/grpc/grpc_server.cpp)

target_link_libraries(system_server
    PRIVATE
    system_proto
    Threads::Threads
)

add_executable(system_client src/grpc/grpc_client.cpp)

target_link_libraries(system_client
    PRIVATE
    system_proto
    Threads::Threads
)

