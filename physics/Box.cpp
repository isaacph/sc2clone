//
// Created by isaac on 11/11/2019.
//

#include "Box.h"

Box::Box(glm::vec2 position, glm::vec2 scale) : position(position), scale(scale) {

}

glm::vec2 Box::max() const {
    return position + scale / 2;
}

glm::vec2 Box::min() const {
    return position - scale / 2;
}

glm::mat4 Box::toMatrix() const {
    return glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0)),
                      glm::vec3(scale.x, scale.y, 0));
}

std::pair<double, double> Box::shadow(glm::vec2 start, glm::vec2 dir) const {
    double max = glm::dot(position - scale / 2 - start, dir), min = max;
    for(int n = 1; n < 4; n++) {
        double val = glm::dot(glm::vec2(n % 2 == 0 ? position.x - scale.x / 2 : position.x + scale.x / 2,
                                        n > 1 ? position.y + scale.y / 2 : position.y - scale.y / 2) - start, dir);
        max = std::max(max, val);
        min = std::min(min, val);
    }
    return {min, max};
}

int Box::axes() const {
    return 2;
}

glm::vec2 Box::axis(int n) const {
    return n == 0 ? glm::vec2(1, 0) : glm::vec2(0, 1);
}