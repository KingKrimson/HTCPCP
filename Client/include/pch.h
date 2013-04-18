/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       pch.h
|   Project:    Client
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_CLIENT_PCH_H
#define HTCPCP_CLIENT_PCH_H

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET coffee_socket;
#elif defined(__linux__)
typedef int coffee_socket;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#endif /* HTCPCP_CLIENT_PCH_H */