/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       coffeepot.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_COFFEEPOT_H
#define HTCPCP_SERVER_COFFEEPOT_H

typedef struct {
    int brewing;
    const int teapot;
    const char *const additions;
} coffeepot;

const int is_brewing(const int);
const int is_teapot(const int);
const char *const get_additions(const int);

const int set_brewing(const int, const int);

#endif /* HTCPCP_SERVER_COFFEEPOT_H */