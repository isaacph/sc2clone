//
// Created by isaac on 5/10/2020.
//

#include "Server.h"
#include <chrono>
#include <atomic>
#include <iostream>
#include "../math_util.h"

Server::Server(std::atomic<bool>& running) : running(running), total_users(0) {
}

void Server::run() {

    if(!initSocket(3800)) {
        std::cout << "Server didn't start.\n";
        return;
    }
    double delta_add, delta = 0;
    std::chrono::steady_clock::time_point delta_begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point delta_end = delta_begin;
    while(running.load(std::memory_order_relaxed)) {
        delta = 0;
        while(delta < 0.001) {
            delta_end = std::chrono::steady_clock::now();
            delta_add = std::chrono::duration<double>(delta_end - delta_begin).count();
            delta_begin = delta_end;
            delta += delta_add;
        }

        std::vector<std::unique_ptr<Packet>> packets = pollAndSend();
        for(auto& packet : packets) {
            std::cout << packet->message << "\n";
            processPacket(std::move(packet));
        }

        world.update(delta);

        while(world.has_next_event()) {
            WorldEvent event = world.next_event();
            if(event.type == WorldEvent::UNIT_UPDATE) {
                broadcastSync("world unit " + world.units[event.identity].to_string());
            } else if(event.type == WorldEvent::COMMAND) {
                broadcastSync("world command " + world.unit_command_string(event.command, event.command_ids));
            } else if(event.type == WorldEvent::DEAD_UNIT) {
                broadcastSync("world unit " + world.units[event.identity].to_string());
            } else if(event.type == WorldEvent::PROJECTILE) {
                broadcastSync("world projectile " + world.projectiles[event.identity].to_string());
            }
        }
    }

    closeSocket();
}

void Server::sendSync(User& user, const std::string& message) {
    send(std::make_unique<Packet>(Packet(message, user.address)));
}

void Server::processPacket(std::unique_ptr<Packet> packet) {
    // find the user who made the command
    // add the user if that user does not exist
    // find the command in the packet
    // check if the user has authority to perform the command
    // perform the command (if authorized)
    size_t spacePos = packet->message.find(' ');
    if(spacePos == std::string::npos) {
        std::cout << "Invalid packet received from address " + packet->str_address() << "\n";
    }
    UniqueID identifier(packet->message.substr(0, spacePos));
    auto p2 = users.find(identifier);
    User* user = nullptr;
    if(p2 == users.end()) {
        user = &users.insert({identifier, {
                packet->address,
                std::string("temp") + std::to_string(total_users),
                0
        }}).first->second;
    } else {
        user = &p2->second;
    }
    std::string removeIdentifier = packet->message.substr(spacePos + 1, std::string::npos);
    std::string cmd = format_cmd(removeIdentifier);
    std::string args = format_args(removeIdentifier);
    command(*user, cmd, args);
}
void Server::broadcastSync(std::string message) {
    for(auto& u : users) {
        auto& user = u.second;
        if(user.authority > 0) {
            sendSync(user, message);
        }
    }
}