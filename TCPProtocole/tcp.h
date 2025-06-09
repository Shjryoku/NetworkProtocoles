#ifndef TCP_H
#define TCP_H
#include "../HttpsProtocole/main_header.h"

static int tcp_socket;
static struct sockaddr_in tcp_server_addr;

static int tcp_socket_reader;
static struct sockaddr_in tcp_client_addr;
static socklen_t tcp_client_len;


#endif