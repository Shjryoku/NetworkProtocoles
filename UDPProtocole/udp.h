#ifndef UDP_H
#define UDP_H
#include "../HttpsProtocole/main_header.h"

extern int udp_server_socket;
extern int udp_client_socket;

extern struct sockaddr_in udp_server_addr;
extern struct sockaddr_in udp_client_addr;

extern socklen_t udp_client_len;
extern socklen_t udp_server_len;

int udp_server_init();
int udp_client_init();
#endif