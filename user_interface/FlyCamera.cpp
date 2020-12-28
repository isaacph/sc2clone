//
// Created by isaac on 4/7/2020.
//

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include "FlyCamera.h"
#include <iostream>
#define MY_PI 3.1415926535979

FlyCamera::FlyCamera(glm::mat4 &view, GLFWwindow* window) : view(view), move_speed(4), window(window) {}

void FlyCamera::onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos) {
    last_mouse_position = mouse_position;
    mouse_position = mouse_pos;
}

void FlyCamera::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(toggle) {
        if (key == GLFW_KEY_W) {
            key_w = action >= GLFW_PRESS;
        } else if (key == GLFW_KEY_S) {
            key_s = action >= GLFW_PRESS;
        } else if (key == GLFW_KEY_A) {
            key_a = action >= GLFW_PRESS;
        } else if (key == GLFW_KEY_D) {
            key_d = action >= GLFW_PRESS;
        } else if (key == GLFW_KEY_SPACE) {
            key_space = action >= GLFW_PRESS;
        } else if (key == GLFW_KEY_LEFT_SHIFT) {
            key_shift = action >= GLFW_PRESS;
        }
    }
}

void FlyCamera::enable() {
    toggle = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    rotation.x = 0;
}

void FlyCamera::disable() {
    toggle = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void FlyCamera::update(double delta) {
    if(toggle) {
        glm::vec2 push = (mouse_position - last_mouse_position);
        push.x /= 200.0f;
        push.y /= 200.0f;
        rotation.x -= push.y;
        rotation.y += push.x;
        rotation = glm::vec3(std::max<float>(std::min<float>(MY_PI / 2.0f, rotation.x), -MY_PI / 2.0f),
                rotation.y, 0);

        glm::vec4 move = glm::vec4(0, 0, 0, 1);
        move.z = key_w - key_s;
        move.x = key_a - key_d;
        move.y = key_shift - key_space;
        move = glm::rotate(glm::mat4(1.0f), -rotation.y, glm::vec3(0, 1, 0)) * move;
        move *= delta * move_speed;
        center.x += -move.x;
        center.y += -move.y;
        center.z += -move.z;

        dir = glm::rotate(glm::mat4(1.0f), -rotation.y, glm::vec3(0, 1, 0)) *
                glm::rotate(glm::mat4(1.0f), -rotation.x, glm::vec3(1, 0, 0)) * glm::vec4(0, 0, 1, 1);

        view = glm::mat4(1.0f);
        view = glm::rotate(view, rotation.x, glm::vec3(1, 0, 0));
        view = glm::rotate(view, rotation.y, glm::vec3(0, 1, 0));
        view = view * glm::translate(glm::mat4(1.0f), -center);
    }
}