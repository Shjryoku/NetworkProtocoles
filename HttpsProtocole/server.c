#include "main_header.h"

void InitTypeProtocole(int type_of_protocole){
    switch (type_of_protocole) {
    case 0:
        tcp_init();
        break;
    
    default:
        perror("Unknown protocole type");
        break;
    }
}