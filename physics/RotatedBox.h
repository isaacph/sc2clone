//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_ROTATEDBOX_H
#define UNTITLED_ROTATEDBOX_H
#include "Box.h"

struct RotatedBox : public Box {
public:
    RotatedBox(glm::vec2 position, glm::vec2 scale, double rotation = 0);
    double rotation;
    [[nodiscard]] glm::mat4 toMatrix() const override;
    [[nodiscard]] MovingShadow getShadowX() const override;
    [[nodiscard]] MovingShadow getShadowY() const override;
    [[nodiscard]] MovingShadow getShadowAxis(const glm::vec2& dir) const override;
    [[nodiscard]] std::set<SortVec> getAxes() const override;
};

#endif //UNTITLED_ROTATEDBOX_H
