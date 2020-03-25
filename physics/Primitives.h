//
// Created by isaac on 3/24/2020.
//

#ifndef UNTITLED2_PRIMITIVES_H
#define UNTITLED2_PRIMITIVES_H

struct Duration {
    double start, end;
    inline double magnitude() {
        return end - start;
    }
};

typedef std::pair<double, double> Shadow;
typedef std::pair<double, double> ShadowVelocity;

struct RotatingShadow {
    Shadow shadow;
    ShadowVelocity velocity;
};

struct AxisIntersection {
    Duration duration;
    double posStart, posEnd;
};

#endif //UNTITLED2_PRIMITIVES_H
