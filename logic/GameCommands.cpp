//
// Created by isaac on 5/10/2020.
//

#include "Game.h"
#include <chrono>

namespace GameCommands {
    void test(Game& game, const std::string& args_temp) {
        std::string args = args_temp;
        if(args.size() > 0) {
            if(args[0] == 'h') {
                auto v = format_args_arr(args);
                args = "";
                for(int i = 1; i < v.size(); ++i) {
                    args += v[i];
                }
            }
            game.chatbox.println(std::string("Test works: ") + args);
        } else {
            game.chatbox.println("Test works!");
        }
    }
    void chat(Game& game, const std::string& args) {
        game.chatbox.println(args);
    }
    void overheadpos(Game& game, const std::string& args) {
        if(args.size() == 0) {
            game.overheadCamera.camera_focus_point = glm::vec3(0);
        } else if(args.size() > 0) {
            std::vector<std::string> args_arr = format_args_arr(args);
            if(args_arr.size() == 2) {
                std::stringstream sstream;
                float x, z;
                sstream << args_arr[0];
                sstream >> x;
                sstream.clear();
                sstream << args_arr[1];
                sstream >> z;
                game.overheadCamera.camera_focus_point = glm::vec3(x, 0, z);
            } else {
                game.chatbox.println("Invalid number of arguments");
            }
        }
    }
    void send(Game& game, const std::string& args) {
        game.send(args);
    }
    void ping(Game& game, const std::string& args) {
        if(!game.pinging) {
            game.pinging = true;
            game.pingStart = game.currentTime;
            game.send("ping");
        } else {
            game.pinging = false;
            game.chatbox.println("Ping is " + std::to_string(game.currentTime - game.pingStart) + " seconds");
        }
    }
    void root(Game& game, const std::string& args) {
        if(args == "0") {
            game.world.root = false;
            game.chatbox.println("Client is subservient");
        } else if(args == "1") {
            game.world.root = true;
            game.chatbox.println("Client is master");
        } else {
            game.chatbox.println("Invalid root cmd arguments: " + args);
        }
    }
    void team_num(Game& game, const std::string& args) {
        std::vector<std::string> params = format_args_arr(args);
        if(params.size() == 1) {
            int team = std::atoi(params[0].c_str());
            if(team >= 0 && team < 7) {
                game.team = team;
            }
        }
    }
    void clear_cmd(Game& game, const std::string& args) {
        game.world.units = {};
        game.team = 0;
    }
    void world_cmd(Game& game, const std::string& args) {
        game.world.server_command(args);
    }
    void host(Game& game, const std::string& args) {
//        game.server_address = get_address(3800, args.c_str());
        game.serverAddress = getNetworkingAddress(3800, args);
        game.chatbox.println("Ip set to " + args + ":3800");
    }
}

void Game::initCommands() {
    using namespace GameCommands;
    pingStart = -1;
    pinging = false;
    commands = {
            {"test", test},
            {"chat", chat},
            {"overheadpos", overheadpos},
            {"send", GameCommands::send},
            {"ping", ping},
            {"root", root},
            {"team", team_num},
            {"clear", clear_cmd},
            {"world", world_cmd},
            {"host", host}
    };
}

void Game::command(std::string input) {
    std::string cmd = format_cmd(input);
    std::string args = format_args(input);
    auto pair = commands.find(format_cmd(input));
    if(pair == commands.end()) {
        chatbox.println(std::string("Command \"") + format_cmd(input) + std::string("\" was not found."));
    } else {
        pair->second(*this, args);
    }
}