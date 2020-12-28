//
// Created by isaac on 5/9/2020.
//

#include "Game.h"
#include "SelectMath.h"
#include <chrono>

GLFWwindow* initWindow(int width, int height) {
    glfwInit();

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Render Engine",
                                          nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // enable VSYNCtest
    glfwSwapInterval(1);

    // init GL
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 0);
    errorCheckGl("draw pre-init");

    return window;
}


Game::Game() : window(initWindow(800, 600)), flyCamera(view, window), overheadCamera(view, window),
focusManager({&noFocus, &flyCamera, &overheadCamera, &chatbox}, &noFocus),
uniqueIDGenerator(), clientID(uniqueIDGenerator.generate()) {
    initCommands();
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int new_width, int new_height) {
        ((Game*) glfwGetWindowUserPointer(win))->windowSize(new_width, new_height);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        ((Game*) glfwGetWindowUserPointer(win))->onMouse(button, action, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
        ((Game*) glfwGetWindowUserPointer(win))->onCursorPos(x, y);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* win, double x, double y) {
        ((Game*) glfwGetWindowUserPointer(win))->onScroll(x, y);
    });
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        ((Game*) glfwGetWindowUserPointer(win))->onKey(key, scancode, action, mods);
    });
    glfwSetCharCallback(window, [](GLFWwindow* win, unsigned int codepoint) {
        ((Game*) glfwGetWindowUserPointer(win))->onChar(codepoint);
    });
    focusManager.add_link(&noFocus, &chatbox, {GLFW_KEY_ENTER, GLFW_PRESS, 0});
    focusManager.add_link(&overheadCamera, &chatbox, {GLFW_KEY_ENTER, GLFW_PRESS, 0});
    focusManager.add_link(&flyCamera, &chatbox, {GLFW_KEY_ENTER, GLFW_PRESS, 0});
    focusManager.add_link(&chatbox, FocusMode::FOCUS_PREV, {GLFW_KEY_ENTER, GLFW_PRESS, 0, [&](){
        return chatbox.typed.empty();
    }});
    focusManager.add_link(&chatbox, &noFocus, {GLFW_KEY_ESCAPE, GLFW_PRESS, 0});
    focusManager.add_link(&noFocus, &overheadCamera, {GLFW_KEY_G, GLFW_PRESS, 0});
    focusManager.add_link(&overheadCamera, &noFocus, {GLFW_KEY_G, GLFW_PRESS, 0});
    focusManager.add_link(&overheadCamera, &noFocus, {GLFW_KEY_ESCAPE, GLFW_PRESS, 0});
    focusManager.add_link(&overheadCamera, &flyCamera, {GLFW_KEY_V, GLFW_PRESS, 0});
    focusManager.add_link(&noFocus, &flyCamera, {GLFW_KEY_V, GLFW_PRESS, 0});
    focusManager.add_link(&flyCamera, &noFocus, {GLFW_KEY_V, GLFW_PRESS, 0});
    focusManager.add_link(&flyCamera, &noFocus, {GLFW_KEY_ESCAPE, GLFW_PRESS, 0});
    focusManager.add_link(&flyCamera, &overheadCamera, {GLFW_KEY_G, GLFW_PRESS, 0});
    focusManager.add_link(&noFocus, &flyCamera, {GLFW_KEY_V, GLFW_PRESS, 0});
    overheadCamera.camera_focus_point = glm::vec3(0, 0, 0);

    windowSize(800, 600);
}

void Game::windowSize(int new_width, int new_height) {
    if(new_width <= 0 || new_height <= 0) {
        return;
    }
    glViewport(0, 0, new_width, new_height);
    ortho = glm::ortho<float>(0.0f, new_width, 0.0f, new_height, 0.0f, 1.0f);
    persp = glm::perspective(60.0f * 3.14159265359f / 180.0f, (float) new_width / (float) new_height, 0.1f, 1000.0f);
    width = new_width;
    height = new_height;
    overheadCamera.onWindowSize(window, glm::vec2(new_width, new_height));
}

