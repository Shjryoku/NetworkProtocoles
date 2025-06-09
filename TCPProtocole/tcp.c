#include "tcp.h"

int tcp_server_socket;
int tcp_client_socket;

struct sockaddr_in tcp_server_addr;
struct sockaddr_in tcp_client_addr;

socklen_t tcp_client_len;
socklen_t tcp_server_len;


int tcp_server_init(){
    // IPV4 socket creation
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcp_server_socket == -1){
        perror("Creation socked error");
        exit(EXIT_FAILURE);
    }

    printf("tcp_server_socket = %d\n", tcp_server_socket);

    // Reuse addr
    int opt = 1;
    if(setsockopt(tcp_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Error for reusing addr");
        exit(EXIT_FAILURE);
    }

    // Linking socket to address and port
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_server_addr.sin_port = htons(PORT);
    tcp_server_addr.sin_family = AF_INET;

    if(bind(tcp_server_socket, (struct sockaddr*)&tcp_server_addr, sizeof(tcp_server_addr)) < 0){
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // Toggle to listen
    if(listen(tcp_server_socket, BACKLOG) < 0){
        perror("Listen() error");
        exit(EXIT_FAILURE);
    }

    printf("Server was started by port %d\n", PORT);
    return 0;
}

int tcp_client_init(){
    // IPV4 socket creation
    tcp_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcp_client_socket == -1){
        perror("Creation socked failed");
        exit(EXIT_FAILURE);
    }

    // Linking socket to address and port
    tcp_client_addr.sin_family = AF_INET;
    tcp_client_addr.sin_port = htons(PORT);
    tcp_client_addr.sin_addr.s_addr = inet_addr(ADDRESS);

    tcp_server_len = sizeof(tcp_server_addr);

    if(connect(tcp_client_socket, (struct sockaddr*)&tcp_client_addr, sizeof(tcp_client_addr)) < 0){
        perror("Connect() error");
        exit(EXIT_FAILURE);
    }

    return 0;
}