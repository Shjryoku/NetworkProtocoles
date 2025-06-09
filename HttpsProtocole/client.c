#include "main_header.h"

int tcp_client_interactive(){
    char send_buffer[MESSAGE_BUFFER_SIZE];
    char read_buffer[MESSAGE_BUFFER_SIZE];

    while (1)
    {
        printf("Enter request: ");
        if(!fgets(send_buffer, sizeof(send_buffer), stdin)) break;

        ssize_t sent = write(tcp_client_socket, send_buffer, strlen(send_buffer));
        if(sent < 0){
            perror("Write error");
            break;
        }

        ssize_t received = read(tcp_client_socket, read_buffer, sizeof(read_buffer) - 1);
        if(received < 0){
            perror("Read error");
            break;
        } else if(received == 0){
            perror("Server closed connection\n");
            break;
        }

        read_buffer[received] = '\0';
        printf("Server answer:\n%s\n", read_buffer);
    }

    return 0;
}