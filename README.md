# proto_test

## Overview

`proto_test` is a small C++20 gRPC service and client that demonstrate a Protocol Buffers contract, build-time C++ code generation, and unary RPC calls. The server exposes telemetry readings and accepts a request to switch its operating mode.

## Purpose

`MetricsService` has two unary RPCs:

- `GetMetrics` returns fixed CPU, memory, and temperature values together with the current mode.
- `SetMode` changes the server mode to `eco` or `performance`.

The client reads the metrics, changes the mode to `performance`, and reads the metrics again. The server keeps the mode in memory and protects it with a mutex.

## Technologies and prerequisites

- CMake 3.5 or newer
- A C++20-capable compiler
- Protocol Buffers development libraries and the `protoc` compiler
- gRPC C++ development libraries and the `grpc_cpp_plugin` executable

The project uses CMake as its build system. The included Makefile is only a convenience wrapper around CMake.

## Project structure

```
.
├── CMakeLists.txt          # Configures code generation and C++ targets
├── Makefile                # Convenience build and run targets
├── proto/system.proto      # Protobuf messages and MetricsService contract
├── src/grpc/
│   ├── grpc_server.cpp     # Thread-safe MetricsService implementation
│   └── grpc_client.cpp     # Example client workflow
├── protobuf.md             # General protobuf C++ notes
└── README.md
```

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build --parallel
```

This produces `build/system_server` and `build/system_client`.

The equivalent convenience command is:

```sh
make build
```

## Run

Start the server in one terminal. It listens on `0.0.0.0:50051` and runs until interrupted.

```sh
./build/system_server
```

In another terminal, run the client:

```sh
./build/system_client
```

With a reachable server, the client prints the initial `eco` metrics, reports a successful mode update, then prints metrics with mode `performance`. If an RPC cannot be completed, the client writes the gRPC status code and message to standard error and exits unsuccessfully.

Makefile run helpers are also available:

```sh
make run-server
make run-client
```

`make run` is an alias for `make run-server`.

## Tests

There is currently no automated test target. After building, this command confirms that no CTest tests are registered:

```sh
ctest --test-dir build --output-on-failure
```

Use the two-terminal server/client workflow above as the integration check.

## Protocol Buffers and gRPC generation

[`proto/system.proto`](proto/system.proto) uses `proto3` syntax and the `telemetry` package. It defines the `Metrics`, `SetModeRequest`, `SetModeResponse`, and `Empty` messages, plus `MetricsService`.

During the build, CMake invokes `protoc` with `grpc_cpp_plugin` and writes these generated files to `build/generated/`:

- `system.pb.h` and `system.pb.cc` for Protocol Buffers messages
- `system.grpc.pb.h` and `system.grpc.pb.cc` for gRPC service stubs

Generated files are build artifacts; they are not committed and should not be edited by hand. `system_proto` compiles them and links both executables to the Protocol Buffers runtime and gRPC C++ library.

## Implementation notes

`MetricsServiceImpl` serializes access to its mutable `mode_` member with `std::mutex`. gRPC owns request/response lifetimes for each call; the server fills the provided response object and returns `grpc::Status::OK` for valid RPC delivery. An invalid mode is represented by a successful transport status with `success = false` in `SetModeResponse`.

The server checks whether gRPC successfully created its listening server before calling `Wait()`. This makes a port-binding failure a clear error instead of a null-pointer crash.

## Troubleshooting

- If CMake cannot find gRPC or Protocol Buffers, install their C++ development packages along with `protoc` and `grpc_cpp_plugin`, then configure again.
- If the client reports an unavailable RPC, start `system_server` first and ensure port `50051` is available.
