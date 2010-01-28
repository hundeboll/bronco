include_directories(.)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS messages.proto)
add_library(protocol SHARED ${PROTO_SRCS})
