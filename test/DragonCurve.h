//
// Created by isaac on 3/25/2020.
//

#ifndef UNTITLED2_MANDELBROT_H
#define UNTITLED2_MANDELBROT_H
#include "../graphics/draw.h"

class DragonCurve {
public:
    explicit DragonCurve(Graphics& graphics);
    void draw(glm::mat4 proj_view);
    void add();
    void subtract();
private:
    Graphics::Rectangle rectangle;
    std::vector<glm::mat4> curve, prescaling;
};

#endif //UNTITLED2_MANDELBROT_H
