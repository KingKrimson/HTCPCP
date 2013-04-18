/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       client_linux.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_CLIENT_CLIENT_LINUX_H
#define HTCPCP_CLIENT_CLIENT_LINUX_H

int connect_to_server(char*, int);
int disconnect_from_server(int);
int send_message(int, char*);
int receive_message(int, char*, int);

#endif /* HTCPCP_CLIENT_CLIENT_LINUX_H */