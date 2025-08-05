#ifndef ICMP_H
#define ICMP_H
#include "../HttpsProtocole/main_header.h"

extern int icmp_socket;

// ICMP Types
#define ICMP_ECHOREPLY              0       // Echo Reply
#define ICMP_DEST_UNREACH           3       // Destination Unreachable
#define ICMP_SOURCE_QUENCH          4       // Source Quench (Deprecated)
#define ICMP_REDIRECT               5       // Redirect Message
#define ICMP_ECHO                   8       // Echo Request (Ping)
#define ICMP_TIME_EXCEEDED         11       // Time Exceeded
#define ICMP_PARAMETERPROBLEM      12       // Parameter Problem
#define ICMP_TIMESTAMP             13       // Timestamp Request
#define ICMP_TIMESTAMPREPLY        14       // Timestamp Reply
#define ICMP_INFO_REQUEST          15       // Information Request (Obsolete)
#define ICMP_INFO_REPLY            16       // Information Reply (Obsolete)
#define ICMP_ADDRESS               17       // Address Mask Request
#define ICMP_ADDRESSREPLY          18       // Address Mask Reply 

// ICMP Type 3 Codes
#define ICMP_NET_UNREACH           0        // Network Unreachable
#define ICMP_HOST_UNREACH          1        // Host Unreachable
#define ICMP_PROT_UNREACH          2        // Protocol Unreachable
#define ICMP_PORT_UNREACH          3        // Port Unreachable
#define ICMP_FRAG_NEEDED           4        // Fragmentation Needed
#define ICMP_SR_FAILED             5        // Source Route Failed
#define ICMP_NET_UNKNOWN           6        // Network Unknown
#define ICMP_HOST_UNKNOWN          7        // Host Unknown
#define ICMP_HOST_ISOLATED         8        // Host Isolated
#define ICMP_NET_PROHIB            9        // Network Prohibited
#define ICMP_HOST_PROHIB          10        // Host Prohibited
#define ICMP_TOS_NET_UNREACH      11        // TOS Network Unreachable
#define ICMP_TOS_HOST_UNREACH     12        // TOS Host Unreachable
#define ICMP_COMM_PROHIB          13        // Communication Prohibited
#define ICMP_HOST_PRECEDENCE      14        // Host Precedence Violation
#define ICMP_PRECEDENCE_CUTOFF    15        // Precedence Cutoff

// ICMP Type 11 Codes
#define ICMP_EXC_TTL               0        // TTL Exceeded
#define ICMP_EXC_FRAGTIME          1        // Fragment Reassembly Time Exceeded

// ICMP Type 5 Codes
#define ICMP_REDIR_NET             0        // Redirect for Network
#define ICMP_REDIR_HOST            1        // Redirect for Host
#define ICMP_REDIR_TOS_NET         2        // Redirect for TOS and Network
#define ICMP_REDIR_TOS_HOST        3        // Redirect for TOS and Host

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