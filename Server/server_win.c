/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       server_win.c
|   Project:    Server
|   Created:    2013-03-18
|
|   server_win.c is a platform specific implmentation of the HTCPCP server.
|   It binds to a port, and when it recieves a connection, spawns a child that
|   deals with that client. The parent continues to listen for connections.
|
\=============================================================================*/

#include "include/pch.h"
#include "include/server_win.h"
#include "include/packet_handler.h"

DWORD WINAPI child(LPVOID);

/*******************************************************************************
* Initilises the server on the port specified by port.
*******************************************************************************/
SOCKET init_server(const int port)
{
    WSADATA wsa_data;
    SOCKADDR_IN server_info;
    SOCKET server_socket;

    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    server_socket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

    ZeroMemory(&server_info, sizeof(SOCKADDR_IN));
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = INADDR_ANY;
    server_info.sin_port = htons(port);

    bind(server_socket, (SOCKADDR*) (&server_info), sizeof(server_info));
    listen(server_socket, 1);

    InitializeCriticalSection(&critical_section);
    return server_socket;
}

/*******************************************************************************
* Runs the server. listens for connections, and spawns children to deal with
* clients.
*******************************************************************************/
int run_server(SOCKET socket)
{
#if defined(_DEBUG)
    char packet[512];
    char response[512];
    int num_bytes;
    FILE *test_file;

    test_file = fopen("test_packet.txt", "rb");
    num_bytes = fread(packet, sizeof(char), 512, test_file);
    packet[num_bytes] = '\0';
    printf(packet);
    process_packet(response, packet);
    printf(response);
    fclose(test_file);
#else
    SOCKET connection = SOCKET_ERROR;
    HANDLE thread;

    printf("Waiting for incoming connections...\n");
    for (; ; )
    {
        connection = accept(socket, NULL, NULL);
        if (connection == SOCKET_ERROR) {
            continue;
        }

        thread = CreateThread(NULL, 0, child, (LPVOID) connection, 0, NULL);
        CloseHandle(thread);
    }
#endif
    return 0;
}

/*******************************************************************************
* closes the server. Closes the socket the server was bound to.
*******************************************************************************/
int kill_server(SOCKET socket)
{
    shutdown(socket, SD_SEND);
    closesocket(socket);
    WSACleanup();
    return 0;
}

/*******************************************************************************
* The child receives the clients packet, sends it to be processed, and then
* sends the returned reponse packet to the client. It then dies.
*******************************************************************************/
DWORD WINAPI child(LPVOID lParam) {
    int num_bytes;
    char packet[512];
    char response[512];

    if ((num_bytes = recv((SOCKET) lParam, packet, 511, 0)) == -1) {
        printf("unable to read packet\n");
        return 0;
    }
    packet[num_bytes] = '\0';

    process_packet(response, packet);
    send((SOCKET) lParam, response, strlen(response), 0);
    return 0;
}