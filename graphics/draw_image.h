//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_DRAW_IMAGE_H
#define UNTITLED2_DRAW_IMAGE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class DrawImage {
public:
    DrawImage();
    void draw(const glm::mat4& matrix, const glm::vec4& color, int texture_index);
    ~DrawImage();
    const std::string VERTEX_SHADER = "res/shaders/texture_v.glsl";
    const std::string FRAGMENT_SHADER = "res/shaders/texture_f.glsl";

private:

    GLfloat vertexData[16] = {
            -0.5f, -0.5f, 0.0f, 1.0f,
            +0.5f, -0.5f, 1.0f, 1.0f,
            +0.5f, +0.5f, 1.0f, 0.0f,
            -0.5f, +0.5f, 0.0f, 0.0f
    };

    GLuint elementData[6] = {
            0, 1, 2,
            2, 3, 0
    };

    GLint shaderProgram;

    GLint uniformColor;
    GLint uniformMatrix;
    GLint uniformSampler;

    GLuint vao{}, vbo{}, ebo{};
    GLuint buffers[2]{};
};

#endif //UNTITLED2_DRAW_IMAGE_H
