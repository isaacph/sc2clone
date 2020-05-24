//
// Created by isaac on 5/9/2020.
//

#include "FocusMode.h"

FocusMode* const FocusMode::FOCUS_PREV = nullptr;
void FocusMode::update(double delta) {}
void FocusMode::onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos) {}
void FocusMode::onWindowSize(GLFWwindow* window, const glm::vec2& window_size) {}
void FocusMode::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {}
void FocusMode::onScroll(GLFWwindow* window, double x, double y) {}
void FocusMode::onChar(GLFWwindow* window, unsigned int codepoint) {}
void FocusNone::enable() {}
void FocusNone::disable() {}
FocusManager::FocusManager(std::vector<FocusMode *> modes, FocusMode* start) : focusModes(modes), current(start), previous(modes[0]) {
    for(auto* mode : modes) {
        mode->disable();
    }
    current->enable();
}
void FocusManager::add_link(FocusMode* a, FocusMode* b, Edge edge) {
    graph[a].push_back({edge, b});
}
void FocusManager::update(double delta) {
    current->update(delta);
}
void FocusManager::onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos) {
    current->onMousePosition(window, mouse_pos);
}
void FocusManager::onWindowSize(GLFWwindow* window, const glm::vec2& window_size) {
    current->onWindowSize(window, window_size);
}
void FocusManager::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for(auto pair : graph[current]) {
        Edge& edge = pair.first;
        if(edge.key == key && edge.action == action && edge.mods == mods && edge.condition()) {
            focus(pair.second);
        }
    }
    current->onKey(window, key, scancode, action, mods);
}
void FocusManager::onScroll(GLFWwindow* window, double x, double y) {
    current->onScroll(window, x, y);
}
void FocusManager::onChar(GLFWwindow* window, unsigned int codepoint) {
    current->onChar(window, codepoint);
}
void FocusManager::focus(FocusMode *mode) {
    current->disable();
    if(mode == FocusMode::FOCUS_PREV) {
        mode = current;
        current = previous;
        previous = mode;
    } else {
        previous = current;
        current = mode;
    }
    current->enable();
}