/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       main.c
|   Project:    Server
|   Created:    2013-03-18
|
|   The entry point to the HTCPCP server. It takes a portnumber to bind to as
|   an optional parameter. The default portnumber is 8000. Platform specific
|   server functions are contained in server_linux.c and server_win.c
|
\=============================================================================*/

#include "include/pch.h"
#include "include/main.h"

#if defined(_WIN32)
#include "include/server_win.h"
#elif defined(__linux__)
#include "include/server_linux.h"
#endif

#define DEFAULTPORT 8000
#ifndef MAXINT16
#define MAXINT16 65535
#endif

int get_port(int *port);

int main(int argc, char **argv)
{
    uwe_socket socket;
    int port = 0;
    int i;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv[++i]);
            continue;
        } else {
            printf("usage: %s [-p \"port\"]\n", argv[0]);
            exit(1);
        }
    }

    if (port == 0) {
        get_port(&port);
    }

    socket = init_server(port);
    run_server(socket);
    kill_server(socket);
    return 0;
}

int get_port(int *port)
{
    char port_string[6];

    do {
        printf("Please type a port (0 to use default, %i):\n", DEFAULTPORT);
        printf("> ");
        scanf("%5s", &port_string);
        printf("\n");
        port_string[5] = '\0';

        if (port_string[0] == '0') {
            *port = DEFAULTPORT;
            break;
        }
        *port = atoi(port_string);
    } while (*port < 0 || *port > MAXINT16);

    return 0;
}