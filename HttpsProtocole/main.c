#include "main_header.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [server|client]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "server") == 0) {
        tcp_server_init();
        tcp_server_read();
    } else if (strcmp(argv[1], "client") == 0) {
        tcp_client_init();
        tcp_client_interactive();
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return 0;
}