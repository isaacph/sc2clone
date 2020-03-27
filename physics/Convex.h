//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_CONVEX_H
#define UNTITLED_CONVEX_H
#include "Primitives.h"
#include <set>

class Convex {
public:
    [[nodiscard]] virtual MovingShadow getShadowX() const = 0;
    [[nodiscard]] virtual MovingShadow getShadowY() const = 0;
    [[nodiscard]] virtual MovingShadow getShadowAxis(const glm::vec2& dir) const = 0;
    [[nodiscard]] virtual std::set<SortVec> getAxes() const = 0;
};


#endif //UNTITLED_CONVEX_H
