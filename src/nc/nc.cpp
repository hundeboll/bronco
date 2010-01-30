/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "peermanager.hpp"

bronco::peermanager *manager_ptr;

void signal_close(int not_used)
{
    manager_ptr->close();
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

    uint16_t port;
    if (argc > 1) {
        std::istringstream p(argv[1]);
        p >> port;
    } else {
        port = bronco::peermanager::select_port();
    }

    /* Create peer manager */
    manager_ptr = new bronco::peermanager(port, &myprintf);

    /* Catch signals */
    signal(SIGINT, signal_close);
    signal(SIGTERM, signal_close);

    /* Start io_service */
    manager_ptr->run();

    std::cout << "Bye bye" << std::endl;

    return 0;
}
