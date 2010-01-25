/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>

#include <iostream>

#include "peerconnection.hpp"

void bronco::peerconnection::handle_peer()
{
    read_type();
}

void bronco::peerconnection::handle_read()
{
    std::cout << "Message read!" << std::endl;
}

void bronco::peerconnection::handle_connect(const boost::system::error_code &error)
{
    std::cout << "Writing message" << std::endl;
    write_message();
}
