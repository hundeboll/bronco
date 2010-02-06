/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <signal.h>
#include <string>
#include <inttypes.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include "peermanager.hpp"

#define HEADER_HEIGHT 2
#define FILE_HEIGHT 3

bronco::peermanager *manager_ptr;
WINDOW *header_win, *file_win, *info_win;

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
    int ret = vwprintw(info_win, format, ap);
    wrefresh(info_win);
    va_end(ap);

    return ret;
}

void update_windows(uint32_t *maxy, uint32_t *maxx)
{
    /* Get screen dimensions */
    getmaxyx(stdscr, *maxy, *maxx);

    /* Update window dimensions */
    wresize(header_win, HEADER_HEIGHT, *maxx);
    wresize(file_win, FILE_HEIGHT, *maxx);
    wresize(info_win, *maxy-HEADER_HEIGHT-FILE_HEIGHT, *maxx);
    mvwin(header_win, 0, 0);
    mvwin(file_win, HEADER_HEIGHT, 0);
    mvwin(info_win, HEADER_HEIGHT+FILE_HEIGHT, 0);

    /* Refresh windows */
    wrefresh(header_win);
    wrefresh(file_win);
    wrefresh(info_win);
}

int main(int argc, char **argv)
{
    std::string header = "*** BRONCO v0.1 ***";

    /* Read arguments */
    if (argc != 2) {
        printf("Announce: %s --announce <path-to-file> bronco://<host>[:<port>]\n", argv[0]);
        printf("    Join: %s bronco://<host>[:<port>]/<content_id>\n", argv[0]);
        cleanup_and_exit(EXIT_FAILURE);
    }

    /* Set signal handlers */
    signal(SIGINT, signal_close);
    signal(SIGTERM, signal_close);
    signal(SIGHUP, signal_close);
    
    /* Initialize ncurses */
    initscr();

    /* Get screen dimensions */
    uint32_t maxy = 0;
    uint32_t maxx = 0;
    getmaxyx(stdscr, maxy, maxx);

    /* Create windows */
    header_win = newwin(HEADER_HEIGHT, maxx, 0, 0);
    file_win = newwin(FILE_HEIGHT, maxx, HEADER_HEIGHT, 0);
    info_win = newwin(maxy-HEADER_HEIGHT-FILE_HEIGHT, maxx, HEADER_HEIGHT+FILE_HEIGHT, 0);

    /* Set ncurses options */
    noecho();
    curs_set(0);
    halfdelay(5);
    keypad(stdscr, TRUE);
    scrollok(info_win, TRUE);

    /* Create peer manager */
    struct bronco::peermanager::peer_config c = {10, 10, 5, bronco::peermanager::select_port()};
    struct bronco::peermanager::nc_parameters p = {"bin/bronco-nc", 512, 6400};

    manager_ptr = new bronco::peermanager(argv[1], &c, &p, &info_printf);

    /* Screen update loop */
    while (1) {
        /* Write content */
        wclear(header_win);
        wclear(file_win);
        mvwprintw(header_win, 0, (maxx - header.size())/2, header.c_str());
        mvwprintw(file_win, 0, 0, " Anders_Bech_-_Store_patter_betyder_undskyld.mp3\n [#####      ]");

        /* Resize and refresh windows */
        update_windows(&maxy, &maxx);

        /* Wait for keypress or timeout */
        int key = wgetch(info_win);
        if (key != ERR)
            info_printf("Key pressed!\n");
    }
    
    /* Finalize */
    cleanup_and_exit(EXIT_SUCCESS);

    return 0;
}
