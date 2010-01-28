/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "server.hpp"

boost::asio::io_service bronco::server::io_;

bronco::server::server(std::string &address, uint16_t port)
    : address_(address),
    port_(port),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_))
{
    std::cout << "Listening on port " << port_ << std::endl;

    /* Initiate accepting loop */
    conn_ = clientconnection::create(io_, this);
    acceptor_.async_accept(conn_->socket(),
            boost::bind(&server::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::server::handle_incoming(const boost::system::error_code &error)
{
    if (!error) {
        /* Handle control to client connection */
        conn_->handle_client();
        conn_list_.push_back(conn_);
    }

    /* Listen for next incoming */
    conn_ = clientconnection::create(io_, this);
    acceptor_.async_accept(conn_->socket(),
            boost::bind(&server::handle_incoming, this, boost::asio::placeholders::error));
}
