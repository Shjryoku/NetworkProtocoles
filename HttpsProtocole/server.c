#include "main_header.h"

int tcp_server_read(){
    tcp_client_len = sizeof(tcp_client_addr);

    // Reading cycle
    while(1){
        printf("\nWaiting for new connect...\n");

        // Accepting incoming connect
        int client_socket = accept(tcp_server_socket, (struct sockaddr*)&tcp_client_addr, &tcp_client_len);
        if(client_socket < 0){
            perror("Server connecting error");
            continue;
        }

        printf("Client was connected: %s\n", inet_ntoa(tcp_client_addr.sin_addr));

        char buffer[MESSAGE_BUFFER_SIZE] = {0};
        ssize_t bytes_read;

        while((bytes_read = read(client_socket, buffer, sizeof(buffer) -1)) > 0){
            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);

            char response_buffer[MESSAGE_BUFFER_SIZE];
            printf("Enter response: \n");
            fflush(stdout);

            if(fgets(response_buffer, sizeof(response_buffer), stdin) == NULL) break;

            size_t len = strlen(response_buffer);

            if (len > 0 && response_buffer[len-1] == '\n')
                response_buffer[len-1] = '\0';

            write(client_socket, response_buffer, strlen(response_buffer));
        }
        
        if (bytes_read == 0) {
            printf("Client closed connection\n");
        } else if (bytes_read < 0) {
            perror("read error");
        }

        close(client_socket);   
    }

    return 0;
}

int tcp_server_write(int client_socket){
    char response[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 13\r\n"
                    "\r\n";
    
    ssize_t total_written = 0;
    ssize_t to_write = strlen(response);

    while(total_written < to_write){
        ssize_t writen = write(client_socket, response + total_written, to_write - total_written);
        if(writen < 0){
            perror("Write error");
            break;
        }
        total_written += writen;
    }

    close(client_socket);

    return 0;
}