/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include "peermanager.hpp"
#include "utils.hpp"

bronco::peermanager::peermanager(boost::asio::io_service &io)
    : port_(select_port()),
    io_(io),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
    out_conn_(peerconnection::create(io_)),
    in_conn_(peerconnection::create(io_))
{
    /* Setup configuration */
    me.set_in_conn_max(5);
    me.set_port(port_);

    /* Open port for listening */
    listen();
}

void bronco::peermanager::listen()
{
    std::cout << "Listening on port " << port_ << std::endl;

    /* Initiate accepting loop */
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
        in_conn_->handle_peer(update_connections(in_peers_) < me.in_conn_max());
    }

    /* Listen for next incoming connection */
    in_conn_ = peerconnection::create(io_);
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
    out_conn_ = peerconnection::create(io_);
    out_conn_->socket().async_connect(*endpoint_it,
            boost::bind(&peerconnection::handle_connect, out_conn_, boost::asio::placeholders::error));

    /* Save connection */
    out_peers_.push_back(out_conn_);
}

void bronco::peermanager::connect_peer(const std::string &address, const size_t port) {
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
