//
// Created by isaac on 5/10/2020.
//

#ifndef UNTITLED2_SERVER_H
#define UNTITLED2_SERVER_H
#include "../logic/World.h"
#include "../logic/UniqueID.h"
#include <queue>
#include <memory>
#include <winsock2.h>
#include "packet.h"

struct User {
    struct sockaddr_in address;
    std::string name;
    int authority;
    int team;
};

struct UserCommand {
    int authority;
    std::function<void(User& user, std::string str)> function;
};

class Server {
public:
    Server(std::atomic<bool>& running);

    void run();
private:
    void broadcastSync(std::string message);
    void sendSync(User& user, const std::string& message);
    void command(User& user, std::string cmd, std::string args);
    void processPacket(std::unique_ptr<Packet> packet);

    bool initSocket(int port, bool findPort = false);
    void closeSocket();
    std::vector<std::unique_ptr<Packet>> pollAndSend();
    void send(std::unique_ptr<Packet> packet);

    bool canSend;
    int socketOn;
    int port;
    SOCKET serverSocket;
    struct sockaddr_in address;
    std::queue<std::unique_ptr<Packet>> packetQueue;

    World world;
    std::map<UniqueID, User> users;
    int total_users;

    std::atomic<bool>& running;
};


#endif //UNTITLED2_SERVER_H
