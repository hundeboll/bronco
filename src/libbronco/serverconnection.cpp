/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "serverconnection.hpp"

void bronco::serverconnection::handle_announce(const boost::system::error_code &error, const protocol::Announce &announce)
{
    std::cout << "Connected to server" << std::endl;

    /* Send join request */
    write_message(announce);

    /* Wait for reply */
    read_type();
}

void bronco::serverconnection::handle_connect(const boost::system::error_code &error, const protocol::Peer &me)
{
    std::cout << "Connected to server" << std::endl;

    /* Send join request */
    write_message(me);

    /* Wait for reply */
    read_type();
}

void bronco::serverconnection::handle_read(const boost::system::error_code &error, size_t type)
{
    /* Check if read operation resulted in error */
    if (!error) {
        /* Read OK */
        process_type(type);
    } else {
        handle_error(error);
    }
}

void bronco::serverconnection::handle_write(const boost::system::error_code &error)
{
    /* Check if write operation resulted in error */
    if (error) {
        handle_error(error);
    }
}

void bronco::serverconnection::handle_error(const boost::system::error_code &error)
{
    /* Check if error was caused by closing socket */
    if (error == boost::asio::error::eof
            || error == boost::asio::error::connection_reset
            || error == boost::asio::error::bad_descriptor
            || error == boost::asio::error::broken_pipe) {
        /* Closing socket properly */
        close_socket();
        std::cout << "Connection closed: " << error.message() << std::endl;
    } else {
        throw std::runtime_error("Socket error: " + error.message());
    }
}

void bronco::serverconnection::process_type(const size_t type)
{
    protocol::Confirm confirm;
    protocol::Config config;
    protocol::Peers peers;

    /* Perform operation determined by message type */
    switch (type) {
        case protocol::confirmtype:
            deserialize(confirm);
            process_message(confirm);
            break;

        case protocol::configtype:
            deserialize(config);
            process_message(config);
            break;

        case protocol::peerstype:
            deserialize(peers);
            process_message(peers);
            break;

        default:
            throw std::runtime_error("Unknown message type received");
            break;
    }
}

void bronco::serverconnection::process_message(const protocol::Confirm &confirm)
{
    std::cout << "Received confirmation on announce" << std::endl;
}

void bronco::serverconnection::process_message(const protocol::Config &config)
{
    std::cout << "Received config" << std::endl;

}

void bronco::serverconnection::process_message(const protocol::Peers &peers)
{
    std::cout << "Received peers:" << std::endl;

    for (size_t i; i < peers.peers_size(); ++i) {
        std::cout << "  " << peers.peers(i).peer_hash() << std::endl;
    }
}
