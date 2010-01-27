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

bronco::peermanager::peermanager(uint16_t port, int (*f)(const char *format, ...))
    : bprint(f),
    port_(port),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
    stop_(false)
{
    PRINTF("Starting BRONCO\n");

    /* Setup configuration */
    me.set_in_conn_max(5);
    me.set_port(port_);

    /* Open port for listening */
    listen();

    /* Collect garbage */
    boost::thread t(boost::bind(&peermanager::updater, this));
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
        if (out_peers_.size() < me.out_conn_max()) {
            /* Request peers in server connection */

            /* Connect to received peers */
        }
    }
}

void bronco::peermanager::listen()
{
    std::cout << "Listening on port " << port_ << std::endl;

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
        in_conn_->handle_peer(in_peers_.size() <= me.in_conn_max());
    }

    /* Listen for next incoming connection */
    in_conn_ = peerconnection::create(io_, this);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::peermanager::connect_peer(const std::string &address, const std::string &port)
{
    /* Resolve IP and port to an endpoint */
    using boost::asio::ip::tcp;
    tcp::resolver resolver(io_);
    tcp::resolver::query query(address, port);
    tcp::resolver::iterator endpoint_it = resolver.resolve(query);

    /* Open connection */
    out_conn_ = peerconnection::create(io_, this);
    out_conn_->socket().async_connect(*endpoint_it,
            boost::bind(&peerconnection::handle_connect, out_conn_, boost::asio::placeholders::error));

    /* Save connection */
    out_peers_.push_back(out_conn_);
}

void bronco::peermanager::connect_peer(const std::string &address, const uint16_t port)
{
    /* Wrap port */
    connect_peer(address, utils::to_string(port));
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
