/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <signal.h>
#include <inttypes.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include "peermanager.hpp"

bronco::peermanager *manager_ptr;
WINDOW *mw;

void cleanup_and_exit(int status)
{
    /* Finalize ncurses */
    endwin();

    /* Stop peermanager */
    if (manager_ptr != NULL)
        manager_ptr->close();
    
    exit(status);
}

void signal_close(int not_used)
{
    cleanup_and_exit(EXIT_SUCCESS);
}

int info_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = vwprintw(mw, format, ap);
    wrefresh(mw);
    va_end(ap);

    return ret;
}

int main(int argc, char **argv)
{
    /* Read arguments */
    if (argc != 2) {
        printf("Announce: %s --announce <path-to-file> bronco://<host>[:<port>]\n", argv[0]);
        printf("    Join: %s bronco://<host>[:<port>]/<content_id>\n", argv[0]);
        cleanup_and_exit(EXIT_FAILURE);
    }

    /* Catch signals */
    signal(SIGINT, signal_close);
    signal(SIGTERM, signal_close);
    signal(SIGHUP, signal_close);
    
    /* Initialize ncurses */
    mw = initscr();
    noecho();
    curs_set(0);
    halfdelay(5);
    scrollok(mw, TRUE);

    /* Create peer manager */
    struct bronco::peermanager::peer_config c = {10, 10, 5, bronco::peermanager::select_port()};
    struct bronco::peermanager::nc_parameters p = {"bin/bronco-nc", 512, 6400};

    manager_ptr = new bronco::peermanager(argv[1], &c, &p, &info_printf);

    /* Screen update loop */
    while (1) {
        int key = wgetch(mw);
    }
    
    /* Finalize ncurses */
    cleanup_and_exit(EXIT_SUCCESS);

    return 0;
}
