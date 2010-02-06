include_directories(.)
link_libraries(boost_system-mt boost_thread-mt boost_regex-mt boost_iostreams-mt protoc protoc gf2 ssl)
add_library(bronco SHARED arbitrator.cpp peermanager.cpp connection.cpp peerconnection.cpp serverconnection.cpp coder.cpp)
add_library(broncoserver SHARED server.cpp connection.cpp clientconnection.cpp peerlist.cpp)
