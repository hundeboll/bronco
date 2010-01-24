include_directories(.)
link_libraries(boost_system-mt)
add_library(bronco SHARED arbitrator.cpp peermanager.cpp peerconnection.cpp)
