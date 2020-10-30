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
        char buffer[20];
        int bufLen = 20;
        WSAAddressToString(reinterpret_cast<LPSOCKADDR>(&address), address_length, NULL, buffer,
                           reinterpret_cast<LPDWORD>(bufLen));
        return std::string(buffer, bufLen);
    }
};

#endif //UNTITLED2_PACKET_H
