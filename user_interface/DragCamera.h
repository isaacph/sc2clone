//
// Created by isaac on 3/27/2020.
//

#ifndef UNTITLED2_DRAGCAMERA_H
#define UNTITLED2_DRAGCAMERA_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class DragCamera {
public:
    DragCamera(GLFWwindow* window, glm::mat4 &view);
    void update();
    void onResize(glm::vec2 size);
    void onMouse(int button, int action, int mods);
    void onScroll(double x, double y);
    ~DragCamera() = default;
private:
    GLFWwindow* window;
    glm::mat4 &view;
    glm::vec2 window_size;
    glm::vec2 camera_position;
    glm::vec2 camera_position_drag_start;
    glm::vec2 mouse_drag_position;
    bool mouse_drag = false;
    double zoom = 0;
};


#endif //UNTITLED2_DRAGCAMERA_H
