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
    connect.set_peer_hash("connecting peer");
    write_message(connect);

    /* Wait for reply */
    read_type();
}

void bronco::peerconnection::handle_write(const boost::system::error_code &error)
{
    if (error) {
        handle_error(error);
    }
}

void bronco::peerconnection::handle_error(const boost::system::error_code &error)
{
    if (error == boost::asio::error::eof
            || error == boost::asio::error::connection_reset
            || error == boost::asio::error::bad_descriptor) {
        std::cout << "Connection closed" << std::endl;
    } else {
        throw std::runtime_error("Socket error: " + error.message());
    }
}

void bronco::peerconnection::handle_read(const boost::system::error_code &error, const size_t type)
{
    if (!error) {
            process_type(type);
    } else {
        handle_error(error);
    }
}

void bronco::peerconnection::process_type(const size_t type)
{
    protocol::Connect connect;
    protocol::Reply reply;
    protocol::Start start;
    protocol::Data data;
    protocol::Stop stop;
    protocol::Leave leave;

    switch (type) {
        case protocol::connecttype:
            deserialize(connect);
            process_message(connect);
            break;

        case protocol::replytype:
            deserialize(reply);
            process_message(reply);
            break;

        case protocol::starttype:
            deserialize(start);
            process_message(start);
            break;

        case protocol::datatype:
            deserialize(data);
            process_message(data);
            break;

        case protocol::stoptype:
            deserialize(stop);
            process_message(stop);
            break;

        case protocol::leavetype:
            deserialize(leave);
            process_message(leave);
            break;

        default:
            throw std::runtime_error("Unknown message type received");
            break;
    }
}

void bronco::peerconnection::process_message(const protocol::Connect &connect)
{
        std::cout << "Received connect from: " << connect.peer_hash() << std::endl;

        protocol::Reply reply;
        reply.set_peer_hash("accepting peer");
        write_message(reply);
}

void bronco::peerconnection::process_message(const protocol::Reply &reply)
{
        std::cout << "Received reply from: " << reply.peer_hash() << std::endl;

        /* Send start */
        protocol::Start start;
        start.set_peer_hash("connecting peer");
        write_message(start);
}

void bronco::peerconnection::process_message(const protocol::Start &start)
{
        std::cout << "Received start from: " << start.peer_hash() << std::endl;

        /* Send data packet */
        protocol::Data data;
        data.set_generation(1);
        write_message(data);
}

void bronco::peerconnection::process_message(const protocol::Data &data)
{
        std::cout << "Received data from generation: " << data.generation() << std::endl;

        /* Send stop */
        protocol::Stop stop;
        stop.set_peer_hash("connecting peer");
        write_message(stop);
}

void bronco::peerconnection::process_message(const protocol::Stop &stop)
{
        std::cout << "Received stop from: " << stop.peer_hash() << std::endl;

        /* Send leave packet */
        protocol::Leave leave;
        leave.set_peer_hash("accepting peer");
        write_message(leave);
}

void bronco::peerconnection::process_message(const protocol::Leave &leave)
{
        std::cout << "Received leave from: " << leave.peer_hash() << std::endl;

        /* Close socket */
        this->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        this->socket().close();
}
