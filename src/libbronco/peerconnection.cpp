/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <iostream>

#include "peerconnection.hpp"
#include "peermanager.hpp"

void bronco::peerconnection::handle_peer(const bool accept)
{
    /* Obey peer manager */
    accept_ = accept;

    /* Start reading loop */
    read_type();
}

void bronco::peerconnection::handle_connect(const boost::system::error_code &error, const protocol::Peer &remote_peer)
{
    if (!error) {
        /* Save info about connected peer */
        remote_peer_ = remote_peer;

        /* Handshake */
        protocol::Connect connect;
        connect.set_peer_hash(manager_->me().peer_hash());
        write_message(connect);

        /* Wait for reply */
        read_type();
    } else if (error == boost::asio::error::connection_refused) {
        manager_->print("Connection to %s refused\n", remote_peer.peer_hash().c_str());
        manager_->update_connections();
    } else {
        handle_error(error);
    }
}

void bronco::peerconnection::handle_write(const boost::system::error_code &error)
{
    /* Check if write operation resulted in error */
    if (error) {
        handle_error(error);
    }
}

void bronco::peerconnection::handle_error(const boost::system::error_code &error)
{
    /* Check if error was caused by closing socket */
    if (error == boost::asio::error::eof
            || error == boost::asio::error::connection_reset
            || error == boost::asio::error::bad_descriptor
            || error == boost::asio::error::broken_pipe) {
        /* Closing socket properly */
        close_socket();
        manager_->update_connections();
    } else {
        throw std::runtime_error("Socket error: " + error.message());
    }
}

void bronco::peerconnection::handle_read(const boost::system::error_code &error, const size_t type)
{
    /* Check if read operation resulted in error */
    if (!error) {
        /* Read OK */
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

    /* Perform operation determined by message type */
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
        if (accept_) {
            manager_->print("Accepting %s\n", connect.peer_hash().c_str());
            remote_peer_.set_peer_hash(connect.peer_hash());
            connected_ = true;
        } else {
            manager_->print("Rejecting %s\n", connect.peer_hash().c_str());
        }

        protocol::Reply reply;
        reply.set_busy(!accept_);
        reply.set_peer_hash(manager_->me().peer_hash());
        write_message(reply);
}

void bronco::peerconnection::process_message(const protocol::Reply &reply)
{
        if (reply.busy()) {
            /* Rejected, closing connection */
            manager_->print("Rejected by %s\n", reply.peer_hash().c_str());
            close_socket();
            manager_->update_connections();
        } else {
            /* Accepted, proceeding */
            manager_->print("Accepted by %s\n", reply.peer_hash().c_str());
            connected_ = true;

            /* Send start */
            protocol::Start start;
            start.set_peer_hash(manager_->me().peer_hash());
            write_message(start);
        }
}

void bronco::peerconnection::process_message(const protocol::Start &start)
{
    manager_->print("Received start from %s\n", start.peer_hash().c_str());

    /* Send data packet */
    protocol::Data data;
    data.set_generation(1);
    data.set_packet(std::string(6400, '\0'));
    write_message(data);
}

void bronco::peerconnection::process_message(const protocol::Data &data)
{
    /* Send stop */
    write_message(data);
}

void bronco::peerconnection::process_message(const protocol::Stop &stop)
{
    manager_->print("Received stop from %s\n", stop.peer_hash().c_str());

    /* Send leave packet */
    protocol::Leave leave;
    leave.set_peer_hash(manager_->me().peer_hash());
    write_message(leave);
}

void bronco::peerconnection::process_message(const protocol::Leave &leave)
{
    manager_->print("Received leave from %s\n", leave.peer_hash().c_str());

    /* Close socket */
    close_socket();
    manager_->update_connections();
}
