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

            if(fgets(response_buffer, MESSAGE_BUFFER_SIZE, stdin) == NULL) break;

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

int udp_server_read(){
    udp_client_len = sizeof(udp_client_addr);

    // Reading cycle
    while(1){
        printf("\nWaiting for new connect...\n");

        char buffer[MESSAGE_BUFFER_SIZE] = {0};
        ssize_t bytes_read;
        while ((bytes_read = recvfrom(udp_server_socket, buffer, MESSAGE_BUFFER_SIZE - 1, 0, (struct sockaddr*)&udp_client_addr, &udp_client_len)) > 0)
        {
            buffer[bytes_read] = '\0';
            printf("Received from %s: %s\n", inet_ntoa(udp_client_addr.sin_addr), buffer);

            char response_buffer[MESSAGE_BUFFER_SIZE];
            printf("Enter response: \n");
            fflush(stdout);

            if(fgets(response_buffer, MESSAGE_BUFFER_SIZE, stdin) == NULL) break;

            size_t len = strlen(response_buffer);
            if(len > 0 && response_buffer[len - 1] == '\n')
                response_buffer[len - 1] = '\0';

            unsigned int bytes_sent = sendto(udp_server_socket, response_buffer, strlen(response_buffer), 0, (struct sockaddr*)&udp_client_addr, udp_client_len);
            if(bytes_sent < 0){
                perror("sendto error");
            }
        }
    }

    return 0;
}