/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/asio.hpp>

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <iostream>
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

    bronco::peermanager manager(select_port());
    manager.set_print(&myprintf);

    if (argc > 1) {
        std::cout << "Connecting to " << argv[1] << std::endl;
        manager.connect_peer("localhost", argv[1]);
    }

    manager.run();

    std::cout << "Bye bye" << std::endl;

    return 0;
}
