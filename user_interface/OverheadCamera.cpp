//
// Created by isaac on 5/6/2020.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "OverheadCamera.h"

OverheadCamera::OverheadCamera(glm::mat4& view, GLFWwindow* window) : view(view), window(window) {
    rotation = glm::vec3(3.14159265359f / 4 * 1.2f, 3.14159265359f * 0, 0);
    camera_focus_distance = 32;
    camera_scroll_speed = 16;
}

void OverheadCamera::onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos) {
    if(toggle) {
        last_mouse_position = mouse_position;
        mouse_position = mouse_pos;
        mouse_position.x = std::min<float>(std::max<float>(mouse_position.x, 4), window_size.x - 4);
        mouse_position.y = std::min<float>(std::max<float>(mouse_position.y, 4), window_size.y - 4);
        if(mouse_pos.x != mouse_position.x || mouse_pos.y != mouse_position.y) {
            glfwSetCursorPos(window, mouse_position.x, window_size.y - mouse_position.y);
        }
    }
}

void OverheadCamera::onWindowSize(GLFWwindow* window, const glm::vec2 &size) {
    if(toggle) {
        glfwSetWindowSize(window, window_size.x, window_size.y);
    } else {
        window_size = size;
    }
}

void OverheadCamera::update(double delta) {
    if(toggle) {
        moved = false;
        float edge_down_dist = abs(mouse_position.y - 0);
        float edge_up_dist = abs(mouse_position.y - window_size.y);
        float edge_left_dist = abs(mouse_position.x - 0);
        float edge_right_dist = abs(mouse_position.x - window_size.x);
        glm::vec3 delta_pos(0);
        if(edge_down_dist < SCREEN_EDGE_SCROLL_DISTANCE) {
            delta_pos.z += 1;
            moved = true;
        }
        if(edge_up_dist < SCREEN_EDGE_SCROLL_DISTANCE) {
            delta_pos.z += -1;
            moved = true;
        }
        if(edge_left_dist < SCREEN_EDGE_SCROLL_DISTANCE) {
            delta_pos.x += -1;
            moved = true;
        }
        if(edge_right_dist < SCREEN_EDGE_SCROLL_DISTANCE) {
            delta_pos.x += 1;
            moved = true;
        }
        delta_pos *= delta * camera_scroll_speed;
        camera_focus_point += delta_pos;
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0, 0, -camera_focus_distance));
        view = glm::rotate(view, rotation.x, glm::vec3(1, 0, 0));
        view = glm::rotate(view, rotation.y, glm::vec3(0, 1, 0));
        //view = glm::rotate(view, rotation.z, glm::vec3(0, 0, 1));
        view = glm::translate(view, -camera_focus_point);
        glm::vec4 c = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
        camera_position = glm::vec3(c.x, c.y, c.z);
    }
}

void OverheadCamera::enable() {
    toggle = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OverheadCamera::disable() {
    toggle = false;
}
