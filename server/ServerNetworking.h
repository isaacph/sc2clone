//
// Created by isaac on 12/21/2020.
//

#ifndef UNTITLED2_SERVERNETWORKING_H
#define UNTITLED2_SERVERNETWORKING_H
#include <queue>
#include <winsock2.h>
#include <memory>
#include "Packet.h"

inline struct sockaddr_in getNetworkingAddress(int port, const std::string& ip) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    return addr;
}

class Networking {
public:
    bool initSocket(int port, bool findPort = false);
    void closeSocket();
    std::vector<std::unique_ptr<Packet>> pollAndSend();
    void send(std::unique_ptr<Packet> packet);
private:
    bool canSend;
    int port;
    SOCKET serverSocket;
    struct sockaddr_in address;
    std::queue<std::unique_ptr<Packet>> packetQueue;
};

#endif //UNTITLED2_SERVERNETWORKING_H
