/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include "peermanager.hpp"
#include "utils.hpp"

/* Initialize statics */
boost::mutex bronco::peermanager::update_mutex_;
boost::condition_variable bronco::peermanager::update_cond_;
boost::asio::io_service bronco::peermanager::io_;

bronco::peermanager::peermanager(const std::string &url, print_ptr f)
    : print(f),
    parsed_url_(url),
    port_(select_port()),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
    stop_(false)
{
    print("Bronco peer listening on port %s\n", utils::to_string(port_).c_str());

    print("Scheme: %s\n", parsed_url_.scheme().c_str());
    print("Host: %s\n", parsed_url_.host().c_str());
    print("Port: %s\n", parsed_url_.port().c_str());
    print("Content id: %s\n", parsed_url_.content_id().c_str());

    /* Setup configuration */
    me_.set_in_conn_max(5);
    me_.set_out_conn_max(3);
    me_.set_spare_peers(2);
    me_.set_port(utils::to_string(port_));
    me_.set_peer_hash(me_.port());
    me_.set_content_id(parsed_url_.content_id());

    /* Open port for listening */
    listen();

    /* Watch connections */
    boost::thread t(boost::bind(&peermanager::updater, this));

    /* Resolve server host */
    server_conn_.reset(new serverconnection(io_, this));
    srv_endpoint_ = resolve_host(parsed_url_.host(), parsed_url_.port());
}

void bronco::peermanager::connect()
{
    if (parsed_url_.scheme() == "abronco") {
        /* Prepare file for sharing and announce to server */
        announce_file(parsed_url_.content_id());
    } else {
        /* Store dontent id and join network */
        content_id_ = parsed_url_.path();
        connect_server();
    }
}

void bronco::peermanager::announce_file(const std::string &path)
{
    /* Load file */
    protocol::Announce announce;
    announce.set_file_hash(path);
    announce.set_peer_hash(me_.peer_hash());
    announce.set_peer_port(me_.port());

    /* Send announce to server */
    announce_server(announce);
}

void bronco::peermanager::updater()
{
    while (!stop_) {
        /* Wait for closed connections */
        scoped_lock lock(update_mutex_);
        update_cond_.wait(lock);

        if (stop_)
            break;

        /* Clean up */
        update_connections(in_peers_);
        update_connections(out_peers_);

        /* Connect to more if needed */
        if (out_peers_.size() < me_.out_conn_max()) {
            /* Request peers in server connection */

            /* Connect to received peers */
        }
    }
}

void bronco::peermanager::listen()
{
    /* Initiate accepting loop */
    in_conn_ = peerconnection::create(io_, this);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::peermanager::handle_incoming(const boost::system::error_code &error)
{
    /* Check for error from async_accept */
    if (!error) {
        /* Save connection to list */
        in_peers_.push_back(in_conn_);

        /* Handle control to connection */
        in_conn_->handle_peer(in_peers_.size() <= me_.in_conn_max());
    }

    /* Listen for next incoming connection */
    in_conn_ = peerconnection::create(io_, this);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::peermanager::connect_peer(const protocol::Peer &peer)
{
    print("Connecting to %s:%s\n", peer.address().c_str(), peer.port().c_str());
    endpoint_it peer_end(resolve_host(peer.address(), peer.port()));

    /* Open connection */
    out_conn_ = peerconnection::create(io_, this);
    out_conn_->socket().async_connect(*peer_end,
            boost::bind(&peerconnection::handle_connect, out_conn_, boost::asio::placeholders::error, peer));

    /* Save connection */
    out_peers_.push_back(out_conn_);
}

size_t bronco::peermanager::update_connections(std::vector<peerconnection::pointer> &peers)
{
    /* Remove objects with closed sockets */
    typedef std::vector<peerconnection::pointer>::iterator peer_it;
    for (peer_it it(peers.begin()), end(peers.end()); it != end; it++) {
        if (!(*it)->socket().is_open()) {
            std::cout << "Removing connection" << std::endl;
            peers.erase(it);
        }
    }

    /* Return new size */
    return peers.size();
}

void bronco::peermanager::connect_server()
{
    server_conn_->socket().async_connect(*srv_endpoint_,
            boost::bind(&serverconnection::handle_connect, server_conn_, boost::asio::placeholders::error, me_));
}

void bronco::peermanager::announce_server(const protocol::Announce &announce)
{
    server_conn_->socket().async_connect(*srv_endpoint_,
            boost::bind(&serverconnection::handle_announce, server_conn_, boost::asio::placeholders::error, announce));
}

void bronco::peermanager::leave_server()
{
    /* Create leave message */
    protocol::Leave leave;
    leave.set_peer_hash(me_.peer_hash());
    leave.set_content_id(content_id_);

    /* Connect to server and send leave */
    boost::system::error_code error;
    server_conn_->socket().connect(*srv_endpoint_, error);
    if (!error) {
        server_conn_->write_sync_message(leave);
        server_conn_->close_socket();
    } else if (error == boost::asio::error::connection_refused) {
        print("Connection refused by server when sending leave\n");
    } else {
        throw error;
    }
}
