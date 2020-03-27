//
// Created by isaac on 11/11/2019.
//

#include "Box.h"
#include <limits>

Box::Box(glm::vec2 position, glm::vec2 scale) : position(position), scale(scale), velocity(0) {

}

MovingShadow Box::getShadowX() const {
    return {{position.x - scale.x / 2, position.x + scale.x / 2},
            {velocity.x, velocity.x}};
}

MovingShadow Box::getShadowY() const {
    return {{position.y - scale.y / 2, position.y + scale.y / 2},
            {velocity.y, velocity.y}};
}

MovingShadow Box::getShadowAxis(const glm::vec2 &dir) const {
    const glm::vec2 center = position;
    const glm::vec2 rel_corners[] = {
            {+0.5f, +0.5f},
            {-0.5f, +0.5f},
            {-0.5f, -0.5f},
            {+0.5f, -0.5f}
    };
    auto axis_max = -DBL_MAX, axis_min = DBL_MAX;
    for(auto rel : rel_corners) {
        glm::vec2 corner = rel * scale + center;
        double val = glm::dot(dir, corner);
        axis_max = std::max(axis_max, val);
        axis_min = std::min(axis_min, val);
    }
    double axis_vel = glm::dot(dir, velocity);

    return {{axis_min, axis_max}, {axis_vel, axis_vel}};
}

std::set<SortVec> Box::getAxes() const {
    return {{{0, 1}}, {{1, 0}}};
}

glm::mat4 Box::toMatrix() const {
    return glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0)),
                      glm::vec3(scale.x, scale.y, 0));
}