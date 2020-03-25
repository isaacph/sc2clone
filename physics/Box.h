//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_BOX_H
#define UNTITLED_BOX_H
#include "../math_util.h"
#include "Convex.h"

class Box : public Convex {
public:
    Box(glm::vec2 position, glm::vec2 scale);
    glm::vec2 position;
    glm::vec2 scale;
    [[nodiscard]] glm::vec2 max() const;
    [[nodiscard]] glm::vec2 min() const;
    [[nodiscard]] virtual glm::mat4 toMatrix() const;
    [[nodiscard]] std::pair<double, double> shadow(glm::vec2 start, glm::vec2 dir) const override;
    [[nodiscard]] int axes() const override;
    [[nodiscard]] glm::vec2 axis(int n) const override;
};

#endif //UNTITLED_BOX_H
