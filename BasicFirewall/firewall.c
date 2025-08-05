#include "firewall.h"

int socketfw;

struct firewall_rule rules[] = {
    {0, 0, 0, 22, IPPROTO_TCP, 0},      // Block SSH
    {0, 0, 0, 80, IPPROTO_TCP, 1},      // Allow HTTP
    {0, 0, 0, 0, 0, 0}                  // Block everything else
};

struct firewall_rule icmp_rules[] = {
    {0, 0, 0, 0, IPPROTO_ICMP, ICMP_ECHO, 0xFF, 0},                         // Block ping-requests
    {0, 0, 0, 0, IPPROTO_ICMP, ICMP_ECHOREPLY, 0xFF, 1},                    // Allow ping-replies
    {0, 0, 0, 0, IPPROTO_ICMP, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, 1},     // Allow "Fragmentation needed" for PMTUD
    {0, 0, 0, 0, IPPROTO_ICMP, ICMP_REDIRECT, 0xFF, 0},                     // Block ICMP redirect
    {0, 0, 0, 0, IPPROTO_ICMP, 0xFF, 0xFF, 0}                               // Block other ICMP, but log it 
};

void init_firewall(){
    socketfw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(socketfw < 0){
        fprintf(stderr, "Socket error.\n");
        return;
    }

    int buff_size = 1024 * 1024;
    if(setsockopt(socketfw, SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof(buff_size)) < 0){
        fprintf(stderr, "Set Socket Option failed.\n");
        close(socketfw);
        return;
    }
}

int check_icmp_packet(struct iphdr* iph, struct icmp_hdr* icmp){
    for(int i = 0; i < sizeof(icmp_rules)/sizeof(icmp_rules[i]); i++){
        if(icmp_rules[i].protocol != IPPROTO_ICMP) continue;

        if((icmp_rules[i].icmp_type == 0xFF || 
            icmp_rules[i].icmp_type == icmp->type) &&
            (icmp_rules[i].icmp_code == 0xFF ||
            icmp_rules[i].icmp_code == icmp->code))
        return icmp_rules[i].action;
    }
    // Block by default
    return 0;
}

int check_packet(struct iphdr* iph, uint16_t dst_port){
    for(int i = 0; rules[i].protocol != 0 || rules[i].dst_port != 0; i++){
        if((rules[i].protocol == 0 || 
            rules[i].protocol == iph->protocol) && 
            (rules[i].dst_port == 0 || rules[i].dst_port == dst_port)) 
        return rules[i].action;
    }
    // Block by default
    return 0;
}

void process_packet(unsigned char* data, int size){
    struct ethhdr* eth = (struct ethhdr*)data;

    // Only IP packets
    if(ntohs(eth->h_proto) != ETH_P_IP) return;
    struct iphdr* iph = (struct iphdr*)(data + sizeof(struct ethhdr));
    uint16_t dst_port = 0;

    // Filtration
    printf("Packet has been received!\n");
    switch (iph->protocol)
    {
    case IPPROTO_TCP:
        struct tcphdr* tcp = (struct tcphdr*)(data + sizeof(struct ethhdr) + iph->ihl * 4);
        dst_port = htons(tcp->dest);
        break;
    case IPPROTO_UDP:
        struct udphdr* udp = (struct udphdr*)(data + sizeof(struct ethhdr) + iph->ihl * 4);
        dst_port = htons(udp->dest);
        break;
    case IPPROTO_ICMP:
        struct icmp_hdr* icmp = (struct icmp_hdr*)(data + sizeof(struct ethhdr) + iph->ihl * 4);
        printf("Packet %-20s:\nType=%d\nCode=%d\n", check_icmp_packet(iph, icmp) ? "accepted" : "denied", icmp->type, icmp->code);
        return;
    default:
        fprintf(stderr, "Unknown packet protocol");
        close(socketfw);
        return;
    }

    printf("Packet %-20s:\nProtocol=%d\nPort=%d", check_packet(iph, dst_port) ? "accepted" : "denied", iph->protocol, dst_port);
}

void read_all_packets(){
    struct ifreq ifr;                                                                                   // Create Interface Request structure
    memset(&ifr, 0, sizeof(ifr));                                                                       // Allocate memory
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);                                                        // Copy Interface name
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if(ioctl(socketfw, SIOCGIFINDEX, &ifr) < 0){                                                        // Get Iterface Index
        fprintf(stderr, "Failed to get index!\n");
        close(socketfw);
        return;
    }

    if(ioctl(socketfw, SIOCGIFFLAGS, &ifr) < 0){                                                        // Get Interface Flags
        fprintf(stderr, "Failed to get flags!\n");
        close(socketfw);
        return;
    }

    // Write Interface name
    printf("\n%-40s\n", "-----------------------------------------------");
    printf("%-20s %-20s \n\n", "INTERFACE:", ifr.ifr_name);
     // Is Interface enabled
    printf("%-20s %-10s\n", "STATUS:", (ifr.ifr_flags & IFF_UP) ? "UP" : "DOWN");
    // Is Interface active
    printf("%-20s %-10s\n", "PHYSICAL LINK:", (ifr.ifr_flags & IFF_RUNNING) ? "CONNECTED" : "NOT CONNECTED");
    // Is Interface virtual loopback
    printf("%-20s %-10s\n", "TYPE:", (ifr.ifr_flags & IFF_LOOPBACK) ? "LOOPBACK" : "PHYSICAL");

    if(!(ifr.ifr_flags & IFF_LOOPBACK)){
        printf("%-20s %-10s\n\n", "PROMISC:", (ifr.ifr_flags & IFF_PROMISC) ? "ENABLED" : "DISABLED");
        
        // Turn on promiscuous if it turned off
        if(!(ifr.ifr_flags & IFF_PROMISC)) {
            printf("%-20s\n", "Enabling promiscuous mode...");
            ifr.ifr_flags |= IFF_PROMISC;
            if(ioctl(socketfw, SIOCSIFFLAGS, &ifr) < 0)
                fprintf(stderr, "%-20s %s\n", "Error:", "Failed to enable promiscuous mode");
            else
                printf("%-20s %s\n", "Success:", "Promiscuous mode enabled");
        }
    }
    printf("%-40s\n\n", "-----------------------------------------------");

    printf("Firewall started!\n");
    unsigned char buffer[BUFFER_SIZE];
    while(1){                                                                                           // Infinite cycle for read packets
        // Get packet
        int packet_size = recvfrom(socketfw, &buffer, BUFFER_SIZE, 0, NULL, NULL);
        if(packet_size < 0){
            printf("Failed to receive packet.\n");
            continue;
        } else if(packet_size < sizeof(struct ethhdr) + sizeof(struct iphdr)){
            printf("Packet is too small!\n");
            continue;
        }

        // Process packet
        process_packet(buffer, packet_size);
    }
}

void cleanup() {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    // Get current flags
    if (ioctl(socketfw, SIOCGIFFLAGS, &ifr) < 0) {
        perror("Failed to get interface flags");
        return;
    }

    // Turn off promiscuous mode
    ifr.ifr_flags &= ~IFF_PROMISC;

    // Apply changes
    if (ioctl(socketfw, SIOCSIFFLAGS, &ifr) < 0)
        perror("Failed to disable promiscuous mode");
    else
        printf("Promiscuous mode disabled\n");

    if(socketfw > 0) {
        close(socketfw);
        socketfw = -1;
    }
}