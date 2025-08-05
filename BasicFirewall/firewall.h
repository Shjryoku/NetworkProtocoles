#ifndef FIREWALL_H
#define FIREWALL_H
#include "../HttpsProtocole/main_header.h"
#include <linux/if_ether.h>
#include <net/if.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include "../ICMP/icmp.h"

#define BUFFER_SIZE 1024

extern int socketfw;

struct firewall_rule {
    uint32_t src_ip;                    // Source IP
    uint32_t dst_ip;                    // Destination IP
    uint16_t src_port;                  // Source port
    uint16_t dst_port;                  // Destination port
    uint8_t protocol;                   // Link protocol
    uint8_t icmp_type;                  // ICMP type
    uint8_t icmp_code;                  // ICMP code
    uint8_t action;                     // 0 = DROP, 1 = ACCEPT
};

extern struct firewall_rule rules[];
extern struct firewall_rule icmp_rules[];

void init_firewall();
int check_packet(struct iphdr*, uint16_t);
int check_icmp_packet(struct iphdr*, struct icmp_hdr*);
void read_all_packets();
void process_packet(unsigned char*, int);
void cleanup();

#endif