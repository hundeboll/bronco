/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <iostream>
#include "arbitrator.hpp"

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

    while(1);

    return 0;
}
