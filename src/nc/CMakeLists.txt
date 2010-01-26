include_directories(.)

add_executable(bronco-nc nc.cpp)
target_link_libraries(bronco-nc bronco protocol)
