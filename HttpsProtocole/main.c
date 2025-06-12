#include "main_header.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [server|client] %s [tcp|udp]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "server") == 0) {
        if(strcmp(argv[2], "tcp") == 0){
            tcp_server_init();
            tcp_server_read();
        } else if(strcmp(argv[2], "udp") == 0){
            udp_server_init();
            udp_server_read();
        }
    } else if (strcmp(argv[1], "client") == 0) {
        if(strcmp(argv[2], "tcp") == 0){
            tcp_client_init();
            tcp_client_interactive();
        } else if(strcmp(argv[2], "udp") == 0){
            udp_client_init();
            udp_client_interactive();
        }
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return 0;
}