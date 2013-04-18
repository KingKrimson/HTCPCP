/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       pch.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_PCH_H
#define HTCPCP_SERVER_PCH_H

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
CRITICAL_SECTION critical_section;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static const enum STATUS_CODES {
    CONTINUE = 100,
    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    CONFLICT = 409,
    GONE = 410,
    UNSUPPORTED_MEDIA_TYPE = 415,
    IM_A_TEAPOT = 418,
    NOT_IMPLEMENTED = 501,
    SERVICE_UNAVAILABLE = 503,
    HTCPCP_VERSION_NOT_SUPPORTED = 505
};

#endif /* HTCPCP_SERVER_PCH_H */