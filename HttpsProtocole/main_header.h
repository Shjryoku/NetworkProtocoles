#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../TCPProtocole/tcp.h"

#define PORT 8080                   // PORT of server
#define BACKLOG 10                  // Queue size
#define MESSAGE_BUFFER_SIZE 2048    // Message size


#endif