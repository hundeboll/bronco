include_directories(.)

add_executable(broncod broncod.cpp)
target_link_libraries(broncod broncoserver)
