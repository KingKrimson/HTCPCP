/*=============================================================================\
|
|   Copyright (C) 2013 by Christopher Harpum & Andrew Brown.
|
|   File:       main.c
|   Project:    Client
|   Created:    2013-03-18
|   Description:
|
|   main.c is the general purpose client implementation. The platform specific
|   features, such as connecting, sending and receiving messages, and closing
|   the connection are in client_linux.c and client_win.c
|
|   The client accepts a method and uri as individual parameters. If the uri is
|   entered, then it is processed, and a packet is build, once a method has
|   been chosen. If it's not entered, then the user can use a menu based system
|   to build a packet. The method can be entered at the command line, or chosen
|   in the menu.
|
|   Once the packet has been assembled, sent, recieved and displayed, the
|   client quits.
|
\=============================================================================*/

#include "include/pch.h"
#include "include/main.h"

#if defined(_WIN32)
#include "include/client_win.h"
#elif defined(__linux__)
#include "include/client_linux.h"
#endif

#define DEFAULTHOSTNAME "127.0.0.1"
#define DEFAULTPORT 8000
#define MAXHOSTNAMESIZE 15
#define MAXPACKETSIZE 512
#define MAXMETHODSIZE 8
#ifndef MAXINT16
#define MAXINT16 65535
#endif

/*******************************************************************************
* The entry point to client. Builds a request packet, and then sends it. The
* way the packet is built depends on whether the URI and method have been
* specified in the command line arguments.
*******************************************************************************/
int main(int argc, char **argv) {
    int num_bytes;
    coffee_socket sockfd;
    int port = 0;
    char hostname[MAXHOSTNAMESIZE + 1] = {0};
    char method[MAXMETHODSIZE + 1] = {0};
    int pot_number = 0;
    char additions[400] = {0};
    char message[80];
    char packet[MAXPACKETSIZE + 1] = {0};
    int i;

    /*check the arguments*/
    printf("\n");
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-f") == 0) {
            ++i;
            continue;
        } else if (strcmp(argv[i], "-u") == 0) {
            process_uri(argv[++i], &pot_number, additions, hostname, &port);
            continue;
        } else if (strcmp(argv[i], "-m") == 0) {
            strcpy(method, argv[++i]);
            continue;
        } else {
            printf("usage: %s [-f \"file\"] | [[-m \"method\"] [- u \"URI\"]]",
                argv[0]);
            exit(1);
        }
    }

    if (method[0] == '\0') {
        get_method(method);
    }

    /*allows the user to choose the message for a BREW/POST request*/
    if (strcmp(method, "BREW") == 0 || strcmp(method, "POST") == 0) {
        get_message(message);
    } else {
        strcpy(message, "Content-length:0\r\n\r\n");
    }

    if (strstr(message, "start") && additions[0] == '\0') {
        get_additions(additions);
    }

    if (pot_number == 0) {
        pot_number = get_pot_number();
    }

    if (hostname[0] == '\0') {
        get_hostname(hostname);
    }

    if (port == 0) {
        get_port(&port);
    }

    /*build the packet from the indivdual elements built up to now*/
    sprintf(packet, "%s pot-%d HTCPCP/1.0\r\n%s%s", method, pot_number,
        additions, message);

    sockfd = connect_to_server(hostname, port);

    /*send the packet*/
    if (num_bytes = send_message(sockfd, packet) == 0) {
        printf("Failed to send message %s", packet);
    }

    /*wait for response*/
    do {
        if ((num_bytes = receive_message(sockfd, packet, MAXPACKETSIZE))) {
            packet[num_bytes] = '\0';
            printf("received: %s\n", packet);
        }
    } while (packet[0] == '\0');

    disconnect_from_server(sockfd);
    return 0;
}

/*******************************************************************************
* Processes the uri, if one is given. Finds the hostname, port, pot and
* additions, and saves them for use in craeting the packet.
*******************************************************************************/
int process_uri(char *uri, int *pot_number, char *additions, char *hostname,
                int *port)
{
    static const char *coffee_scheme_list[] = {
        "test", "koffie", "q%C3%A6hv%C3%A6", "%D9%82%D9%87%D9%88%D8%A9",
        "kohv", "kafo", "kafe", "kava", "caf%C3%E8", "%CE%BA%CE%B1%CF%86%CE%AD",
        "k%C3%A1va", "kahva", "kaffe", "%4Baffee", "coffee", "akeita", "kahvi",
        "koffee", "E5%92%96%E5%95%A1", "%E0%A4%95%E0%A5%8C%E0%A4%AB%E0%A5%80",
        "%E3%82%B3%E3%83%BC%E3%83%92%E3%83%BC", "%D0%BA%D0%BE%D1%84%D0%B5",
        "%EC%BB%A4%ED%94%BC", "%E0%B8%81%E0%B8%B2%E0%B9%81%E0%B8%9F", "\0"
    };

    char *coffee_scheme;
    char *host;
    char *port_string;
    char *addition;
    int i;

    coffee_scheme = strtok(uri, ":");
    for (i = 0; i < (sizeof(coffee_scheme_list) / sizeof(char*)); ++i) {
        if (strcmp(coffee_scheme, coffee_scheme_list[i]) == 0) {
            break;
        }
        if (strcmp(coffee_scheme_list[i], "\0") == 0) {
            return -1;
        }
    }

    host = strtok(NULL, "/");
    *pot_number = (strtok(NULL, "?")[4] - '0');

    while (addition = strtok(NULL, "#&")) {
        strcat(additions, "Accept-Additions:#");
        strcat(additions, addition);
        strcat(additions, "\r\n");
    }

    strcpy(hostname, strtok(host, ":"));
    *port = (port_string = strtok(NULL, "")) ? atoi(port_string) : DEFAULTPORT;
    return 0;
}

