project(bronco)
cmake_minimum_required(VERSION 2.8)
add_definitions(-pedantic -Wall -ansi -msse2 -O2 -ftree-vectorize -pipe)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(PROTOBUF_INCLUDE_DIR src/proto)
find_package(Protobuf REQUIRED)


include_directories(src/libbronco/)
add_subdirectory(src/libbronco)
add_subdirectory(src/daemon)
add_subdirectory(src/nc)
