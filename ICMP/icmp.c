#include "icmp.h"
#include <sys/time.h>
#include <time.h>

int icmp_socket;

uint16_t checksum(const void* data, size_t length){
    const uint16_t* ptr = data;                                     // Pointer to data (16-bit)
    uint32_t sum = 0;                                               // Accumulated sum (32-bit)

    // Main sum cycle
    while (length > 1) {
        sum += *ptr++;                                              // Add current 16-bit data
        length -= 2;                                                // Reduce sum
    }

    if(length == 1)
        sum += *(uint8_t*)ptr;                                      // Add last byte as hight byte
    
    // Overflow collapse
    sum = (sum >> 16) + (sum & 0xFFFF);                             // Add transfer
    sum += (sum >> 16);                                             // For new overflow

    printf("Sended packet checksum: 0x%04X\n", sum);
    return (uint16_t)(~sum);
}

void init_icmp(){
    icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(icmp_socket < 0){
        fprintf(stderr, "ICMP socket is not initiated!\n");
        return;
    }
}

void send_icmp_echo(uint8_t seq){
    // Fill destination address
    struct sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_addr.s_addr = inet_addr("127.0.0.1");           // Set local address for testing or "208.67.222.222" for testing by OpenDNS
    if(destination.sin_addr.s_addr == INADDR_NONE){
        fprintf(stderr, "Invalid IP Address.\n");
        return;
    }

    // Fill ICMP packet
    struct icmp_packet packet;
    packet.header.type = 8;                                         // Echo Request type
    packet.header.code = 0;                                         // Echo Reply code
    packet.header.echo.id = htons(getpid());                        // Set ICMP echo id by PID
    packet.header.echo.seq = htons(seq);                            // Set ICMP echo sequence by given sequence
    packet.timestamp = htonl(time(NULL));                           // Get current Unix Time


    // Calculating checksum
    packet.header.checksum = 0;                                     // Set checksum to 0 every call
    packet.header.checksum = checksum(&packet, sizeof(packet));     // Get packet checksum

    // Send packet
    ssize_t sent = sendto(icmp_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&destination, sizeof(destination));
    if(sent < 0){
        fprintf(stderr, "Sending packet is impossible.\n");
        close(icmp_socket);
        return;
    }
}

void reply_icmp_echo(uint8_t seq){
    char buffer[1024];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    time_t start_time = time(NULL);
    const int timeout_sec = 15;

    // Set timeout for every call recvfrom
    struct timeval per_recv_timeout = { .tv_sec = 1, .tv_usec = 0 };

    while(1){
        // Check timeout
        if(difftime(time(NULL), start_time) > timeout_sec){
            fprintf(stderr, "Timeout waiting for reply.\n");
            return;
        }

        setsockopt(icmp_socket, SOL_SOCKET, SO_RCVTIMEO, &per_recv_timeout, sizeof(per_recv_timeout));

        // Wait for reply
        ssize_t received = recvfrom(icmp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender, &sender_len);
            if(received < 0){
            fprintf(stderr, "Receive message error.\n");
            return;
        }

        // Parse IP header
        struct iphdr *ip_header = (struct iphdr*)buffer;
        int ip_header_len = ip_header->ihl * 4;
        
        if(received < ip_header_len + sizeof(struct icmp_hdr)) {
            fprintf(stderr, "Invalid packet size.\n");
            return;
        }

        // Parse ICMP header
        struct icmp_hdr* reply = (struct icmp_hdr*)(buffer + ip_header_len);

        uint16_t recv_id = ntohs(reply->echo.id);
        uint16_t recv_seq = ntohs(reply->echo.seq);

        printf("DEBUG: Received packet - type: %d, code: %d, id: %d, seq: %d\n", 
            reply->type, reply->code, recv_id, recv_seq);
        printf("DEBUG: Expected - id: %d, seq: %d\n", (uint16_t)getpid(), seq);
        printf("Received %zd bytes\n", received);
        fflush(stdout);

        if(reply->type == 0 && reply->code == 0 && recv_id == (uint16_t)getpid() && recv_seq == seq){
            struct icmp_packet* recv_packet = (struct icmp_packet*)(buffer + ip_header_len);

            time_t now = time(NULL);
            uint32_t timestamp_net = recv_packet->timestamp;
            uint32_t timestamp_host = ntohl(timestamp_net);
            double rtt = difftime(now, timestamp_host);

            // Verify checksum
            uint16_t saved_checksum = reply->checksum;
            reply->checksum = 0;
            uint16_t calculated_checksum = checksum(reply, received - ip_header_len);
            if(saved_checksum != calculated_checksum) {
                fprintf(stderr, "Checksum mismatch! Packet corrupted.\n");
                continue;
            }

            // Only process our packets
            if(recv_id != (uint16_t)getpid()) {
                continue;
            }

            switch (reply->type) {
            case ICMP_ECHOREPLY:                                                        // Echo Reply
                printf("Answer from %s: \nSequence: %d \nTime: %.2f ms\n",
                inet_ntoa(sender.sin_addr),
                recv_seq,
                rtt * 1000);
                break;
            case ICMP_DEST_UNREACH:                                                     // Destination Unreachable
                switch (reply->code)
                {
                case 0:
                    fprintf(stderr, "Network Unreachable.\n");
                    break;
                case 1:
                    fprintf(stderr, "Host Unreachable.\n");
                    break;
                case 2:
                    fprintf(stderr, "Protocol Unreachable.\n");
                    break;
                case 3:
                    fprintf(stderr, "Port unreachable.\n");
                    break;
                default:
                    fprintf(stderr, "Unknown Echo Code.\n");
                    break;
                }
                break;
            case ICMP_TIME_EXCEEDED:                                                    // Time Exceeded (TTL Expired)
                fprintf(stderr, "Time exceeded (TTL expired).\n");
                break;
            default:
                fprintf(stderr, "Unknown ICMP Type: %d\n", reply->type);
                break;
            }
        }
    }
}