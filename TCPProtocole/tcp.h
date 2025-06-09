#ifndef TCP_H
#define TCP_H
#include "../HttpsProtocole/main_header.h"

extern int tcp_server_socket;
extern int tcp_client_socket;

extern struct sockaddr_in tcp_server_addr;
extern struct sockaddr_in tcp_client_addr;

extern socklen_t tcp_client_len;
extern socklen_t tcp_server_len;

inline int tcp_server_init();
inline int tcp_client_init();
#endif