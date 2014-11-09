/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       server_linux.c
|   Project:    Server
|   Created:    2013-03-18
|
|   server_linux.c is a platform specific implmentation of the HTCPCP server.
|   It binds to a port, and when it recieves a connection, forks itself. The
|   the forked process deals with that client, while the parent continues to
|   listen for connections.
|
\=============================================================================*/

#include "include/pch.h"
#include "include/server_linux.h"
#include "include/packet_handler.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BACKLOG 10 /* max no. of pending connections in server queue */
#define MAXDATASIZE 512 /* max data to send/retreive*/

/*******************************************************************************
* Handles signals from dying children.
*******************************************************************************/
void sigchld_handler(int s)
{
    int exit;

    if ((wait(&s)) == -1) /* wait for any child to finish */
        exit = WIFEXITED(s);
    if (exit) {
        if (WEXITSTATUS(s) != 0) {
            printf("Child encountered an error and terminated normally.\n");
        } else if (WEXITSTATUS(s) == 0) {
            printf("Child terminated normally\n");
        }
    } else {
        printf("Child was killed, or otherwise terminated abnormally.\n");
    }
}

/*******************************************************************************
* Initilises the server on the port specified by portnumber.
*******************************************************************************/
int init_server(int portnumber)
{
    int listenfd; /* listening socket */
    struct sockaddr_in server_addr; /* info for my addr i.e. server */
    socklen_t sin_size; /* size of address structure */
    int yes = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server socket");
        exit(1);
    }

    /* Set Unix socket level to allow address reuse */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Server setsockopt");
        exit(1);
    }

    sin_size = sizeof(server_addr);
    memset(&server_addr, 0, sin_size); /* zero struct */
    server_addr.sin_family = AF_INET; /* host byte order ... */
    server_addr.sin_port = htons(portnumber); /* . short, network byte order */
    server_addr.sin_addr.s_addr = INADDR_ANY; /* any server IP addr */

    /*bind the socket to this machine, and the specified port number*/
    if (bind(listenfd, (struct sockaddr *)&server_addr,
        sizeof(struct sockaddr)) == -1) {
        perror("Server bind");
        exit(1);
    }

    /*begin listening.*/
    if (listen(listenfd, BACKLOG) == -1) {
        perror("Server listen");
        exit(1);
    }

    printf("Server bound to port %i and listening.\n", portnumber);

    return listenfd;
}

/*******************************************************************************
* Runs the server. listens for connections, and spawns children to deal with
* clients.
*******************************************************************************/
int run_server(int listenfd)
{
    struct sigaction sa; /* deal with signals from dying children! */
    int connfd; /* connection socket */
    struct sockaddr_in client_addr; /* client's address info */
    socklen_t sin_size; /* size of address structure */
    char clientAddr[20]; /* holds ascii dotted quad address */

    /* Signal handler stuff */
    sa.sa_handler = sigchld_handler; /* reap all dead processes */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Server sigaction");
        exit(1);
    }

    while (1) {
        /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);

        if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr,
            &sin_size)) == -1) {
            perror("Server accept");
            continue;
        }

        strcpy(clientAddr, inet_ntoa(client_addr.sin_addr));
        printf("Server: got connection from %s\n", clientAddr);

        if (!fork()) {
            /* the child process dealing with a client */
            close(listenfd); /*child does not need listener*/
            coffee_handler(connfd);
        } /* fork() */

        close(connfd); /* parent does not need the connection socket */
    } /* while(1) */
    return 0;
}

/*******************************************************************************
* Kills the server by closing listenfd.
*******************************************************************************/
int kill_server(int listenfd)
{
    close(listenfd);
    return 0;
}

/*******************************************************************************
* The fork calls this method, which receives the clients packet, sends it to
* be processed, and then sends the returned reponse packet to the client.
* It then dies.
*******************************************************************************/
void coffee_handler(int connfd)
{
    char packet[MAXDATASIZE];
    char response[MAXDATASIZE];
    int numbytes;

    packet[0] = '\0'; /* no message yet! */

    if ((numbytes = recv(connfd, packet, MAXDATASIZE - 1, 0)) == -1) {
        perror("Server recv");
        exit(1); /* error end of child */
    }

    packet[numbytes] = '\0'; /* end of string */

    printf("received:\n%s\n", packet);

    /*process the packet*/
    process_packet(response, packet);

    if (send(connfd, response, strlen(response), 0) == -1) {
        perror("Server send");
        exit(1); /* error end of child */
    }

    printf("Sent:\n%s\n", response);

    close(connfd);
    exit(0); /* end of child! */
}