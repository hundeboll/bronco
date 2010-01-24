/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/asio.hpp>

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <iostream>
#include "peermanager.hpp"

void close_peer(int signo)
{
    /* Handle shutdown */
    std::cout << "Bye bye" << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    /* Catch signals */
    signal(SIGINT, close_peer);

    boost::asio::io_service io;
    bronco::peermanager manager(io);
    io.run();

    return 0;
}
