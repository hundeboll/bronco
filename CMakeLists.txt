project(bronco)
cmake_minimum_required(VERSION 2.8)
add_definitions(-pedantic -ggdb -Wall -Werror -ansi -msse2 -O2 -ftree-vectorize -pipe -funsigned-char)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(PROTOBUF_INCLUDE_DIR src/proto)
find_package(Protobuf REQUIRED)


include_directories(src/libbronco/)
include_directories(src/protocol/)
include_directories(src/third-party/)
add_subdirectory(src/third-party/gf2/)
add_subdirectory(src/protocol)
add_subdirectory(src/libbronco)
add_subdirectory(src/daemon)
add_subdirectory(src/nc)
