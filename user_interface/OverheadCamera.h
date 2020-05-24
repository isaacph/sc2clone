//
// Created by isaac on 5/6/2020.
//

#ifndef UNTITLED2_OVERHEADCAMERA_H
#define UNTITLED2_OVERHEADCAMERA_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "FocusMode.h"

class OverheadCamera : public FocusMode {
public:
    explicit OverheadCamera(glm::mat4& view, GLFWwindow* window);
    void onMousePosition(GLFWwindow* window, const glm::vec2& mouse_pos);
    void onWindowSize(GLFWwindow* window, const glm::vec2& window_size);
    void update(double delta);
    void enable();
    void disable();

    glm::vec3 camera_focus_point;
    float camera_focus_distance;
    glm::vec3 rotation;
    glm::vec3 camera_position;
    glm::mat4 &view;
    float camera_scroll_speed;
    const float SCREEN_EDGE_SCROLL_DISTANCE = 9; // pixels
    bool moved;
private:
    bool toggle = false;
    glm::vec2 mouse_position, last_mouse_position;
    glm::vec2 window_size;
    GLFWwindow* window;
};


#endif //UNTITLED2_OVERHEADCAMERA_H
