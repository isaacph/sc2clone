//
// Created by isaac on 3/24/2020.
//

#ifndef UNTITLED2_MATH_H
#define UNTITLED2_MATH_H
#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

inline glm::vec2 operator*(const glm::vec2 &a, double b) {
    return glm::vec2(a.x * b, a.y * b);
}
inline glm::vec2 operator*(double b, const glm::vec2 &a) {
    return glm::vec2(a.x * b, a.y * b);
}
inline glm::vec2 operator/(const glm::vec2 &a, double b) {
    return glm::vec2(a.x / b, a.y / b);
}

#endif //UNTITLED2_MATH_H
