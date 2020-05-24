//
// Created by isaac on 5/10/2020.
//

#include "Server.h"
#include <chrono>
#include "../math_util.h"

Server::Server(Shared &shared) : shared(shared), total_users(0) {
}

void Server::run() {
    world.force_root = true;

    double delta_add, delta = 0;
    std::chrono::steady_clock::time_point delta_begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point delta_end = delta_begin;
    while(!shared.exit) {
        delta = 0;
        while(delta < 0.001) {
            delta_end = std::chrono::steady_clock::now();
            delta_add = std::chrono::duration<double>(delta_end - delta_begin).count();
            delta_begin = delta_end;
            delta += delta_add;
        }

        std::unique_ptr<Packet> packet;
        while((packet = shared.listen.get()) != nullptr) {
            std::cout << packet->ip() << ":" << packet->port() << ": " << packet->message << std::endl;
            processPacket(std::move(packet));
        }

        world.update(delta);

        while(world.has_next_event()) {
            WorldEvent event = world.next_event();
            if(event.type == WorldEvent::UNIT_UPDATE) {
                broadcast_sync("world unit " + world.units[event.identity].to_string());
            } else if(event.type == WorldEvent::COMMAND) {
                broadcast_sync("world command " + world.unit_command_string(event.command, event.command_ids));
            } else if(event.type == WorldEvent::DEAD_UNIT) {
                broadcast_sync("world unit " + world.units[event.identity].to_string());
            } else if(event.type == WorldEvent::PROJECTILE) {
                broadcast_sync("world projectile " + world.projectiles[event.identity].to_string());
            }
        }
    }
}

void Server::send_async(User& user, std::string message) {
    shared.response.send_async(std::make_unique<Packet>(Packet(message, user.address)));
}

void Server::send_sync(User& user, std::string message) {
    shared.response.send_sync(std::make_unique<Packet>(Packet(message, user.address)));
}

void Server::processPacket(std::unique_ptr<Packet> packet) {
    // find the user who made the command
    // add the user if that user does not exist
    // find the command in the packet
    // check if the user has authority to perform the command
    // perform the command (if authorized)
    std::string address_str = packet->str_address();
    auto p2 = users.find(address_str);
    User* user = nullptr;
    if(p2 == users.end()) {
        user = &users.insert({address_str, {
                packet->address,
                std::string("temp") + std::to_string(total_users),
                0
        }}).first->second;
    } else {
        user = &p2->second;
    }

    std::string cmd = format_cmd(packet->message);
    command(*user, cmd, format_args(packet->message));
}
void Server::broadcast_sync(std::string message) {
    for(auto& u : users) {
        auto& user = u.second;
        if(user.authority > 0) {
            send_sync(user, message);
        }
    }
}