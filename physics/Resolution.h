//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_RESOLUTION_H
#define UNTITLED_RESOLUTION_H
#include "../math_util.h"
#include <ostream>
#include "Primitives.h"

class Convex {
public:
    [[nodiscard]] virtual RotatingShadow getShadowX() const = 0;
    [[nodiscard]] virtual RotatingShadow getShadowY() const = 0;
    [[nodiscard]] virtual RotatingShadow getShadowAxis(const glm::vec2& dir) const = 0;
    [[nodiscard]] virtual std::set<glm::vec2> getAxes() const = 0;
};

Duration boxIntersect(Duration delta, const Convex& a, const Convex& b);
AxisIntersection axisIntersect(Duration delta, const RotatingShadow& a, const RotatingShadow& b);
Duration durationIntersection(const Duration& a, const Duration& b);

double stoppingPoint(Duration delta, const Convex& a, const Convex& b);

bool between(double a, double b, double between);

std::ostream& operator<<(std::ostream& os, const Duration& duration);

/*
bool shadowIntersect(const std::pair<double, double>& a, const std::pair<double, double>& b);
double shadowResolve(const std::pair<double, double>& a, const std::pair<double, double>& b);
void shrinkShadow(std::pair<double, double>& shadow);

// returns empty if no intersect
// returns the ways A should move to resolve the collision
std::vector<glm::vec2> resolve(const Convex& a, const Convex& b);

// returns the shortest vector in a list of vectors
glm::vec2 shortest(const std::vector<glm::vec2>& resolutions);

glm::vec2 shortest_noncolliding(const RotatedBox& start, const std::vector<glm::vec2>& resolutions);
 */

#endif //UNTITLED_RESOLUTION_H
