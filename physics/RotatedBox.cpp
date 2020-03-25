//
// Created by isaac on 11/11/2019.
//

#include "RotatedBox.h"

RotatedBox::RotatedBox(glm::vec2 position, glm::vec2 scale, double rotation) : MovingBox(position, scale),
rotation(rotation) {

}

glm::mat4 RotatedBox::toMatrix() const {
    return glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0)),
                                  glm::vec3(scale.x, scale.y, 0)), (float) rotation, glm::vec3(0, 0, 1));
}

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
}