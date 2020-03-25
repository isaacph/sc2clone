//
// Created by isaac on 11/11/2019.
//

#include "Resolution.h"


bool between(double a, double b, double between) {
    return (a <= between && between <= b) || (b <= between && between <= a);
}

bool boxIntersect(const glm::vec2& amin, const glm::vec2& amax, const glm::vec2& bmin, const glm::vec2& bmax) {
    return (between(amin.x, amax.x, bmin.x) || between(amin.x, amax.x, bmax.x) ||
            between(bmin.x, bmax.x, amin.x) || between(bmin.x, bmax.x, bmax.x)) &&
           (between(amin.y, amax.y, bmin.y) || between(amin.y, amax.y, bmax.y) ||
            between(bmin.y, bmax.y, amin.y) || between(bmin.y, bmax.y, amax.y));
}

AxisIntersection axisIntersect(Duration delta, const RotatingShadow& a, const RotatingShadow& b) {
    double dvel1 = b.velocity.first - a.velocity.second;
    double time1 =
            (a.shadow.second - b.shadow.first) * (dvel1 != 0 ? 1 / dvel1 : delta.end);
    double dvel2 = a.velocity.first - b.velocity.second;
    double time2 =
            (b.shadow.second - a.shadow.first) * (dvel2 != 0 ? 1 / dvel2 : delta.start);

    bool intersection = false;
    double start = delta.start, startp;
    double end = delta.end;
    if(time1 < time2) {
        if(time1 > delta.start && time1 < delta.end) {
            start = time1;

        }
    } else {

    }
    // ok this is getting super complicated
    // for future reference:
    /*
     * I'm trying to make it so that we have the coordinates of intersection start and end on an axis
     * This way we will be able to get the collision point, which can later be used to calculate collision response
     */

    Duration d = {
        std::min(delta.end, std::max(delta.start, std::min(time1, time2))),
        std::max(delta.start, std::min(delta.end, std::max(time1, time2)))
    };
    return {d,
            d.start == time1 ? b.shadow.first + b.velocity.first * time1 :
            d.start == time2 ? a.shadow.first + a.velocity.first * time2 : 0,
            d.end == time1 ? b.shadow.first + b.velocity.first * time1 :
            d.end == time2 ? a.shadow.first + a.velocity.first * time2 : 0
    };
}

Duration durationIntersection(const Duration& a, const Duration& b) {
    return {
        between(a.start, a.end, b.start) ? b.start : between(b.start, b.end, a.start) ? a.start : 0,
        between(a.start, a.end, b.end) ? b.end : between(b.start, b.end, a.end) ? a.end : 0
    };
}

std::ostream& operator<<(std::ostream& os, const Duration& duration) {
    os << duration.start << " -> " << duration.end;
    return os;
}

Duration boxIntersect(Duration delta, const Convex& a, const Convex& b) {
    return durationIntersection(axisIntersect(delta, a.getShadowX(), b.getShadowX()),
            axisIntersect(delta, a.getShadowY(), b.getShadowY()));
}

struct CollisionPoint {
    double time;
    glm::vec2 point;
};

double stoppingPoint(Duration delta, const Convex& a, const Convex& b) {
    Duration intersectionDuration = delta;
    std::set<glm::vec2> axes = a.getAxes(), baxes = b.getAxes();
    axes.insert(baxes.begin(), baxes.end());
    glm::vec2 collisionPoint;
    for(const glm::vec2& axis : axes) {
        double prevStart = intersectionDuration.start;
        RotatingShadow ashadow = a.getShadowAxis(axis), bshadow = b.getShadowAxis(axis);
        intersectionDuration = durationIntersection(intersectionDuration,
                axisIntersect(delta, ashadow, bshadow));
        if(prevStart != intersectionDuration.start) {
            double a = ashadow.shadow.first + ashadow.velocity.first * intersectionDuration.start;
            double b = ashadow.shadow.second + bshadow.velocity.second * intersectionDuration.start;
        }
    }
    return intersectionDuration.start;
}

// OLD
/*
bool shadowIntersect(const std::pair<double, double>& a, const std::pair<double, double>& b) {
    return between(a.first, a.second, b.first) || between(a.first, a.second, b.second) ||
           between(b.first, b.second, a.first) || between(b.first, b.second, a.second);
}

double shadowResolve(const std::pair<double, double>& a, const std::pair<double, double>& b) {
    return a.second - b.first;
}

void shrinkShadow(std::pair<double, double>& shadow) {
    shadow.first += 0.001;
    shadow.second -= 0.001;
}
// returns empty if no intersect
// returns the ways A should move to resolve the collision
std::vector<glm::vec2> resolve(const Convex& a, const Convex& b) {
    std::vector<glm::vec2> resolutions;
    glm::vec2 origin(0, 0);
    glm::vec2 dir;

    for(int n = 0; n < a.axes() + b.axes(); n++) {

        // dir must be unit vector
        dir = n < a.axes() ? a.axis(n) : b.axis(n - a.axes());

        auto ashadow = a.shadow(origin, dir);
        auto bshadow = b.shadow(origin, dir);
        shrinkShadow(ashadow);
        shrinkShadow(bshadow);
        if(!shadowIntersect(ashadow, bshadow)) {
            // no intersect
            return {};
        }

        resolutions.push_back(-dir * shadowResolve(ashadow, bshadow));
        resolutions.push_back(dir * shadowResolve(bshadow, ashadow));
    }
    return resolutions;
}

// returns the shortest vector in a list of vectors
glm::vec2 shortest(const std::vector<glm::vec2>& resolutions) {
    if(resolutions.empty()) {
        return glm::vec2(0);
    }
    double shortestMag2 = resolutions.at(0).x * resolutions.at(0).x +
                          resolutions.at(0).y * resolutions.at(0).y;
    glm::vec2 shortest = resolutions.at(0);
    for(size_t i = 1; i < resolutions.size(); i++) {
        double res = resolutions.at(i).x * resolutions.at(i).x +
                     resolutions.at(i).y * resolutions.at(i).y;
        if(res < shortestMag2) {
            shortest = resolutions.at(i);
            shortestMag2 = res;
        }
    }
    return shortest;
}

glm::vec2 shortest_noncolliding(const RotatedBox& start, const std::vector<glm::vec2>& resolutions) {
    if(resolutions.empty()) {
        return glm::vec2(0);
    }
    double shortestMag2 = resolutions.at(0).x * resolutions.at(0).x +
                          resolutions.at(0).y * resolutions.at(0).y;
    glm::vec2 shortest = resolutions.at(0);
    for(size_t i = 1; i < resolutions.size(); i++) {
        double res = resolutions.at(i).x * resolutions.at(i).x +
                     resolutions.at(i).y * resolutions.at(i).y;
        if(res < shortestMag2) {
            shortest = resolutions.at(i);
            shortestMag2 = res;
        }
    }
    return shortest;
}*/