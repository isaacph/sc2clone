//
// Created by isaac on 5/10/2020.
//

#ifndef UNTITLED2_SERVER_H
#define UNTITLED2_SERVER_H
#include "shared.h"
#include "../logic/World.h"
#include "../logic/UniqueID.h"

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
    Server(Shared& shared);

    void run();
    void send_async(User& user, std::string message);
    void send_sync(User& user, std::string message);
    void broadcast_sync(std::string message);
private:
    void command(User& user, std::string cmd, std::string args);
    void processPacket(std::unique_ptr<Packet> packet);

    Shared& shared;
    World world;
    std::map<UniqueID, User> users;
    int total_users;
};


#endif //UNTITLED2_SERVER_H
