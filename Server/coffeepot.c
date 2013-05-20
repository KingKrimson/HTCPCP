/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       coffeepot.c
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#include "include/pch.h"
#include "include/coffeepot.h"

static coffeepot coffeepot_list[] = {
    {0, 0, "Whole-milk, Part-skim, Skim"},
    {0, 0, "Whole-milk, Part-skim, Skim, White-sugar, Sweetener"},
    {0, 0, "Whole-milk, Part-skim, Skim, Vanilla, Chocolate"},
    {0, 1, ""}
};

const int is_brewing(const int pot) {
    int val;
#if defined(_WIN32)
    EnterCriticalSection(&critical_section);
#endif
    val = coffeepot_list[pot - 1].brewing;
#if defined(_WIN32)
    LeaveCriticalSection(&critical_section);
#endif
    return val;
}

const int is_teapot(const int pot) {
    return coffeepot_list[pot - 1].teapot;
}

const char *const get_additions(const int pot) {
    return coffeepot_list[pot - 1].additions;
}

const int set_brewing(const int pot, const int val) {
#if defined(_WIN32)
    EnterCriticalSection(&critical_section);
#endif
    coffeepot_list[pot - 1].brewing = val;
#if defined(_WIN32)
    LeaveCriticalSection(&critical_section);
#endif
    return val;
}