/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/asio.hpp>

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <iostream>
#include "peermanager.hpp"

boost::asio::io_service io;

void close_peer(int signo)
{
    /* Handle shutdown */
    io.stop();
}

int main(int argc, char **argv)
{
    /* Catch signals */
    signal(SIGINT, close_peer);

    bronco::peermanager manager(io);
    io.run();

    std::cout << "Bye bye" << std::endl;

    return 0;
}
