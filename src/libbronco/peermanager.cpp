/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include "peermanager.hpp"
#include "utils.hpp"

using namespace bronco;

peermanager::peermanager(boost::asio::io_service &io)
    : port_(select_port()),
    io_(io),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
    out_conn_(peerconnection::create(io_)),
    in_conn_(peerconnection::create(io_))
{
    /* Open port for listening */
    listen();

    /* Connect to self */
    connect_peer("localhost", port_);
}

void peermanager::listen()
{
    std::cout << "Listening on port " << port_ << std::endl;

    /* Initiate accepting loop */
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void peermanager::handle_incoming(const boost::system::error_code &error)
{
    /* Check for error from async_accept */
    if (!error) {
        /* Save connection to list */
        peers_.push_back(in_conn_);
        in_conn_->handle_peer();
    }

    /* Listen for next incoming connection */
    in_conn_ = peerconnection::create(io_);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void peermanager::connect_peer(const std::string &address, const size_t port)
{
    /* Resolve IP and port to an endpoint */
    using boost::asio::ip::tcp;
    tcp::resolver resolver(io_);
    tcp::resolver::query query(address, utils::to_string(port));
    tcp::resolver::iterator endpoint_it = resolver.resolve(query);

    /* Open connection */
    in_conn_ = peerconnection::create(io_);
    in_conn_->socket().async_connect(*endpoint_it,
            boost::bind(&peerconnection::handle_connect, in_conn_, boost::asio::placeholders::error));
}
