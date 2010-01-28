/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "connection.hpp"

void bronco::connection::read_message(const boost::system::error_code &error)
{
    if (!error) {
        /* Save length and type in decimal */
        length_ = convert_header(hex_length_);
        type_ = convert_header(hex_type_);

        /* Read message */
        boost::asio::async_read(socket_,
                in_message_.prepare(length_),
                boost::bind(&connection::handle_read, this, boost::asio::placeholders::error, type_));
    } else {
        throw std::runtime_error(error.message());
    }
}

void bronco::connection::make_header(protocol::packettype type, size_t size)
{
    /* Reset header */
    out_header_.seekp(std::iostream::beg);

    /* Set packet type */
    out_header_ << std::setw(type_header_size) << type;

    /* Set stream field width to header_length and input data in hex */
    out_header_ << std::setw(length_header_size) << std::hex << size;
}

void bronco::connection::read_type()
{
    /* Read type_header_size bytes into type_ and call read_length */
    boost::asio::async_read(socket_,
            boost::asio::buffer(hex_type_, type_header_size),
            boost::bind(&connection::read_length, this, boost::asio::placeholders::error));
}

void bronco::connection::read_length(const boost::system::error_code &error)
{
    if (!error) {
        /* Read length_header_size into hex_length_ and call read_message */
        boost::asio::async_read(socket_,
                boost::asio::buffer(hex_length_, length_header_size),
                boost::bind(&connection::read_message, this, boost::asio::placeholders::error));
    } else {
        throw std::runtime_error(error.message());
    }
}

size_t bronco::connection::convert_header(std::vector<char> &header)
{
    size_t t;

    /* Create temporary stream */
    std::istringstream is;
    is.rdbuf()->pubsetbuf(header.data(), header.size());

    /* Read content from stream */
    is >> std::hex >> t;

    return t;
}
