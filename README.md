# Protobuf C++ Mini Project

## What is implemented here

- Data structure description via `.proto`
- C++ code generation via `protoc`
- Serialization to binary file
— Deserialization of file back to C++ object

---

## Project structure

├── CMakeLists.txt
├── include
├── Makefile
├── proto
│     └── system.proto
├── README.md
└── src
    └── main.cpp

## Dependencies

Need to install:
- protobuf (libprotobuf + protoc)
- CMake
- C++20 compiler
On Ubuntu:

```bash
sudo apt install protobuf-compiler libprotobuf-dev
```

launch
```bash
make build
make run
```

proto_test$ xxd ./build/metrics.bin 
00000000: 0d00 0028 4215 0000 8242 1d00 005e 4222  ...(B....B...^B"
00000010: 0365 636f                                .eco

0d00 0028 4215 0000 8242 1d00 005e 4222 0365 636f

[tag][data]
tag = (field_number << 3) | wire_type

0D 00 00 28 42 
0x0D (13) = tag 
0x0D = 0000 1101 (binary)
field_number = 00001 = 1
wire_type = 101 = 5 (fixed32)
(float cpu_usage = 1;)
00 00 28 42
Little-endian → 0x42280000
42.0 (float, IEEE 754)

15 00 00 82 42
0x15 (21) = tag  
0x15 = 0001 0101 (binary)
field_number = 00010 = 2
wire_type = 101 = 5 (fixed32)
(float memory_usage = 2;)
00 00 82 42
Little-endian → 0x42820000
65.0(float, IEEE 754)

1D 00 00 5E 42
0x1D (29) = tag  
0x1D = 0001 1101 (binary)
field_number = 00011 = 3
wire_type = 101 = 5 (fixed32)
(float temperature = 3;)
00 00 5E 42
Little-endian → 0x425E0000
55.5(float, IEEE 754)

22 03 65 63 6F
0x22 (34) = tag
0x22 = 0010 0010 (binary)
field_number = 00100 = 4
wire_type = 010 = 2 (length-delimited)
03 - length = 3 bytes
65 63 6F = 'e' 'c' 'o'