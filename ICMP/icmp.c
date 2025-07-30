#include "icmp.h"

void init_icmp(){
    icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socket < 0){
        fprintf(stderr, "ICMP socket is not initiated!\n");
        return;
    }
}

void send_icmp_echo(uint8_t seq){
    // Fill destination address
    struct sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_addr.s_addr = inet_addr("127.0.0.1");           // Set local address for testing or "208.67.222.222" for testing by OpenDNS

    // Fill ICMP packet
    struct icmp_packet packet;
    packet.header.type = 8;                                         // Echo Request type
    packet.header.code = 0;                                         // Echo Reply code
    packet.header.echo.id = getpid();                               // Set ICMP echo id by PID
    packet.header.echo.seq = seq;                                   // Set ICMP echo sequence by given sequence
    packet.timestamp = time(NULL);                                  // Get current Unix Time


    // Calculating checksum
    packet.header.checksum = 0;                                     // Set checksum to 0 every call
    packet.header.checksum = checksum(&packet, sizeof(packet));     // Get packet checksum

    // Send packet
    ssize_t sent = sendto(icmp_socket, &packet, sizeof(packet), 0, (struct sockaddr_in*)&destination, sizeof(destination));
    if(sent < 0){
        fprintf(stderr, "Sending packet is impossible.\n");
        close(icmp_socket);
    }
}

void reply_icmp_echo(uint8_t seq){
    char buffer[1024];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    // Set timeout - 15 seconds for example
    struct timeval timeout = { .tv_sec = 15, .tv_usec = 0 };
    setsockopt(icmp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sender_len);

    // Wait for reply
    ssize_t received = recvfrom(icmp_socket, buffer, sizeof(buffer), 0, (struct sockaddr_in*)&sender, &sender_len);
    if(received < 0){
        fprintf(stderr, "Receive message error.\n");
        return;
    }

    // Check ICMP Echo Reply
    struct icmp_hdr* reply = (struct icmp_hdr*)(buffer + 20); // +20 - Skip IP-header
    if(reply->type == 0 && reply->code == 0 && reply->echo.id == getpid()){
        
    }
}
