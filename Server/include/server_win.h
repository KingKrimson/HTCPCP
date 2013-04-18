/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       server_win.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_SERVER_WIN_H
#define HTCPCP_SERVER_SERVER_WIN_H

SOCKET init_server(const int);
int kill_server(SOCKET);
int run_server(SOCKET);

#endif /* HTCPCP_SERVER_SERVER_WIN_H */