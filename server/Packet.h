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
    int port;
    Packet(const std::string& message, struct sockaddr_in address, int address_length);
    Packet(const std::string& message, struct sockaddr_in address);

    inline std::string str_address() {
        return std::string(inet_ntoa(address.sin_addr)) + ":" + std::to_string(address.sin_port);
    }
};

#endif //UNTITLED2_PACKET_H
