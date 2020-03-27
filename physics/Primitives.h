//
// Created by isaac on 3/24/2020.
//

#ifndef UNTITLED2_PRIMITIVES_H
#define UNTITLED2_PRIMITIVES_H

#include <ostream>

struct Duration {
    double start, end;
    inline double magnitude() {
        return end - start;
    }
};

typedef std::pair<double, double> Shadow;
typedef std::pair<double, double> ShadowVelocity;

struct MovingShadow {
    Shadow shadow;
    ShadowVelocity velocity;
};

struct AxisIntersection {
    Duration duration;
    double posStart, posEnd;
};

inline std::ostream& operator<<(std::ostream &os, const MovingShadow& s) {
    os << "{(" << s.shadow.first << ", " << s.shadow.second << "), (" << s.velocity.first << ", " <<
    s.velocity.second << ")}";
    return os;
}

#endif //UNTITLED2_PRIMITIVES_H
