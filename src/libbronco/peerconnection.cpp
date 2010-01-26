/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>

#include <iostream>

#include "peerconnection.hpp"

void bronco::peerconnection::handle_peer()
{
    read_type();
}

void bronco::peerconnection::handle_connect(const boost::system::error_code &error)
{
    out_message_.at(99) = '\0';
    write_message();
}

void bronco::peerconnection::handle_read(const boost::system::error_code &error)
{
    write_message();
}

void bronco::peerconnection::handle_write(const boost::system::error_code &error)
{
    read_type();
}