void Game::onMouse(int button, int action, int mods) {
    if(focusManager.get_focus() == &overheadCamera) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            left_click = action > 0;
            if (focusManager.get_focus() == &overheadCamera) {
                if (action == GLFW_PRESS) {
                    bool foundUnit = false;
                    int selected = findMouseOverUnits(foundUnit, !attack);
                    if(!attack) {
                        dragStart = mouse;
                        world.unitsSelected = {};
                        if (foundUnit) {
                            world.unitsSelected.insert(selected);
                        }
                    } else {
                        UnitCommand command = {UnitCommand::NONE, mouseWorld, selected};
                        if(foundUnit) {
                            command.type = UnitCommand::ATTACK;
                        } else {
                            command.type = UnitCommand::ATTACK_MOVE;
                        }
                        world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                                  command);
                    }
                }
                if (action == GLFW_RELEASE) {
                    if (!attack && drag) {
                        drag = false;
                        findMouseDragUnits();
                    }
                    attack = false;
                }
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            std::vector<int> ids;
            UnitCommand command = {UnitCommand::MOVE, mouseWorld};
            world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                      command);
        }
    }

}

void Game::onCursorPos(double x, double y) {
    if(focusManager.get_focus() == &overheadCamera && left_click && !attack) {
        drag = true;
    }
}

void Game::onScroll(double x, double y) {
    focusManager.onScroll(window, x, y);
}

void Game::onKey(int key, int scancode, int action, int mods) {
    focusManager.onKey(window, key, scancode, action, mods);
    if(key == GLFW_KEY_S && action == GLFW_PRESS && focusManager.get_focus() == &overheadCamera) {
        world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                  {UnitCommand::NONE});
    } else if(key == GLFW_KEY_W && action == GLFW_PRESS && focusManager.get_focus() == &overheadCamera) {
        world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                  {UnitCommand::MAKE_WORKER});
    } else if(key == GLFW_KEY_T && action == GLFW_PRESS && focusManager.get_focus() == &overheadCamera) {
        world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                  {UnitCommand::WORKER_TRANSFORM});
    } else if(key == GLFW_KEY_E && action == GLFW_PRESS && focusManager.get_focus() == &overheadCamera) {
        world.local_command_units(team, std::vector<int>(world.unitsSelected.begin(), world.unitsSelected.end()),
                                  {UnitCommand::MAKE_SOLDIER});
    } else if(key == GLFW_KEY_A && action == GLFW_PRESS && focusManager.get_focus() == &overheadCamera) {
        attack = true;
    }
}

void Game::onChar(unsigned int codepoint) {
    focusManager.onChar(window, codepoint);
}

glm::vec3 Game::screenSpaceToOverheadDir(glm::vec2 screen) const {
    glm::vec4 screen_mouse((screen.x / (float) width * 2 - 1), (screen.y / (float) height * 2 - 1), 1.0f, 1);
    glm::vec4 near_point = screen_mouse;
    near_point.z = 0;
    screen_mouse = glm::inverse(persp * view) * screen_mouse;
    near_point = glm::inverse(persp * view) * near_point;
    glm::vec3 n = glm::vec3(near_point.x, near_point.y, near_point.z) / near_point.w;
    glm::vec3 f = glm::vec3(screen_mouse.x, screen_mouse.y, screen_mouse.z) / screen_mouse.w;
    return -glm::normalize(n - f);
}

