//
// Created by isaac on 3/25/2020.
//

#include "DragonCurve.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
static const float F_PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825f;

static std::vector<glm::mat4> double_curve(std::vector<glm::mat4> curve) {
    if(curve.size() == 1) {
        curve = {glm::mat4(1.0f), glm::mat4(1.0f)};
        {
            glm::mat4 &mat = curve.at(0);
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5, 0)) * mat;
            mat = glm::rotate(glm::mat4(1.0f), -F_PI / 2, glm::vec3(0, 0, 1)) * mat;
            mat = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5, 0))) * mat;
        }
        return curve;
    }
    std::vector<glm::mat4> new_curve;
    size_t original_length = curve.size();
    for (auto &mat : curve) {
        new_curve.push_back(mat);
    }
    glm::mat4 transf = curve.front();
    for (size_t i = 0; i < original_length; i++) {
        auto mat = curve.at(i);
        mat = glm::inverse(transf) * mat;
        mat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0)) * mat;
        mat = glm::rotate(glm::mat4(1.0f), -F_PI / 2, glm::vec3(0, 0, 1)) * mat;
        mat = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5, 0)) * mat;
        mat = transf * mat;
        new_curve.insert(new_curve.begin(), mat);
    }
    return new_curve;
}

static std::vector<glm::mat4> half_curve(std::vector<glm::mat4> curve) {
    return std::vector<glm::mat4>(curve.begin() + curve.size() / 2, curve.end());
}

static std::vector<glm::mat4> dragon_curve(int n) {
    if (n == 0) {
        return {glm::mat4(1.0f)};
    }
    if (n >= 1) {
        std::vector<glm::mat4> curve = dragon_curve(n - 1);
        return double_curve(curve);
    }
    return {};
}

static std::vector<glm::mat4> scale(std::vector<glm::mat4> curve) {
    for(auto& mat1 : curve) {
        mat1 = glm::scale(glm::mat4(1.0f), glm::vec3(8, 8, 0)) * mat1 *
               glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 1.0, 0));
    }
    return curve;
}

DragonCurve::DragonCurve(Graphics& graphics) : rectangle(graphics.initRectangle()) {
    rectangle.color = glm::vec4(1, 1, 1, 1);

    prescaling = dragon_curve(12);
    curve = scale(prescaling);
}
void DragonCurve::draw(glm::mat4 proj_view) {
    glm::vec4 c[] = {
            {1, 0, 0, 1},
            {0, 1, 0, 1},
            {0, 0, 1, 1},
            {1, 1, 0, 1},
            {1, 0, 1, 1},
            {0, 1, 1, 1}
    };
    for(int i = 0; i < curve.size(); i++) {
        auto& mat = curve.at(i);
        rectangle.matrix = proj_view * mat;
        //rectangle.color = c[i % 6];
        rectangle.draw();
    }
}

void DragonCurve::add() {
    prescaling = double_curve(prescaling);
    curve = scale(prescaling);
}

void DragonCurve::subtract() {
    prescaling = half_curve(prescaling);
    curve = scale(prescaling);
}