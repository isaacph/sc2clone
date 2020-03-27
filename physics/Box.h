//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_BOX_H
#define UNTITLED_BOX_H
#include "../math_util.h"
#include "Convex.h"
#include <set>

class Box : public Convex {
public:
    Box(glm::vec2 position, glm::vec2 scale);
    glm::vec2 position;
    glm::vec2 scale;
    glm::vec2 velocity;
    [[nodiscard]] virtual glm::mat4 toMatrix() const;
    [[nodiscard]] MovingShadow getShadowX() const override;
    [[nodiscard]] MovingShadow getShadowY() const override;
    [[nodiscard]] MovingShadow getShadowAxis(const glm::vec2& dir) const override;
    [[nodiscard]] std::set<SortVec> getAxes() const override;
};

#endif //UNTITLED_BOX_H
