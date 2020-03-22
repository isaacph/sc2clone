//
// Created by isaac on 12/18/2019.
//

#ifndef UNTITLED2_WINDOW_H
#define UNTITLED2_WINDOW_H

#include <memory>
#include "../observable.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct WindowProperties;
void initGLFW();
std::unique_ptr<WindowProperties> createWindow(const std::string& title, int width, int height);
struct WindowProperties {
    GLFWwindow* window;
    int width;
    int height;
    std::string title;

    struct Key {
        int key, scancode, action, mods;
    };
    struct MouseButton {
        int button, action, mods;
    };
    using Char = unsigned int;

    struct Callbacks {
    private:
        EventEmitter<glm::ivec2> sizeChangeEmitter;
        EventEmitter<unit> windowCloseEmitter;
        EventEmitter<Key> keyEmitter;
        EventEmitter<MouseButton> mouseButtonEmitter;
        EventEmitter<Char> characterEmitter;
        EventEmitter<glm::dvec2> cursorPosEmitter;
        EventEmitter<glm::dvec2> scrollEmitter;
    public:
        Observer<glm::ivec2> sizeChange{sizeChangeEmitter};
        Observer<unit> windowClose{windowCloseEmitter};
        Observer<Key> key{keyEmitter};
        Observer<MouseButton> mouseButton{mouseButtonEmitter};
        Observer<Char> character{characterEmitter};
        Observer<glm::dvec2> cursorPos{cursorPosEmitter};
        Observer<glm::dvec2> scroll{scrollEmitter};
        friend WindowProperties;
    } callbacks;

private:
    WindowProperties(GLFWwindow* window_in, std::string title_in, int width_in, int height_in);
    friend std::unique_ptr<WindowProperties> createWindow(const std::string& title, int width, int height);
    void initCallbacks();
};

#endif //UNTITLED2_WINDOW_H
