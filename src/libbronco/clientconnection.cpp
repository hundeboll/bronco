/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "clientconnection.hpp"
#include "server.hpp"

void bronco::clientconnection::handle_read(const boost::system::error_code &error, const size_t type)
{
    /* Check if read operation resulted in error */
    if (!error) {
        /* Read OK */
        process_type(type);
    } else {
        handle_error(error);
    }
}

void bronco::clientconnection::handle_write(const boost::system::error_code &error)
{
    /* Check if write operation resulted in error */
    if (error) {
        handle_error(error);
    }
}

void bronco::clientconnection::handle_error(const boost::system::error_code &error)
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

void bronco::clientconnection::handle_client()
{
    /* Receive message */
    read_type();
}

void bronco::clientconnection::process_type(const size_t type)
{
    protocol::Announce announce;
    protocol::Peer peer;
    protocol::Keepalive keepalive;
    protocol::Leave leave;

    /* Perform operation determined by message type */
    switch (type) {
        case protocol::announcetype:
            deserialize(announce);
            process_message(announce);
            break;

        case protocol::peertype:
            deserialize(peer);
            process_message(peer);
            break;

        case protocol::keepalivetype:
            deserialize(keepalive);
            process_message(keepalive);
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

void bronco::clientconnection::process_message(const protocol::Announce &announce)
{
    /* Create new file manager */
    peerlist_ = srv_->new_peerlist(announce);

    /* Assign peer to new list */
    protocol::Peer peer;
    peer.set_address(announce.peer_address());
    peer.set_port(announce.peer_port());
    peer.set_peer_hash(announce.peer_hash());
    peer.set_complete(true);
    peerlist_->assign(peer);

    /* Return new list hash */
    protocol::Confirm confirm;
    confirm.set_list_hash(peerlist_->list_hash());
    write_message(confirm);
}

void bronco::clientconnection::process_message(const protocol::Peer &peer)
{
    /* Fetch right file manager and return list of peers */
    peerlist_ = srv_->get_peerlist(peer.content_id());
    if (peerlist_ == NULL) {
        std::cerr << "Unknown content id" << std::endl;
    } else {
        protocol::Peers peers(peerlist_->get_peers(peer.out_conn_max() + peer.spare_peers()));
        write_message(peers);
    }

    /* Insert new peer in list */
    peerlist_->assign(peer);
}

void bronco::clientconnection::process_message(const protocol::Keepalive &keepalive)
{
    /* Update time stamp of transmitting peer */
}

void bronco::clientconnection::process_message(const protocol::Leave &leave)
{
    /* Remove peer from list */
}

