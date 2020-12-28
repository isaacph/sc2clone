//
// Created by isaac on 5/10/2020.
//

#ifndef UNTITLED2_SERVER_H
#define UNTITLED2_SERVER_H
#include "../logic/World.h"
#include "../logic/UniqueID.h"
#include <queue>
#include <memory>
#include "Packet.h"
#include "ServerNetworking.h"

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
    void broadcast(std::string message);
    void send(User& user, const std::string& message);
    void command(User& user, std::string cmd, std::string args);
    void processPacket(std::unique_ptr<Packet> packet);

    World world;
    std::map<UniqueID, User> users;
    int total_users;

    std::atomic<bool>& running;

    Networking networking;
};


#endif //UNTITLED2_SERVER_H