/*******************************************************************************
* a menu based way to choose a hostname.
*******************************************************************************/
int get_hostname(char *hostname)
{
    printf("Please type a hostname (0 to use default, %s):\n", DEFAULTHOSTNAME);
    printf("> ");
    scanf("%15s", hostname);
    printf("\n");
    if (strcmp(hostname, "0") == 0) {
        strcpy(hostname, DEFAULTHOSTNAME);
    }

    return 0;
}

/*******************************************************************************
* a menu based way to choose a port.
*******************************************************************************/
int get_port(int *port)
{
    char port_string[6];

    do {
        printf("Please type a port (0 to use default, %i):\n", DEFAULTPORT);
        printf("> ");
        scanf("%5s", &port_string);
        printf("\n");
        port_string[5] = '\0';

        if (port_string[0] == '0') {
            *port = DEFAULTPORT;
            break;
        }
        *port = atoi(port_string);
    } while (*port < 0 || *port > MAXINT16);

    return 0;
}

/*******************************************************************************
* a menu based way to choose a method.
*******************************************************************************/
int get_method(char *method)
{
    char choice[2];

    do {
        printf("Please choose your desired METHOD:\n");
        printf("A: BREW\n");
        printf("B: PROPFIND\n");
        printf("C: WHEN\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");
    } while (choice[0] < 'A' || choice[0] > 'C');

    switch (choice[0]) {
    case 'A':
        strcpy(method, "BREW");
        break;
    case 'B':
        strcpy(method, "PROPFIND");
        break;
    case 'C':
        strcpy(method, "WHEN");
        break;
    }

    return 0;
}

/*******************************************************************************
* a menu based way to choose a potnumber.
*******************************************************************************/
int get_pot_number()
{
    char pot_number[2];

    do {
        printf("Please enter the pot number you wish to access (1 - 9):\n");
        printf("> ");
        scanf("%1s", pot_number);
        while (getchar() != '\n');
        printf("\n");
    } while (pot_number[0] < '1' || pot_number[0] > '9');

    return atoi(pot_number);
}

/*******************************************************************************
* If BREW has been chosen, choose which message body to send to the server.
*******************************************************************************/
int get_message(char *message) {
    char choice[2];

    do {
        printf("Do you want to start or stop brewing?:\n");
        printf("A: start.\n");
        printf("B: stop.\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");
    } while (choice[0] < 'A' || choice[0] > 'B');

    strcpy(message, "Content-Type:message/coffeepot\r\n");
    switch (choice[0]) {
    case 'A':
        strcat(message, "Content-Length:5\r\n\r\nstart");
        break;
    case 'B':
        strcat(message, "Content-Length:4\r\n\r\nstop");
        break;
    }

    return 0;
}

/*******************************************************************************
* a series of menu based functions that allow the user to choose additions.
*******************************************************************************/
int get_additions(char *additions) {
    char choice[2];

    for (; ; ) {
        do {
            printf("Please choose additions:\n");
            printf("A: Milk.\n");
            printf("B: Sweetener.\n");
            printf("C: Syrup.\n");
            printf("D: Spice.\n");
            printf("E: Alcohol.\n");
            printf("X: No more additions.\n");
            printf("> ");
            scanf("%1s", choice);
            while (getchar() != '\n');
            choice[0] = toupper(choice[0]);
            printf("\n");

            if (choice[0] == 'X') {
                return 0;
            }
        } while (choice[0] < 'A' || choice[0] > 'E');

        switch (choice[0]) {
        case 'A':
            choose_milk(additions);
            break;
        case 'B':
            choose_sweetener(additions);
            break;
        case 'C':
            choose_syrup(additions);
            break;
        case 'D':
            choose_spice(additions);
            break;
        case 'E':
            choose_alcohol(additions);
            break;
        }
    }
}

