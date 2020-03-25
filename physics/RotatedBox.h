//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_ROTATEDBOX_H
#define UNTITLED_ROTATEDBOX_H
#include "MovingBox.h"

struct RotatedBox : public MovingBox {
public:
    RotatedBox(glm::vec2 position, glm::vec2 scale, double rotation = 0);
    double rotation;
    [[nodiscard]] glm::mat4 toMatrix() const override;
    [[nodiscard]] std::pair<double, double> shadow(glm::vec2 start, glm::vec2 dir) const override;
    [[nodiscard]] int axes() const override;
    [[nodiscard]] glm::vec2 axis(int n) const override;
};

#endif //UNTITLED_ROTATEDBOX_H
