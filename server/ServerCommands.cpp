//
// Created by isaac on 5/10/2020.
//

#include "Server.h"
#include "../math_util.h"
#include "socket_init.h"

void Server::command(User& user, std::string cmd, std::string args) {
    if(cmd == "ping") {
        send_sync(user, "ping");
    } else if(cmd == "help") {
        std::string help = "chat Command list not implemented";
        send_sync(user, help);
    } else if(cmd == "username") {
        send_sync(user, "chat User name is: " + user.name);
    } else if(cmd == "login") {
        if (user.authority >= 0) {
            auto args_arr = format_args_arr(args);
            if (args_arr.size() >= 1) {
                user.name = args_arr[0];
                user.authority = 1;
                send_sync(user, "chat Logged in as " + user.name);
                send_sync(user, "root 0");
                for (auto pair : world.units) {
                    send_sync(user, "world unit " + pair.second.to_string());
                }
                if (args_arr.size() == 2 && args_arr[1] == "amazing") {
                    user.authority = 10;
                    send_sync(user, "chat Admin permissions granted");
                }
            } else {
                send_sync(user, "chat Invalid arguments");
            }
        } else {
            send_sync(user, "chat Already logged in");
        }
    } else if(cmd == "logout") {
        user.authority = 0;
        send_sync(user, "chat Logged out");
    } else if(cmd == "world") {
        /*
        if (args.size() > 0 && user.authority >= 0) {
            world.server_command(args);
        }*/
    } else if(cmd == "getunits") {
        for (auto pair : world.units) {
            send_sync(user, "world unit " + pair.second.to_string());
        }
    } else if(cmd == "command") {
        std::vector<int> ids;
        UnitCommand command = UnitCommand::parse(args, ids);
        world.local_command_units(user.team, ids, command);
    } else if(cmd == "team") {
        std::vector<std::string> params = format_args_arr(args);
        if(params.size() == 1) {
            int team = std::atoi(params[0].c_str());
            if(team >= 0 && team < 7) {
                user.team = team;
                send_sync(user, "team " + std::to_string(user.team));
                send_sync(user, "chat Joined team " + std::to_string(user.team));
                bool spawn_unit = true;
                for(auto& unit : world.units) {
                    if(unit.second.alive && unit.second.type == Unit::WORKER && unit.second.team == user.team) {
                        spawn_unit = false;
                    }
                }
                if(spawn_unit) {
                    world.local_create_worker(user.team, glm::vec2(0));
                    send_sync(user, "chat Unit spawned.");
                }
            } else {
                send_sync(user, "chat Bad team #");
            }
        } else {
            send_sync(user, "chat Invalid parameters");
        }
    } else if(cmd == "say") {
        if(user.authority > 0) {
            broadcast_sync("chat " + user.name + ": " + args);
        }
    } else if(cmd == "ip") {
        if(user.authority > 0) {
            std::vector<std::string> args_arr = format_args_arr(args);
            if(args_arr.size() == 3) {
                std::string name = args_arr[0];
                for(auto& userp : users) {
                    auto& u = userp.second;
                    if(u.name == name) {
                        std::string ip = args_arr[1];
                        int port = std::atoi(args_arr[2].c_str());
                        u.address = get_address(port, ip);
                        send_sync(user, "chat Ip for user " + name + " has been set to " + ip + ":" + std::to_string(port));
                        break;
                    }
                }
            }
        }
    } else {
        send_sync(user, "chat Command not found: " + cmd);
    }
}