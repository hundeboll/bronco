include_directories(.)
link_libraries(ncurses)
add_executable(bronco-nc nc.cpp)
target_link_libraries(bronco-nc bronco protocol)
