//
// Created by isaac on 5/9/2020.
//

#ifndef UNTITLED2_GAME_H
#define UNTITLED2_GAME_H

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../graphics/draw.h"
#include "../user_interface/FlyCamera.h"
#include "../user_interface/OverheadCamera.h"
#include "../user_interface/Chatbox.h"
#include "../math_util.h"
#include <pthread.h>
#include "../server/shared.h"
#include "../server/socket_init.h"
#include <sstream>
#include <windows.h>
#include <winuser.h>
#include <windef.h>
#include "../logic/World.h"
#include "../user_interface/FocusMode.h"
#include "UniqueID.h"

struct Game;

typedef void (*GameCommand)(Game& game, const std::string& args);

struct Game {
    GLFWwindow* window;
    Graphics graphics;
    glm::mat4 ortho = glm::mat4(1.0f), view = glm::mat4(1.0f), persp = glm::mat4(1.0f);
    int width = 0, height = 0;
    glm::vec2 mouse;
    glm::vec2 mouse_world;
    glm::vec3 mouse_dir;

    FlyCamera flyCamera;
    OverheadCamera overheadCamera;
    Chatbox chatbox = Chatbox(graphics.initText("arial", 30), 20, 30, 800,
            glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0)));
    FocusNone noFocus;
    FocusManager focusManager;

    glm::vec2 drag_start;
    bool left_click = false;
    bool drag = false;
    bool attack = false;

    Graphics::PlainModel workerModel = graphics.initPlainModel("res/cube.obj");
    Graphics::PlainModel barracksModel = graphics.initPlainModel("res/cube.obj");
    Graphics::PlainModel soldierModel = graphics.initPlainModel("res/cube.obj");
    Graphics::PlainModel projectileModel = graphics.initPlainModel("res/cube.obj");

    World world;
    int team = 0;

    UniqueIDGenerator uniqueIDGenerator;
    UniqueID clientID;

    Shared communication;
    sockaddr_in server_address = get_address(3800, "127.0.0.1");
    int ping_timer;
    bool ping_started;

    std::map<std::string, GameCommand> commands;

    explicit Game(GLFWwindow* window);
    void windowSize(int new_width, int new_height);
    void onMouse(int button, int action, int mods);
    void onCursorPos(double x, double y);
    void onScroll(double x, double y);
    void onKey(int key, int scancode, int action, int mods);
    void onChar(unsigned int codepoint);
    glm::vec3 screen_space_to_overhead_dir(glm::vec2 screen) const;
    void run();
    void command(std::string command);
    void initCommands();
    void send_async(const std::string& msg);
    void send_sync(const std::string& msg);
    int select_units_click(bool& foundUnit, bool sameTeam);
    void select_units_drag();

    Graphics::Model* get_model_unit(const Unit& unit);
};


#endif //UNTITLED2_GAME_H
