/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       main.h
|   Project:    Client
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_CLIENT_MAIN_H
#define HTCPCP_CLIENT_MAIN_H

int process_uri(char*, int*, char*, char*, int*);

int get_hostname(char*);
int get_port(int*);
int get_method(char*);
int get_message(char*);
int get_additions(char*);
int get_pot_number();

int build_packet(char*);
int add_additions(char*);
int choose_milk(char*);
int choose_sweetener(char*);
int choose_syrup(char*);
int choose_spice(char*);
int choose_alcohol(char*);
char choose_amount(char, char);

#endif /* HTCPCP_CLIENT_MAIN_H */