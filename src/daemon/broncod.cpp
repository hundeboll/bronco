/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <string>
#include <signal.h>

#include "server.hpp"


int main(int argc, char **argv)
{
    signal(SIGTERM, bronco::server::stop);
    signal(SIGINT, bronco::server::stop);

    /* Start server */
    std::string address("127.0.0.1");
    uint16_t port(60100);

    bronco::server s(address, port);
    s.run();

    std::cout << "Closing BRONCO server" << std::endl;

    return 0;
}
