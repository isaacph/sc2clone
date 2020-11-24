//
// Created by isaac on 11/18/2020.
//

#include <winsock2.h>
#include <iostream>

int main() {
    SOCKET serverSocket;
    int socketOn;
    WSADATA wsa;
    struct sockaddr_in address;
    int port = 3800;
    bool findPort = false;

    std::cout << "Initialising Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        std::cerr << "Could not initialize Winsock: " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "Initialized.\n";

    if((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "Socket created.\n";

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // set nonblocking
    u_long mode = 1;
    if(ioctlsocket(serverSocket, FIONBIO, &mode) == SOCKET_ERROR) {
        std::cerr << "Could not set socket to nonblocking: " << WSAGetLastError() << std::endl;
        return 1;
    }

    while(bind(serverSocket, (struct sockaddr*) &address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cerr << "Could not bind socket at port " << port << ": " << WSAGetLastError() << std::endl;
        if(findPort) {
            port++;
            address.sin_port = htons(port);
        } else {
            return 1;
        }
    }

    std::cout << "block?\n";

    struct sockaddr_in client;
    int clientSize = sizeof(client);
    bool send = false;
    bool canSend = false;
    std::string message;
    WSAEVENT event = WSACreateEvent();
    WSANETWORKEVENTS events;
    while(true) {

        int rc = WSAEventSelect(serverSocket, event, FD_READ | FD_WRITE);
        if(rc) {
            WSACloseEvent(event);
            continue;
        }
        WSAEnumNetworkEvents(serverSocket, event, &events);
        if((events.lNetworkEvents & FD_READ) && (events.iErrorCode[FD_READ_BIT] == 0)) {
            int rc2 = 0;
            char msg[128];
            while(rc2 >= 0) {
                rc2 = recvfrom(serverSocket, msg, 128, 0, (sockaddr*) &client, &clientSize);
                std::cout << rc2 << ", " << msg << "\n";
                std::cout << WSAGetLastError() << "\n";
                message = std::string(msg);
            }
            std::cout << WSAGetLastError() << "\n";
            send = true;
        }
        if(events.lNetworkEvents & FD_WRITE && (events.iErrorCode[FD_WRITE_BIT] == 0)) {
            canSend = true;
            std::cout << "write\n";
        }
        if(canSend) {
            if(send) {
                std::cout << "attempting to send\n";
                std::string msg = "ping";
                int rc2 = ::sendto(serverSocket, msg.c_str(), msg.size(), 0, (sockaddr*) &client, clientSize);
                std::cout << rc2 << "\n";
                if(rc2 < 0) {
                    canSend = false;
                    std::cout << "send failure\n";
                    std::cout << WSAGetLastError() << "\n";
                } else {
                    return 0;
                }
            }
        }
    }
    WSACloseEvent(event);
//    std::cout << FD_ISSET(serverSocket, &recvFlag) << "\n";
    return 0;
}