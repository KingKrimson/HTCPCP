/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       packet_handler.c
|   Project:    Server
|   Created:    2013-03-18
|
|   packet_handler.c deals with the packets that the server recieves. Given a
|   request packet, it splits the packet into lines and then processes it.
|   An appropriate response packet is returned, which the server sends to the
|   client.
|
\=============================================================================*/

#include "include/pch.h"
#include "include/packet_handler.h"
#include "include/coffeepot.h"

/*******************************************************************************
* Processes the request packet, and returns the appopriate response packet.
* These response packets are built in seperate functions.
*******************************************************************************/
int process_packet(char *response, char *packet)
{
    char *method;
    int pot;
    char *coffee_message_body;
    char *line_list[24];
    char *content_length;
    char *content_type;
    int line_count = -1;
    int i;

    /* find the method, the pot, and the htcpcp type */
    method = strtok(packet, " ");

    if ((pot = valid_pot(strtok(NULL, " "))) == 0) {
        return build_not_found(response, pot);
    } else if (is_teapot(pot)) {
        return build_im_a_teapot(response);
    }

    if (strcmp(strtok(NULL, "\r"), "HTCPCP/1.0")) {
        return build_not_supported(response);
    }

    /* grab the rest of the lines. */
    do {
        ++line_count;
        line_list[line_count] = strtok(NULL, "\r\n");
    } while (line_list[line_count]);

    for (i = 0; i < line_count; ++i) {
        if (strstr(line_list[i], "Content-Length:")) {
            content_length = line_list[i];
        } else if (strstr(line_list[i], "Content-Type:")) {
            content_type = line_list[i];
            if (strcmp(content_type, "Content-Type:message/coffeepot")) {
                return build_unacceptable_media_type(response);
            }
        }
    }
    coffee_message_body = line_list[--line_count];

    /* check the packet's method, and build the appropriate response. */
    if (!strcmp(method, "POST") || !strcmp(method, "BREW")) {
        if (strstr(coffee_message_body, "start") != NULL) {
            return build_brew(response, line_list, line_count, pot);
        } else if (strstr(coffee_message_body, "stop") != NULL) {
            return build_stop(response, pot);
        } else {
            return build_bad_request(response);
        }
    } else if (!strcmp(method, "WHEN")) {
        return build_when(response);
    } else if (!strcmp(method, "PROPFIND")) {
        return build_propfind(response, pot);
    } else if (!strcmp(method, "GET")) {
        return build_method_not_allowed(response);
    } else {
        return build_bad_request(response);
    }
}

/*******************************************************************************
* Build a response to a 'brew' request.
*******************************************************************************/
int build_brew(char *response, char **line_list, int linecount, const int pot)
{
    int i;

    if (is_brewing(pot)) {
        sprintf(response, "HTCPCP/1.0 409 Conflict\r\n"
            "Content-Type:text/plain\r\n"
            "Content-Length:104\r\n\r\n"
            "Coffee is already being brewed in pot-%i. "
            "You can stop it, but I doubt the original brewer will be "
            "happy.", pot);
        return CONFLICT;
    } else {
        for (i = 0; i < linecount; ++i) {
            if (strstr(strtok(line_list[i], "#"), "Accept-Additions:")) {
                if (strstr(get_additions(pot), strtok(NULL, ";")) == NULL) {
                    return build_not_acceptable(response, pot);
                }
                if (valid_addition_parameter(strtok(NULL, "\0")) == 0) {
                    return build_bad_request(response);
                }
            }
        }
        set_brewing(pot, 1);
        sprintf(response, "HTCPCP/1.0 200 OK\r\n"
            "Safe:No\r\n"
            "Content-Type:text/plain\r\n"
            "Content-Length:36\r\n\r\n"
            "Coffee is now being brewed in pot-%i.", pot);
        return OK;
    }
}

/*******************************************************************************
* Build a response to a brew request with 'stop' as the message body.
*******************************************************************************/
int build_stop(char *response, const int pot)
{
    if (is_brewing(pot)) {
        set_brewing(pot, 0);
        sprintf(response, "HTCPCP/1.0 200 OK\r\n"
            "Content-Type:text/plain\r\n"
            "Content-Length:37\r\n\r\n"
            "pot-%i has stopped brewing coffee.", pot);
        return OK;
    } else {
        sprintf(response, "HTCPCP/1.0 409 Conflict"
            "\r\nContent-Type:text/plain\r\n"
            "Content-Length:47\r\n\r\n"
            "Coffee is not currently being brewed in pot-%i.", pot);
        return CONFLICT;
    }
}

/*******************************************************************************
* Build a response to a propfind request.
*******************************************************************************/
int build_propfind(char *response, const int pot)
{
    sprintf(response, "HTCPCP/1.0 200 OK\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:%i\r\n\r\n"
        "pot-%i contains these additions: %s",
        strlen(get_additions(pot)) + 32, pot, get_additions(pot));
    return OK;
}

