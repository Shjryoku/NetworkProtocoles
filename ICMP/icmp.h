#ifndef ICMP_H
#define ICMP_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

extern int icmp_socket;

struct icmp_hdr{
    uint8_t type;                           // Message type ICMP - 1b
    uint8_t code;                           // Message code - 1b
    uint16_t checksum;                      // Calculated sum - 2b
    struct
    {
        uint16_t id;                        // Identificator - 2b
        uint16_t seq;                       // Subsequence number - 2b
    } echo;                                 // Echo struct
};

struct icmp_packet {                        // ICMP Packet (ping for example)
    struct icmp_hdr header;                 // ICMP header
    uint32_t timestamp;                     // RTT calculation variable
    char payload[48];                       // Arbitrary data ("0" - default)
};

// Releasing ICMP echo
void init_icmp();
void send_icmp_echo(uint8_t);
void reply_icmp_echo(uint8_t);

// Checksum method (RFC 1071)
uint16_t checksum(const void*, size_t);
#endif