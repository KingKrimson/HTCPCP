/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       client_linux.c
|   Project:    Client
|   Created:    2013-03-18
|
|   Client_linux.c is a system specific implmentation of a client. It can 
|   connect to a server, send messages, receive messages, and close the 
|   connection. 
|
\=============================================================================*/

#include "include/pch.h"
#include "include/client_linux.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#define MAXDATASIZE 100 /* max bytes to be received at once */


/*******************************************************************************
* Connects to the server. Takes the hostname and port number as parameters.
*******************************************************************************/
int connect_to_server(char *hostname, int port) {
    int sockfd;
    struct hostent *he;
    struct sockaddr_in their_addr; /* server address info */

    /* resolve server host name or IP address */
    if(!(he = gethostbyname(hostname))) { /* host server info */
        perror("Client gethostbyname");
        exit(1);
    }

    /*create the socket.*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Client socket");
        exit(1);
    }

    /*specify the server to connect to*/
    memset(&their_addr, 0, sizeof(their_addr)); /* zero all */
    their_addr.sin_family = AF_INET; /* host byte order .. */
    their_addr.sin_port = htons(port); /* .. short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he -> h_addr);

    /*connect to the server */
    if(connect(sockfd, (struct sockaddr *)&their_addr,
        sizeof(struct sockaddr)) == -1)
    {
        perror("Client connect");
        exit(1);
    }

    return sockfd;
}


/*******************************************************************************
* Sends a message to the server specified by sockfd.
*******************************************************************************/
int send_message(int sockfd, char *message)
{
    return send(sockfd, message, strlen(message), 0);
}

/*******************************************************************************
* Receives a message to the server specified from sockfd.
*******************************************************************************/
int receive_message(int sockfd, char* buf, int buf_size)
{
    return recv(sockfd, buf, buf_size, 0);
}

/*******************************************************************************
* Disconnects sockfd.
*******************************************************************************/
int disconnect_from_server(int sockfd)
{
    return close(sockfd);
}
