#include "udp.h"

int udp_server_socket;
int udp_client_socket;

struct sockaddr_in udp_server_addr;
struct sockaddr_in udp_client_addr;

socklen_t udp_client_len;
socklen_t udp_server_len;

int udp_server_init(){
    udp_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(udp_server_socket == -1){
        perror("Creation socket error");
        exit(EXIT_FAILURE);
    }

      printf("udp_server_socket = %d\n", udp_server_socket);

    // Reuse addr
    int opt = 1;
    if(setsockopt(udp_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Error for reusing addr");
        exit(EXIT_FAILURE);
    }

    // Linking socket to address and port
    udp_server_addr.sin_addr.s_addr = INADDR_ANY;
    udp_server_addr.sin_port = htons(PORT);
    udp_server_addr.sin_family = AF_INET;

    if(bind(udp_server_socket, (struct sockaddr*)&udp_server_addr, sizeof(udp_server_addr)) < 0){
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    printf("Server was started by port %d\n", PORT);
    return 0;
}

int udp_client_init(){
     // IPV4 socket creation
    udp_client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(udp_client_socket == -1){
        perror("Creation socked failed");
        exit(EXIT_FAILURE);
    }

    // Linking socket to address and port
    udp_client_addr.sin_family = AF_INET;
    udp_client_addr.sin_port = htons(PORT);
    udp_client_addr.sin_addr.s_addr = inet_addr(ADDRESS);

    udp_server_len = sizeof(udp_server_addr);

    if(connect(udp_client_socket, (struct sockaddr*)&udp_client_addr, sizeof(udp_client_addr)) < 0){
        perror("Addressing error");
        exit(EXIT_FAILURE);
    }

    return 0;
}