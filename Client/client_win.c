/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       client_win.c
|   Project:    Client
|   Created:    2013-03-18
|
|   Windows specific way of connecting to the server, sending and receiving
|   messages, and disconnecting from the server.
|
\=============================================================================*/

#include "include/pch.h"
#include "include/client_win.h"

/*******************************************************************************
* Connects to the server specified by hostname and port.
*******************************************************************************/
SOCKET connect_to_server(char *hostname, int port)
{
    WSADATA wsa_data;
    SOCKADDR_IN socket_address;
    SOCKET client_socket;
    struct hostent *host;
    int result;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return (SOCKET) NULL;
    }
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    host = gethostbyname(hostname);

    ZeroMemory(&socket_address, sizeof(SOCKADDR_IN));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = *((unsigned long *) host->h_addr);
    socket_address.sin_port = htons((u_short) port);

    result = SOCKET_ERROR;
    do {
        result = connect(client_socket, (SOCKADDR*) (&socket_address),
            sizeof(socket_address));
    } while (result == SOCKET_ERROR);

    return client_socket;
}

/*******************************************************************************
* disconnected from the server specified by socket.
*******************************************************************************/
int disconnect_from_server(SOCKET socket)
{
    shutdown(socket, SD_SEND);
    closesocket(socket);
    WSACleanup();
    return 0;
}

/*******************************************************************************
* Sends a message to the server specified by socket.
*******************************************************************************/
int send_message(SOCKET sockfd, char *message)
{
    return send(sockfd, message, strlen(message), 0);
}

/*******************************************************************************
* Recieves a message from socket.
*******************************************************************************/
int receive_message(SOCKET sockfd, char *buf, int buf_size)
{
    return recv(sockfd, buf, buf_size, 0);
}