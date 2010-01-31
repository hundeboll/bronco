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

bronco::peermanager::peermanager(uint16_t port, print_ptr f)
    : print(f),
    port_(port),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
    stop_(false)
{
    print("Starting BRONCO\n");

    parser parsed("bronco://127.0.0.1/File_Hash_No_Two");
    print("Scheme: %s\n", parsed.scheme().c_str());
    print("Host: %s\n", parsed.host().c_str());
    print("Port: %s\n", parsed.port().c_str());
    print("Content id: %s\n", parsed.content_id().c_str());

    /* Setup configuration */
    std::string sport(utils::to_string(port_));
    me_.set_in_conn_max(5);
    me_.set_out_conn_max(3);
    me_.set_spare_peers(2);
    me_.set_peer_hash(utils::to_string(port_));
    me_.set_content_id("File_Hash_No_Two");
    me_.set_port(sport);

    /* Open port for listening */
    listen();

    /* Collect garbage */
    boost::thread t(boost::bind(&peermanager::updater, this));

    /* TEST: Join network */
    std::string server_address("127.0.0.1");
    std::string server_port("60100");
    protocol::Announce announce;
    announce.set_file_hash("File_Hash_No_Two");
    announce.set_peer_hash(sport);
    announce.set_peer_address("127.0.0.1");
    announce.set_peer_port(sport);

    /* Resolve server host */
    server_conn_.reset(new serverconnection(io_, this));
    srv_endpoint_ = resolve_host(server_address, server_port);

    if (port == 60200)
        announce_server(announce);
    else
        connect_server();
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
    print("Listening on port %d\n", port_);

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
    server_conn_->socket().async_connect(*srv_endpoint_,
            boost::bind(&serverconnection::leave, server_conn_, boost::asio::placeholders::error, me_.peer_hash()));
}
