//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_CONVEX_H
#define UNTITLED_CONVEX_H
#include "RigidBody.h"
#include "Primitives.h"
#include <set>

class Convex {
public:
    [[nodiscard]] virtual RotatingShadow getShadowX() const = 0;
    [[nodiscard]] virtual RotatingShadow getShadowY() const = 0;
    [[nodiscard]] virtual RotatingShadow getShadowAxis(const glm::vec2& dir) const = 0;
    [[nodiscard]] virtual std::set<glm::vec2> getAxes() const = 0;
};


#endif //UNTITLED_CONVEX_H