/*******************************************************************************
* Build a response to a when request. As the method is in the spec, but not
* implemented in this server, it builds a 501 not implemented error response.
*******************************************************************************/
int build_when(char *response)
{
    sprintf(response, "HTCPCP/1.0 501 Not Implemented\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:117\r\n\r\n"
        "The method 'WHEN' is not implemented in this server. "
        "The amount of milk poured is specified in the BREW/POST request.");
    return NOT_IMPLEMENTED;
}

/*******************************************************************************
* Build a response to a malformed request packet.
*******************************************************************************/
int build_bad_request(char *response)
{
    sprintf(response, "HTCPCP/1.0 400 Bad Request\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:52\r\n\r\n"
        "Your packet was malformed. Please check your syntax.");
    return BAD_REQUEST;
}

/*******************************************************************************
* Build a response to a request with an invalid header method. In this case,
* the only request in the spec that is invalid is a GET request.
*******************************************************************************/
int build_method_not_allowed(char *response)
{
    sprintf(response, "HTCPCP/1.0 405 Method Not Allowed\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:200\r\n\r\n"
        "The method you requested is not supported by this coffee pot. "
        "Here is a list of accepted methods: "
        "BREW, POST (deprecated), PROPFIND and WHEN (not implemented).");
    return METHOD_NOT_ALLOWED;
}

/*******************************************************************************
* Builds a 404 not found packet. This method is called if the pot referenced
* in the URL can't be found.
*******************************************************************************/
int build_not_found(char *response, const int pot)
{
    sprintf(response, "HTCPCP/1.0 404 Not Found\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:46\r\n\r\n"
        "The requested pot (pot-%i) could not be found.", pot);
    return NOT_FOUND;
}

/*******************************************************************************
* Build a response to a brew request which has not set it's Content_Type to
* message/coffeepot.
*******************************************************************************/
int build_unacceptable_media_type(char *response)
{
    sprintf(response, "HTCPCP/1.0 415 Unsupported Media Type\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:97\r\n\r\n"
        "Content-Type is not set to message/coffeepot. "
        "Only message/coffeepot is an accepted Content Type.");
    return UNSUPPORTED_MEDIA_TYPE;
}

/*******************************************************************************
* Build a response to a brew request which has supplied unavailable additions
* in it's header fields.
*******************************************************************************/
int build_not_acceptable(char *response, const int pot)
{
    sprintf(response, "HTCPCP/1.0 406 Not Acceptable\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:%i\r\n\r\n"
        "The addition you requested is not supplied by this coffee pot. "
        "Here is a list of accepted additions for pot-%i: %s",
        strlen(get_additions(pot)) + 111, pot, get_additions(pot));
    return NOT_ACCEPTABLE;
}

/*******************************************************************************
* check to see if the pot number in the packet is valid. If so, return the pot
* number. If not, return 0.
*******************************************************************************/
int build_im_a_teapot(char *response)
{
    sprintf(response, "HTCPCP/1.0 418 Im A Teapot\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:38\r\n\r\n"
        "I'm a little teapot, short and stout.");
    return IM_A_TEAPOT;
}

/*******************************************************************************
* check to see if the pot number in the packet is valid. If so, return the pot
* number. If not, return 0.
*******************************************************************************/
int build_not_supported(char *response)
{
    sprintf(response, "HTCPCP/1.0 505 HTCPCP Version Not Supported\r\n"
        "Content-Type:text/plain\r\n"
        "Content-Length:38\r\n\r\n"
        "The version of HTCPCP is not supported. "
        "Only HTCPCP/1.0 is supported by this server.");
    return HTCPCP_VERSION_NOT_SUPPORTED;
}

/*******************************************************************************
* check to see if the pot number in the packet is valid. If so, return the pot
* number. If not, return 0.
*******************************************************************************/
int valid_pot(char *pot)
{
    static const char *pot_list[] = {
        "pot-1", "pot-2", "pot-3", "pot-4"
    };

    int i;
    for (i = 0; i < (sizeof(pot_list) / sizeof(char*) ); ++i) {
        if (strcmp(pot, pot_list[i]) == 0) {
            return i + 1;
        }
    }
    return 0;
}

/*******************************************************************************
* Checks to see if a given addition parameter is valid. If it is, return 1.
* else, return 0.
*******************************************************************************/
int valid_addition_parameter(char *parameter)
{
    static const char *parameter_list[] = {
        "1", "2", "3", "4", "5", "dash", "splash", "little", "splash", "medium",
        "lots"
    };

    int i;
    for (i = 0; i < (sizeof(parameter_list) / sizeof(char*) ); ++i) {
        if (strcmp(parameter, parameter_list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}