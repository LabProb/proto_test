cmake_minimum_required(VERSION 3.5)
project(proto_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

# =========================
# Protobuf
# =========================

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/system.proto)
set(GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

set(PROTO_SRC
    ${GENERATED_SRC_DIR}/system.pb.cc
)

set(PROTO_HDR
    ${GENERATED_SRC_DIR}/system.pb.h
)

add_custom_command(
    OUTPUT ${PROTO_SRC}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${GENERATED_SRC_DIR}
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_library(system_proto ${PROTO_SRC})

target_include_directories(system_proto PUBLIC
    ${GENERATED_SRC_DIR}
)

target_link_libraries(system_proto
    PUBLIC
    protobuf::libprotobuf
)

# =========================
# Executable
# =========================

add_executable(proto_test src/main.cpp)

target_link_libraries(proto_test
    PRIVATE
    system_proto
    Threads::Threads
)