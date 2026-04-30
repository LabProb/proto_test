# Protobuf: Minimal Reusable Pattern (C++)

  1. .proto                                                                   
  syntax = "proto3";                                                          
  package mypkg;                                                              
                                                                              
  message Data { ... }                                                        
                                                                              
  2. CMake                                                                    
  find_package(Protobuf)                                                      
  add_custom_command(protoc)                                                  
  add_library(proto)                                                          
  link                                                                        
  3. Код                                                                      
  Data d;                                                                     
  d.set_...                                                                   
                                                                              
  d.SerializeToOstream(...)                                                   
  d.ParseFromIstream(...) 

***
Базовий шаблон використання protobuf у C++ проєкті (без gRPC).
***

## 1. Опис контракту (.proto)

```cpp
syntax = "proto3";

package mypkg;

message Data {
  float value = 1;
  string name = 2;
}
```
Що тут важливо:
опис структури даних
типи полів
номери полів (critical!)

```cpp
find_package(Protobuf REQUIRED)

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/data.proto)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

add_custom_command(
    OUTPUT ${GENERATED_DIR}/data.pb.cc
           ${GENERATED_DIR}/data.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${GENERATED_DIR}
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_library(proto_lib
    ${GENERATED_DIR}/data.pb.cc
)

target_include_directories(proto_lib PUBLIC
    ${GENERATED_DIR}
)

target_link_libraries(proto_lib PUBLIC
    protobuf::libprotobuf
)
```
Що відбувається:
викликається protoc
генеруються .pb.h / .pb.cc
створюється бібліотека згенерованого коду

```cpp
#include <fstream>
#include <iostream>
#include "data.pb.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // --- створення ---
    mypkg::Data d;
    d.set_value(42.0f);
    d.set_name("example");

    // --- serialize ---
    {
        std::ofstream out("data.bin", std::ios::binary);
        d.SerializeToOstream(&out);
    }

    // --- parse ---
    mypkg::Data loaded;
    {
        std::ifstream in("data.bin", std::ios::binary);
        loaded.ParseFromIstream(&in);
    }

    // --- використання ---
    std::cout << loaded.value() << "\n";
    std::cout << loaded.name() << "\n";

    google::protobuf::ShutdownProtobufLibrary();
}
```
4. Як використовувати в новому проєкті
Пишеш .proto (контракт)
Додаєш protoc у CMake
Генеруєш .pb файли
Лінкуєш protobuf
Використовуєш:
set_*
SerializeToOstream
ParseFromIstream
5. Важливі правила

❌ НЕ змінювати номери полів
❌ НЕ змінювати типи (breaking change)

✔️ можна додавати нові поля
✔️ protobuf = binary формат (нечитабельний вручну)

6. Ментальна модель

.proto → генерація коду → C++ клас → serialize/parse через protobuf runtime