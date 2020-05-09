//
// Created by isaac on 5/8/2020.
//

#include "packet.h"

Packet::Packet(const std::string& message, struct sockaddr_in address, int address_length)
        : message(message), address(address), address_length(address_length) {

}

Packet::Packet(const std::string& message, struct sockaddr_in address)
        : message(message), address(address), address_length(sizeof(address)) {

}

