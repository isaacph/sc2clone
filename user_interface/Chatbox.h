//
// Created by isaac on 5/9/2020.
//

#ifndef UNTITLED2_CHATBOX_H
#define UNTITLED2_CHATBOX_H
#include "../graphics/draw.h"
#include "FocusMode.h"
#include <iostream>

class Chatbox : public FocusMode {
public:
    Chatbox(Graphics::Text text, int log_length, int space, int pixel_width, glm::mat4 origin);
    void println(const std::string& s);
    void draw(double delta, const glm::mat4& setup);
    void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onChar(GLFWwindow *window, unsigned int codepoint);
    void enable();
    void disable();
    std::string typed;
    std::vector<std::string> user_input;
private:
    Graphics::Text text;
    std::vector<std::string> log;
    int log_length;
    int space;
    int pixel_width;
    glm::mat4 origin;
    double bar_counter = 0;
    bool typing = false;
    double inactivity;
    const int MAX_LOG = 40;
};


#endif //UNTITLED2_CHATBOX_H
