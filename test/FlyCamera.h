//
// Created by isaac on 4/6/2020.
//

#ifndef UNTITLED2_FLYCAMERA_H
#define UNTITLED2_FLYCAMERA_H
#include <glm/glm.hpp>

class FlyCamera {
public:
    explicit FlyCamera(glm::mat4& view);
    void onMousePosition(const glm::vec2& mouse_pos);
    void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void update(double delta);
    void enable(GLFWwindow* window);
    void disable(GLFWwindow* window);

    glm::vec3 center;
    glm::vec3 rotation;
    glm::vec3 dir;
    glm::vec2 mouse_position, last_mouse_position;
    glm::mat4 &view;
    float move_speed;
private:
    bool key_w = false, key_s = false, key_a = false, key_d = false, key_space = false, key_shift = false;
    bool toggle = false;
};

#endif //UNTITLED2_FLYCAMERA_H
