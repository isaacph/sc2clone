//
// Created by isaac on 3/27/2020.
//

#include "DragCamera.h"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>

DragCamera::DragCamera(GLFWwindow* window, glm::mat4 &view) : window(window), view(view) {}

void DragCamera::update() {
    if(mouse_drag) {
        double mouse_x, mouse_y;
        int window_width, window_height;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glfwGetWindowSize(window, &window_width, &window_height);
        glm::vec2 mouse_position(mouse_x, window_height - mouse_y);

        glm::vec2 mouse_drag_distance = mouse_position - mouse_drag_position;
        camera_position = camera_position_drag_start + mouse_drag_distance;
    }

    double zoom_exp = pow(2, zoom / 10.0);
    view = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(camera_position.x, camera_position.y, 0)), glm::vec3(zoom_exp, zoom_exp, 0));
    //view = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 0)), glm::vec3(camera_position.x, camera_position.y, 0));
}

void DragCamera::onResize(glm::vec2 size) {
    window_size = size;
}

void DragCamera::onMouse(int button, int action, int mods) {
    double mouse_x, mouse_y;
    int window_width, window_height;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwGetWindowSize(window, &window_width, &window_height);
    glm::vec2 mouse_position(mouse_x, window_height - mouse_y);
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
            mouse_drag = true;
            mouse_drag_position = mouse_position;
            camera_position_drag_start = camera_position;
        } else if(action == GLFW_RELEASE) {
            mouse_drag = false;
        }
    }
}

void DragCamera::onScroll(double x, double y) {
    zoom += y;
}