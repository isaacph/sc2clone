//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_PACKET_H
#define UNTITLED2_PACKET_H
#include <string>
#include <winsock2.h>

struct Packet {
    std::string message;
    struct sockaddr_in address;
    int address_length;
    Packet(const std::string& message, struct sockaddr_in address, int address_length);
    Packet(const std::string& message, struct sockaddr_in address);

    inline std::string ip() {
        return std::string(inet_ntoa(*((in_addr*) &address)));
    }
    inline unsigned short port() {
        return ntohs(address.sin_port);
    }
};

#endif //UNTITLED2_PACKET_H
