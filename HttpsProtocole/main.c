#include "main_header.h"
#include "../ICMP/icmp.h"
#include "../BasicFirewall/firewall.h"
#include <signal.h>

void signal_handler(int sig) {
    printf("\nCaught signal %d, cleaning up...\n", sig);
    cleanup();
    exit(0);
}


int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);  // Ctrl+C
    signal(SIGTERM, signal_handler); // Kill
    init_firewall();
    read_all_packets();

    init_icmp();

    for (int i = 1; i <= 4; i++) {
        send_icmp_echo(i);
        reply_icmp_echo(i);
        sleep(1);
    }

    /*
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
    */
   
    cleanup();

    return 0;
}