void Game::run() {
    networking.initSocket(3801, true);
    serverAddress = getNetworkingAddress(3800, "127.0.0.1");

    Graphics::Text text = graphics.initText("arial", 20);
    text.text = "Isaac Huffman";
    text.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    text.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));

    Graphics::Rectangle rect = graphics.initRectangle();
    rect.color = glm::vec4(1, 1, 1, 0.2f);

    Graphics::PlainModel model2 = graphics.initPlainModel("res/cube.obj");
    model2.baseLightRadius = 1.0f;
    model2.position = glm::vec3(2, 0.5f, 2);
    model2.rotation = glm::mat4(1.0f);
    model2.color = glm::vec4(1.0f);
    model2.scale = glm::vec3(0.1f);

    Graphics::PlainTextureModel plane1 = graphics.initPlainTextureModel("res/plane.obj");
    plane1.image = "res/idk.png";
    plane1.baseLightRadius = 1.0f;
    plane1.position = glm::vec3(0, 0.01, 0);
    plane1.scale = glm::vec3(4);
    plane1.rotation = glm::mat4(1.0f);
    plane1.color = glm::vec4(1, 1, 1, 1);

    Graphics::PlainModel plane = graphics.initPlainModel("res/plane.obj");
    plane.position = glm::vec3(0, 0, 0);
    plane.scale = glm::vec3(80);
    plane.color = glm::vec4(0.5, 0.5, 0.5, 1);

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    Graphics::Light sun = graphics.initLight();
    sun.strength = 100001;
    sun.position = glm::vec3(80, 100, 50);
    sun.color = glm::vec3(1, 1, 1);
    graphics.ambientColor = glm::vec3(0.7f);

    workerModel.baseLightRadius = 1.0f;
    workerModel.color = glm::vec4(1, 0, 0, 1);
    workerModel.scale = glm::vec3(1.0f);
    projectileModel.baseLightRadius = 1.0f;

    Graphics::PlainTextureModel selectionModel = graphics.initPlainTextureModel("res/plane.obj");
    selectionModel.baseLightRadius = 1.0f;
    selectionModel.image = "res/idk.png";
    selectionModel.rotation = glm::mat4(1.0f);
    selectionModel.color = glm::vec4(1, 1, 1, 1);

    world.local_create_worker(0, glm::vec2(0, 0));

    std::map<int, glm::vec4> teamColor = {
            {0, glm::vec4(1, 1, 1, 1)},
            {1, glm::vec4(1, 0, 0, 1)},
            {2, glm::vec4(0, 1, 0, 1)},
            {3, glm::vec4(1, 1, 0, 1)},
            {4, glm::vec4(0, 0, 1, 1)},
            {5, glm::vec4(1, 0, 1, 1)},
            {6, glm::vec4(0, 1, 1, 1)},
            {7, glm::vec4(1, 1, 1, 1)}
    };

    currentTime = glfwGetTime();
    double lastTime = currentTime;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        currentTime = glfwGetTime();
//        double delta = currentTime - lastTime;
        double delta = std::min<double>(currentTime - lastTime, 0.1);
        lastTime = currentTime;

        double m[2];
        glfwGetCursorPos(window, m, m + 1);
        mouse = {m[0], height - m[1]};

        focusManager.onMousePosition(window, mouse);
        focusManager.update(delta);
        if(focusManager.get_focus() == &overheadCamera) {
            glm::vec3 dir = screenSpaceToOverheadDir(mouse);
            glm::vec3 start = overheadCamera.camera_position;
            glm::vec3 plane_start = glm::vec3(0, 0, 0);
            glm::vec3 plane_norm = glm::vec3(0, 1, 0);
            glm::vec3 mouse3d = proj_line_plane(start, dir, plane_start, plane_norm);
            mouseWorld = glm::vec2(mouse3d.x, mouse3d.z);// - glm::vec2(overheadCamera.camera_position.x, overheadCamera.camera_position.z);
            mouseDir = dir;
        }

        while(!chatbox.user_input.empty()) {
            std::string input = chatbox.user_input.front();
            chatbox.user_input.erase(chatbox.user_input.begin());
            if(!input.empty() && input[0] == '/') {
                command(input.substr(1, input.size() - 1));
            } else {
                command("send say " + input);
            }
        }

        auto packets = networking.pollAndSend();
        for(std::unique_ptr<Packet>& packet : packets) {
            std::cout << packet->message << "\n";
            command(packet->message);
        }

        world.update(delta);

        WorldEvent event;
        while(world.has_next_event()) {
            event = world.next_event();
            if(!world.root && event.type == WorldEvent::COMMAND) {
                send("command " + world.unit_command_string(event.command, event.command_ids));
            } else if(event.type == WorldEvent::UNIT_UPDATE) {
                // do nothing
            }
        }

        glEnable(GL_DEPTH_TEST);

        plane1.setup = persp * view;
        plane1.position = glm::vec3(mouseWorld.x, 0.01, mouseWorld.y);

        plane.setup = persp * view;
        plane.draw();

        selectionModel.setup = persp * view;
        for(const auto& id : world.unitsSelected) {
            const auto p = world.units.find(id);
            if(p != world.units.end() && p->second.alive) {
                const auto& unit = p->second;
                selectionModel.position = glm::vec3(unit.position.x, 0.01, unit.position.y);
                selectionModel.scale = glm::vec3(unit.radius * 2);
                selectionModel.draw();
            }
        }

        for(const auto& p : world.units) {
            const auto& unit = p.second;
            if(unit.alive) {
                if (unit.type == Unit::WORKER) {
                    auto &model = (Graphics::PlainModel &) *getUnitModel(unit);
                    model.setup = persp * view;
                    model.color = teamColor[unit.team];
                    model.draw();
                } else if (unit.type == Unit::BARRACKS) {
                    auto &model = (Graphics::PlainModel &) *getUnitModel(unit);
                    model.setup = persp * view;
                    model.color = teamColor[unit.team];
                    model.draw();
                } else if(unit.type == Unit::SOLDIER) {
                    auto &model = (Graphics::PlainModel&) *getUnitModel(unit);
                    model.setup = persp * view;
                    model.color = teamColor[unit.team];
                    model.draw();
                }
            }
        }
        for(const auto& p : world.projectiles) {
            const auto& projectile = p.second;
            if(projectile.alive) {
                Graphics::PlainModel& model = projectileModel;
                model.setup = persp * view;
                model.color = glm::vec4(1, 1, 1, 1);
                model.position = glm::vec3(projectile.position.x, 1, projectile.position.y);
                model.scale = glm::vec3(0.2f);
                model.rotation = glm::mat4(1.0f);
                model.draw();
            }
        }
        glDisable(GL_DEPTH_TEST);

        if(focusManager.get_focus() == &flyCamera) {
            rect.matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(width / 2, height / 2, 0)),
                                     glm::vec3(10, 10, 0));
            rect.setup = ortho;
            rect.draw();
        }

        if(focusManager.get_focus() == &overheadCamera && drag) {
            glm::vec2 center = (dragStart + mouse) * 1.0f / 2.0f;
            glm::vec2 size = mouse - dragStart;
            rect.matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0)),
                                     glm::vec3(abs(size.x), abs(size.y), 0));
            rect.setup = ortho;
            rect.draw();
        }

        chatbox.draw(delta, ortho);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    networking.closeSocket();
}

