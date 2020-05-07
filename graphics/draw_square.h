//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_DRAW_SQUARE_H
#define UNTITLED2_DRAW_SQUARE_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "constants.h"

class DrawSquare {
public:
#ifndef USE_EXTERNAL_FOLDER_RESOURCES
    const std::string VERTEX_SHADER = "res/shaders/simple_v.glsl";
    const std::string FRAGMENT_SHADER = "res/shaders/simple_f.glsl";
#else
    const std::string VERTEX_SHADER = "../res/shaders/simple_v.glsl";
    const std::string FRAGMENT_SHADER = "../res/shaders/simple_f.glsl";
#endif
    DrawSquare();
    void draw(const glm::mat4 &matrix, const glm::vec4 &color) const;
    ~DrawSquare();
private:
    const GLfloat vertexData[8] = {
            -0.5f, -0.5f,
            +0.5f, -0.5f,
            +0.5f, +0.5f,
            -0.5f, +0.5f
    };

    const GLuint elementData[6] = {
            0, 1, 2,
            2, 3, 0
    };

    GLint shaderProgram;

    GLint uniformColor;
    GLint uniformMatrix;

    GLuint vao, vbo, ebo;
    GLuint buffers[2];
};

#endif //UNTITLED2_DRAW_SQUARE_H
