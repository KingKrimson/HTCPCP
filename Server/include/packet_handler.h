/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       packet_handler.h
|   Project:    Server
|   Created:    2013-03-18
|
\=============================================================================*/

#pragma once
#ifndef HTCPCP_SERVER_PACKET_HANDLER_H
#define HTCPCP_SERVER_PACKET_HANDLER_H

int process_packet(char*, char*);

int build_brew(char*, char**, int, const int);
int build_stop(char*, const int);
int build_propfind(char*, const int);
int build_when(char*);
int build_bad_request(char*);
int build_method_not_allowed(char*);
int build_not_found(char*, const int);
int build_unacceptable_media_type(char*);
int build_not_acceptable(char*, const int);
int build_im_a_teapot(char*);
int build_not_supported(char*);

int valid_pot(char*);
int valid_addition_parameter(char*);

#endif /* HTCPCP_SERVER_PACKET_HANDLER_H */