int choose_milk(char *additions) {
    char choice[2];
    char amount;

    do {
        printf("Please choose milk:\n");
        printf("A: Cream.\n");
        printf("B: Half-and-half.\n");
        printf("C: Whole-milk.\n");
        printf("D: Part-skim.\n");
        printf("E: Skim.\n");
        printf("F: Non-dairy.\n");
        printf("X: Cancel.\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");

        if (choice[0] == 'X') {
            return 0;
        }
    } while (choice[0] < 'A' || choice[0] > 'F');

    amount = choose_amount('1', '5');

    strcat(additions, "Accept-Additions:#");
    switch (choice[0]) {
    case 'A':
        sprintf(additions + strlen(additions), "Cream;%c\r\n", amount);
        break;
    case 'B':
        sprintf(additions + strlen(additions), "Half-and-half;%c\r\n", amount);
        break;
    case 'C':
        sprintf(additions + strlen(additions), "Whole-milk;%c\r\n", amount);
        break;
    case 'D':
        sprintf(additions + strlen(additions), "Part-skim;%c\r\n", amount);
        break;
    case 'E':
        sprintf(additions + strlen(additions), "Skim;%c\r\n", amount);
        break;
    case 'F':
        sprintf(additions + strlen(additions), "Non-dairy;%c\r\n", amount);
        break;
    }

    return 0;
}

int choose_sweetener(char *additions) {
    char choice[2];
    char amount;

    do {
        printf("Please choose sweetener:\n");
        printf("A: White-sugar.\n");
        printf("B: Sweetener.\n");
        printf("X: Cancel.\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");

        if (choice[0] == 'X') {
            return 0;
        }
    } while (choice[0] < 'A' || choice[0] > 'B');

    amount = choose_amount('1', '5');

    strcat(additions, "Accept-Additions:#");
    switch (choice[0]) {
    case 'A':
        sprintf(additions + strlen(additions), "White-sugar;%c\r\n", amount);
        break;
    case 'B':
        sprintf(additions + strlen(additions), "Sweetener;%c\r\n", amount);
        break;
    }

    return 0;
}

int choose_syrup(char *additions) {
    char choice[2];
    char amount;

    do {
        printf("Please choose syrup:\n");
        printf("A: Vanilla.\n");
        printf("B: Almond.\n");
        printf("C: Raspberry.\n");
        printf("X: Cancel.\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");

        if (choice[0] == 'X') {
            return 0;
        }
    } while (choice[0] < 'A' || choice[0] > 'C');

    amount = choose_amount('1', '5');

    strcat(additions, "Accept-Additions:#");
    switch (choice[0]) {
    case 'A':
        sprintf(additions + strlen(additions), "Vanilla;%c\r\n", amount);
        break;
    case 'B':
        sprintf(additions + strlen(additions), "Almond;%c\r\n", amount);
        break;
    case 'C':
        sprintf(additions + strlen(additions), "Raspberry;%c\r\n", amount);
        break;
    }
    return 0;
}

int choose_spice(char *additions) {
    char choice[2];
    char amount;

    do {
        printf("Please choose spice:\n");
        printf("A: Cinnamon.\n");
        printf("B: Cardamon.\n");
        printf("X: Cancel.\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");

        if (choice[0] == 'X') {
            return 0;
        }
    } while (choice[0] < 'A' || choice[0] > 'B');

    amount = choose_amount('1', '5');

    strcat(additions, "Accept-Additions:#");
    switch (choice[0]) {
    case 'A':
        sprintf(additions + strlen(additions), "Cinnamon;%c\r\n", amount);
        break;
    case 'B':
        sprintf(additions + strlen(additions), "Cardamon;%c\r\n", amount);
        break;
    }
    return 0;
}

int choose_alcohol(char *additions) {
    char choice[2];
    char amount;

    do {
        printf("Please choose alcohol:\n");
        printf("A: Brandy.\n");
        printf("B: Rum.\n");
        printf("C: Whiskey.\n");
        printf("X: Cancel\n");
        printf("> ");
        scanf("%1s", choice);
        while (getchar() != '\n');
        choice[0] = toupper(choice[0]);
        printf("\n");

        if (choice[0] == 'X') {
            return 0;
        }
    } while (choice[0] < 'A' || choice[0] > 'C');

    amount = choose_amount('1', '5');

    strcat(additions, "Accept-Additions:#");
    switch (choice[0]) {
    case 'A':
        sprintf(additions + strlen(additions), "Brandy;%c\r\n", amount);
        break;
    case 'B':
        sprintf(additions + strlen(additions), "Rum;%c\r\n", amount);
        break;
    case 'C':
        sprintf(additions + strlen(additions), "Whiskey;%c\r\n", amount);
        break;
    }
    return 0;
}

/*******************************************************************************
* a menu based way to addition amount.
*******************************************************************************/
char choose_amount(char min, char max) {
    char amount[2];

    do {
        printf("Please choose amount, from %c to %c.\n", min, max);
        printf("> ");
        scanf("%1s", amount);
        while (getchar() != '\n');
        printf("\n");
    } while (amount[0] < min || amount[0] > max);

    return amount[0];
}