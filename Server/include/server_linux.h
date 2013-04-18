/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       server_linux.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_SERVER_LINUX_H
#define HTCPCP_SERVER_SERVER_LINUX_H

void sigchld_handler(int);
int init_server(int);
int kill_server(int);
int run_server(int);
void coffee_handler(int);

#endif /* HTCPCP_SERVER_SERVER_LINUX_H */