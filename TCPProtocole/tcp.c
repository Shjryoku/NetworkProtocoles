#include "tcp.h"

int tcp_init(){
    // IPV4 socket creation
    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(tcp_socket == -1){
        perror("Creation socked error");
        exit(EXIT_FAILURE);
    }

    // Reuse addr
    int opt = 1;
    if(setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Error for reusing addr");
        exit(EXIT_FAILURE);
    }


    // Linking socket to address and port
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_server_addr.sin_port = htons(PORT);
    tcp_server_addr.sin_family = AF_INET;

    if(bind(tcp_socket, (struct sockaddr*)&tcp_server_addr, sizeof(tcp_server_addr)) < 0){
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // Toggle to listen
    if(listen(tcp_socket, BACKLOG) < 0){
        perror("Listen() error");
        exit(EXIT_FAILURE);
    }

    printf("Server was started by port %d\n", PORT);
    return 0;
}

int tcp_server_read(){
    tcp_client_len = sizeof(tcp_client_addr);

    // Reading cycle
    while(1){
        printf("\nWaiting for new connect...\n");

        // Accepting incoming connect
        tcp_socket_reader = accept(tcp_socket, (struct sockaddr*)&tcp_client_addr, &tcp_client_len);
        if(tcp_socket_reader < 0){
            perror("Connecting error");
            continue;
        }

        printf("Client was connected: %s\n", inet_ntoa(tcp_client_addr.sin_addr));

        char buffer[MESSAGE_BUFFER_SIZE] = {0};

        ssize_t bytes_read = read(tcp_socket_reader, buffer, sizeof(buffer) -1);
        if(bytes_read > 0)
            printf("Request:\n%s\n", buffer);
        else
            printf("Read error or client closed connection\n");

        close(tcp_socket_reader);
    }

    return 0;
}

int tcp_server_write(){
    char response[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 13\r\n"
                    "\r\n";
    
    ssize_t total_written = 0;
    ssize_t to_write = strlen(response);

    while(total_written < to_write){
        ssize_t writen = write(tcp_socket_reader, response, strlen(response));
        if(writen < 0){
            perror("Write error");
            break;
        }
        total_written += writen;
    }

    shutdown(tcp_socket_reader, SHUT_WR);



    return 0;
}

int tcp_client_read(){




    return 0;
}

int tcp_client_write(){



    return 0;
}