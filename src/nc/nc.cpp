/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/asio.hpp>

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "peermanager.hpp"

/**
 * Select random port
 * \return Selected port
 */
uint16_t select_port()
{
    srand(time(0));
    return (rand() % 1024) + 49151;
}

int myprintf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);

    return ret;
}

int main(int argc, char **argv)
{
    /* Catch signals */
    signal(SIGINT, bronco::peermanager::close);
    signal(SIGTERM, bronco::peermanager::close);

    uint16_t port;
    if (argc > 1) {
        std::istringstream p(argv[1]);
        p >> port;
    } else {
        port = select_port();
    }

    bronco::peermanager manager(port);
    manager.set_print(&myprintf);

    manager.run();

    std::cout << "Bye bye" << std::endl;

    return 0;
}
