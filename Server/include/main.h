/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       main.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_MAIN_H
#define HTCPCP_SERVER_MAIN_H

#if defined(_WIN32)
typedef SOCKET uwe_socket;
#elif defined(__linux__)
typedef int uwe_socket;
#endif

#endif /* HTCPCP_SERVER_MAIN_H */