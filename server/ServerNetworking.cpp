//
// Created by isaac on 11/17/2020.
//

#include "Server.h"
#include <iostream>

bool Server::initSocket(int port, bool findPort) {
    this->port = port;
    WSADATA wsa;

    std::cout << "Initialising Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        std::cerr << "Could not initialize Winsock: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Initialized.\n";

    if((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        closeSocket();
        return false;
    }
    std::cout << "Socket created.\n";

//    if((setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &socketOn, sizeof(socketOn))) < 0) {
//        std::cerr << "Could not set socket address reusable: " << WSAGetLastError() << std::endl;
//        closeSocket();
//        return false;
//    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // set nonblocking
    u_long mode = 1;
    if(ioctlsocket(serverSocket, FIONBIO, &mode) == SOCKET_ERROR) {
        std::cerr << "Could not set socket to nonblocking: " << WSAGetLastError() << std::endl;
        closeSocket();
        return false;
    }

    while(bind(serverSocket, (struct sockaddr*) &address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cerr << "Could not bind socket at port " << port << ": " << WSAGetLastError() << std::endl;
        if(findPort) {
            port++;
            address.sin_port = htons(port);
        } else {
            closeSocket();
            return false;
        }
    }

    canSend = false;

    std::cout << "Socket bound to port " << port << ".\n";
    return true;
}

void Server::closeSocket() {
    closesocket(serverSocket);
    std::cout << "Socket closed.\n";
}

std::vector<std::unique_ptr<Packet>> Server::pollAndSend() {
    std::vector<std::unique_ptr<Packet>> received;

    WSAEVENT event = WSACreateEvent();
    WSANETWORKEVENTS eventInfo;

    struct sockaddr_in client;
    int clientSize = sizeof(client);
    char clientData[512];
    int rc = WSAEventSelect(serverSocket, event, FD_READ | FD_WRITE);
    if(rc == -1) {
        std::cerr << "Unidentified network select error: " << WSAGetLastError() << std::endl;
    } else {
        WSAEnumNetworkEvents(serverSocket, event, &eventInfo);
        if(eventInfo.lNetworkEvents & FD_WRITE) {
            if(eventInfo.iErrorCode[FD_WRITE_BIT] != 0) {
                std::cerr << "Unidentified network select write flag error: " << eventInfo.iErrorCode[FD_WRITE_BIT] << "\n";
                std::cerr << "Potential error code: " << WSAGetLastError() << "\n";
            } else {
                canSend = true;
            }
        }
        if(eventInfo.lNetworkEvents & FD_READ) {
            if(eventInfo.iErrorCode[FD_READ_BIT] != 0) {
                std::cerr << "Unidentified network select read flag error: " << eventInfo.iErrorCode[FD_READ_BIT] << "\n";
                std::cerr << "Potential error code: " << WSAGetLastError() << "\n";
            } else {
                bool wouldBlock = false;
                do {
                    rc = recvfrom(serverSocket, clientData, 512, 0, (sockaddr *) &client, &clientSize);
                    if (rc == -1) {
                        rc = WSAGetLastError();
                        if (rc == WSAEWOULDBLOCK) {
                            wouldBlock = true;
                        } else {
                            std::cerr << "Unidentified network select recv error: " << rc << "\n";
                            break;
                        }
                    } else {
                        received.push_back(std::make_unique<Packet>(std::string(clientData, rc), client, clientSize));
                    }
                } while(!wouldBlock);
            }
        }
        if(canSend && !packetQueue.empty()) {
            bool wouldBlock = false;
            do {
                Packet& packet = *packetQueue.front();
                rc = sendto(serverSocket,
                            packet.message.c_str(),
                            packet.message.size(),
                            0,
                            (sockaddr*) &packet.address,
                            packet.address_length);
                if(rc == -1) {
                    rc = WSAGetLastError();
                    if(rc == EWOULDBLOCK) {
                        wouldBlock = true;
                        canSend = false;
                    } else {
                        std::cerr << "Unidentified network send error: " << rc << "\n";
                    }
                } else {
                    packetQueue.pop();
                }
            } while(!wouldBlock && !packetQueue.empty());
        }
    }

    WSACloseEvent(event);

    return received;
}

void Server::send(std::unique_ptr<Packet> packet) {
    packetQueue.push(std::move(packet));
}

