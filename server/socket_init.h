//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_SOCKET_INIT_H
#define UNTITLED2_SOCKET_INIT_H

#include <winsock2.h>

inline int init_socket(int& port, int timeout_ms, SOCKET& soc, struct sockaddr_in& address, bool find_port = false) {
    WSADATA wsa;

    std::cout << "Initialising Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        std::cerr << "Could not initialize Winsock: " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "Initialized.\n";

    if((soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        return 2;
    }
    struct timeval tv;
    //tv.tv_sec = shared.timeout_ms / 1000;
    //tv.tv_usec = (shared.timeout_ms % 1000) * 1000;
    tv.tv_sec = timeout_ms;
    tv.tv_usec = 0;
    if(setsockopt(soc, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv))) {
        std::cerr << "Could not set socket timeout: " << WSAGetLastError() << std::endl;
        return 3;
    }
    std::cout << "Socket created.\n";

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    while(bind(soc, (struct sockaddr*) &address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cerr << "Could not bind socket: " << WSAGetLastError() << std::endl;
        if(find_port) {
            port++;
            address.sin_port = htons(port);
        } else {
            return 4;
        }
    }
    std::cout << "Socket bound to port " << port << ".\n";

    return 0;
}

inline struct sockaddr_in get_address(int port, std::string ip) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    return addr;
}

inline void close_socket(SOCKET& soc) {
    closesocket(soc);
    WSACleanup();
    std::cout << "Socket cleaned up.\n";
}

#endif //UNTITLED2_SOCKET_INIT_H