void Game::send(const std::string &msg) {
    networking.send(std::make_unique<Packet>(Packet(clientID.to_string() + " " + msg, serverAddress)));
}

Graphics::Model* Game::getUnitModel(const Unit& unit) {
    if (unit.type == Unit::WORKER) {
        workerModel.position = glm::vec3(unit.position.x, 1, unit.position.y);
        workerModel.scale = glm::vec3(1);
        workerModel.rotation = glm::rotate(glm::mat4(1.0f), unit.direction, glm::vec3(0, 1, 0));
        if(unit.command.type == UnitCommand::WORKER_TRANSFORM) {
            workerModel.scale = glm::vec3((5 - unit.load_time) / 5.0f * 4);
            workerModel.position = glm::vec3(unit.position.x, (5 - unit.load_time) / 5.0f * 4, unit.position.y);
            workerModel.rotation = glm::rotate(glm::mat4(1.0f), 3.1415926536f / 4.0f, glm::vec3(0, 1, 0));
        }
        return &workerModel;
    } else if(unit.type == Unit::BARRACKS) {
        barracksModel.position = glm::vec3(unit.position.x, 4, unit.position.y);
        barracksModel.scale = glm::vec3(4);
        barracksModel.rotation = glm::rotate(glm::mat4(1.0f), unit.direction, glm::vec3(0, 1, 0));
        if(unit.load_time > 0) {
            barracksModel.scale = glm::vec3(4 + unit.load_time / 5.0f * 2.0f);
            barracksModel.position = barracksModel.position + glm::vec3(0, unit.load_time / 5.0f * 2.0f, 0);
        }
        return &barracksModel;
    } else if(unit.type == Unit::SOLDIER) {
        soldierModel.position = glm::vec3(unit.position.x, 1.4f, unit.position.y);
        soldierModel.scale = glm::vec3(1.4f);
        soldierModel.rotation = glm::rotate(glm::mat4(1.0f), unit.direction, glm::vec3(0, 1, 0));
        return &soldierModel;
    }
    return nullptr;
}

Game::~Game() {
    glfwTerminate();
}