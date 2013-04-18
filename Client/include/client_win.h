/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       client_win.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_CLIENT_CLIENT_WIN_H
#define HTCPCP_CLIENT_CLIENT_WIN_H

#include <WinSock2.h>

SOCKET connect_to_server(char*, int);
int disconnect_from_server(SOCKET);
int send_message(SOCKET, char*);
int receive_message(SOCKET, char*, int);

#endif /* HTCPCP_CLIENT_CLIENT_WIN_H */