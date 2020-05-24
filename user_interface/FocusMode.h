//
// Created by isaac on 5/9/2020.
//

#ifndef UNTITLED2_FOCUSMODE_H
#define UNTITLED2_FOCUSMODE_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <functional>

class FocusMode {
public:
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void update(double delta);
    virtual void onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos);
    virtual void onWindowSize(GLFWwindow* window, const glm::vec2& window_size);
    virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    virtual void onScroll(GLFWwindow* window, double x, double y);
    virtual void onChar(GLFWwindow* window, unsigned int codepoint);
    static FocusMode* const FOCUS_PREV;
};

class FocusNone : public FocusMode {
    void enable() override;
    void disable() override;
};
class FocusManager {
public:
    struct Edge {
        int key, action, mods;
        std::function<bool()> condition = [](){return true;};
    };

    FocusManager(std::vector<FocusMode*> modes, FocusMode* start);
    void add_link(FocusMode* a, FocusMode* b, Edge edge);
    void update(double delta);
    void onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos);
    void onWindowSize(GLFWwindow* window, const glm::vec2& window_size);
    void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onScroll(GLFWwindow* window, double x, double y);
    void onChar(GLFWwindow* window, unsigned int codepoint);
    void focus(FocusMode* mode);
    FocusMode* get_focus() const {
        return current;
    }
private:
    FocusMode* current;
    FocusMode* previous;
    std::vector<FocusMode*> focusModes;
    std::map<FocusMode*, std::vector<std::pair<Edge, FocusMode*>>> graph;
};


#endif //UNTITLED2_FOCUSMODE_H
