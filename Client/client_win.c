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
    WSADATA WsaDat;
    SOCKADDR_IN sockaddr;
    SOCKET Socket;
    struct hostent *host;
    int hResult;

    WSAStartup(MAKEWORD(2, 2), &WsaDat);
    Socket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    host = gethostbyname(hostname);

    ZeroMemory(&sockaddr, sizeof(SOCKADDR_IN));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    sockaddr.sin_port = htons(port);

    hResult = SOCKET_ERROR;
    do {
        hResult = connect(Socket, (SOCKADDR*)(&sockaddr), sizeof(sockaddr));
    } while (hResult == SOCKET_ERROR);

    return Socket;
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