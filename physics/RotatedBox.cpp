//
// Created by isaac on 11/11/2019.
//

#include "RotatedBox.h"

RotatedBox::RotatedBox(glm::vec2 position, glm::vec2 scale, double rotation) : Box(position, scale),
rotation(rotation) {

}

glm::mat4 RotatedBox::toMatrix() const {
    return glm::scale(
            glm::rotate<float>(
                    glm::translate(
                            glm::mat4(1.0f), glm::vec3(position.x, position.y, 0)
                            ), rotation, glm::vec3(0, 0, 1)
                    ), glm::vec3(scale.x, scale.y, 0)
            );
}

MovingShadow RotatedBox::getShadowX() const {
    return getShadowAxis(glm::vec2(1, 0));
}

MovingShadow RotatedBox::getShadowY() const {
    return getShadowAxis(glm::vec2(0, 1));
}

std::set<SortVec> RotatedBox::getAxes() const {
    return {{glm::vec2(cos(rotation + (float) 0 * 3.1415926536f / 2),
                            sin(rotation + (float) 0 * 3.1415926536f / 2))},
            {glm::vec2(cos(rotation + (float) 1 * 3.1415926536f / 2),
                       sin(rotation + (float) 1 * 3.1415926536f / 2))}};
}

MovingShadow RotatedBox::getShadowAxis(const glm::vec2& dir) const {
    const glm::vec2 center = position;
    const glm::vec2 horiz = glm::vec2{cos(rotation), sin(rotation)} * scale.x / 2;
    const glm::vec2 vert = glm::vec2{cos(rotation + 3.1415926536f / 2),
                            sin(rotation + 3.1415926536f / 2)} * scale.y / 2;
    const glm::vec2 rel_corners[] = {
        horiz + vert,
        horiz - vert,
        -horiz + vert,
        -horiz - vert
    };
    auto axis_max = -DBL_MAX, axis_min = DBL_MAX;
    for(auto rel : rel_corners) {
        glm::vec2 corner = rel + center;
        double val = glm::dot(dir, corner);
        axis_max = std::max(axis_max, val);
        axis_min = std::min(axis_min, val);
    }
    double axis_vel = glm::dot(dir, velocity);

    return {{axis_min, axis_max}, {axis_vel, axis_vel}};
}

/*
std::pair<double, double> RotatedBox::shadow(glm::vec2 start, glm::vec2 dir) const {
    glm::vec2 pt = scale / 2;
    pt = glm::vec2(cos(rotation) * pt.x - sin(rotation) * pt.y,
                   sin(rotation) * pt.x + cos(rotation) * pt.y);
    pt += position;
    double max = glm::dot(pt - start, dir), min = max;
    for(int n = 1; n < 4; n++) {
        pt = glm::vec2(n % 2 == 0 ? scale.x / 2 : -scale.x / 2,
                       n < 2 ? scale.y / 2 : -scale.y / 2);
        pt = glm::vec2(cos(rotation) * pt.x - sin(rotation) * pt.y,
                       sin(rotation) * pt.x + cos(rotation) * pt.y);
        pt += position;
        double val = glm::dot(pt - start, dir);
        max = std::max(max, val);
        min = std::min(min, val);
    }
    return {min, max};
}

int RotatedBox::axes() const {
    return 2;
}

glm::vec2 RotatedBox::axis(int n) const {
    return glm::vec2(cos(rotation + (float) n * 3.1415926536f / 2),
                     sin(rotation + (float) n * 3.1415926536f / 2));
}*/