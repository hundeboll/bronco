/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "serverconnection.hpp"
#include "peermanager.hpp"

void bronco::serverconnection::handle_announce(const boost::system::error_code &error, const protocol::Announce &announce)
{
    /* Send join request */
    write_message(announce);

    /* Wait for reply */
    read_type();
}

void bronco::serverconnection::handle_connect(const boost::system::error_code &error, const protocol::Peer &me)
{
    /* Send join request */
    write_message(me);

    /* Wait for reply */
    read_type();
}

void bronco::serverconnection::leave(const boost::system::error_code &error, const std::string &peer_hash)
{
    protocol::Leave leave;
    leave.set_peer_hash(peer_hash);
    write_message(leave);
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
    } else if (error == boost::asio::error::connection_refused) {
        manager_->print("Connection refused\n");
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
    manager_->print("Received confirm on announce\n");
    close_socket();
}

void bronco::serverconnection::process_message(const protocol::Config &config)
{
    manager_->print("Received configuration\n");
}

void bronco::serverconnection::process_message(const protocol::Peers &peers)
{
    const int32_t size(peers.peers_size());

    /* Print info */
    manager_->print("Received %d peers:\n", size);
    for (int32_t i(0); i < size; ++i)
    {
        manager_->print("  %s\n", peers.peers(i).peer_hash().c_str());
    }

    /* We are done with this server connection */
    close_socket();

    /* Connect peers */
    for (int32_t i(0); i < size; ++i)
    {
        manager_->connect_peer(peers.peers(i));
    }
}
