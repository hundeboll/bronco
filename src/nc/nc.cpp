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
    /* Read arguments */
    if (argc == 2) {
        /* Create peer manager */
        manager_ptr = new bronco::peermanager(argv[1], &myprintf);
        manager_ptr->connect();
    } else if (argc == 4) {
        manager_ptr = new bronco::peermanager(argv[3], &myprintf);
        manager_ptr->announce_file(argv[2]);
    } else {
        myprintf("Announce: %s --announce <path-to-file> bronco://<host>[:<port>]\n", argv[0]);
        myprintf("    Join: %s bronco://<host>[:<port>]/<content_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Catch signals */
    signal(SIGINT, signal_close);
    signal(SIGTERM, signal_close);

    /* Start io_service */
    manager_ptr->run();

    std::cout << "Bye bye" << std::endl;

    return 0;
}
