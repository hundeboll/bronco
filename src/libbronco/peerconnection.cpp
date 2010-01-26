/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>

#include <iostream>

#include "peerconnection.hpp"

void bronco::peerconnection::handle_peer()
{
    read_type();
}

void bronco::peerconnection::handle_connect(const boost::system::error_code &error)
{
    /* Handshake */
    protocol::Connect connect;
    connect.set_peer_hash("Hello World!");
    write_message(connect);
}

void bronco::peerconnection::handle_read(const boost::system::error_code &error, size_t type)
{
    if (!error) {
        protocol::Connect connect;

        switch (type) {
            case protocol::connecttype:
                deserialize(connect);
                break;
        }

        std::cout << "Received message: " << connect.peer_hash() << std::endl;
    } else {
        throw std::runtime_error("Read error: " + error.message());
    }

}

void bronco::peerconnection::handle_write(const boost::system::error_code &error)
{
    if (!error) {
        std::cout << "Message written" << std::endl;
    } else {
        throw std::runtime_error("Read error: " + error.message());
    }
}
