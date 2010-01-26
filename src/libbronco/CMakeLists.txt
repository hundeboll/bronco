include_directories(.)
link_libraries(boost_system-mt boost_thread-mt protoc protoc)
add_library(bronco SHARED arbitrator.cpp peermanager.cpp connection.cpp peerconnection.cpp serverconnection.cpp coder.cpp)
