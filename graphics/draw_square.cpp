//
// Created by isaac on 9/27/2019.
//

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "draw_square.h"
#include "shader_util.h"
#include "gl_config.h"
//#define DEBUG_GL

DrawSquare::DrawSquare() : vao(-1), vbo(-1), ebo(-1), buffers{} {
    std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
    int posAttrib;
    try {
        vsource = VERTEX_SHADER;
        fsource = FRAGMENT_SHADER;
        posAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("position");

        assert(posAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cout << "Resource not found! " << err.what() << std::endl;
        return;
    }

    shaderProgram = createShaderProgram(
            vsource,
            fsource,
            "",
            {"position"});
    uniformColor = glGetUniformLocation(shaderProgram, "color");
    uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    buffers[0] = vbo;
    buffers[1] = ebo;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementData), elementData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, false, 0, nullptr);

#ifdef DEBUG_GL
    glDrawArrays(GL_TRIANGLES, 0, 3);

    GLenum error = glGetError();
    if(error) {
        std::cout << "init square draw arrays gl error: " << error << std::endl;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    error = glGetError();
    if(error) {
        std::cout << "init square draw elements gl error: " << error << std::endl;
    }
#else
    GLenum error = glGetError();
    if(error) {
        std::cout << "init square gl error: " << error << std::endl;
    }
#endif
}

void DrawSquare::draw(const glm::mat4 &matrix, const glm::vec4 &color) const {
    glUseProgram(shaderProgram);
    glUniform4f(uniformColor, color.x, color.y, color.z, color.w);
    glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

#ifdef DEBUG_GL
    GLenum error = glGetError();
    if(error) {
        std::cout << "draw square gl error: " << error << std::endl;
    }
#endif
}

DrawSquare::~DrawSquare() {
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, buffers);
}

#ifdef RENDER_0
namespace draw::unique::square {

    GLfloat vertexData[] = {
            -0.5f, -0.5f,
            +0.5f, -0.5f,
            +0.5f, +0.5f,
            -0.5f, +0.5f
    };

    GLuint elementData[] = {
            0, 1, 2,
            2, 3, 0
    };

    GLint shaderProgram;

    GLint uniformColor;
    GLint uniformMatrix;

    GLuint vao, vbo, ebo;
    GLuint buffers[2];
}

using namespace draw::unique::square;

void draw::initSquare() {
    std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
    int posAttrib = -1;
    try {
        vsource = draw::RESOURCES.at("shaders/simple_v");
        fsource = draw::RESOURCES.at("shaders/simple_f");
        posAttrib = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at("position");

        assert(posAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cout << "Resource not found! " << err.what() << std::endl;
        return;
    }

    shaderProgram = createShaderProgram(
            vsource,
            fsource,
            "",
            {"position"});
    uniformColor = glGetUniformLocation(shaderProgram, "color");
    uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    buffers[0] = vbo;
    buffers[1] = ebo;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementData), elementData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, false, 0, nullptr);

#ifdef DEBUG_GL
    glDrawArrays(GL_TRIANGLES, 0, 3);

    GLenum error = glGetError();
    if(error) {
        std::cout << "init square draw arrays gl error: " << error << std::endl;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    error = glGetError();
    if(error) {
        std::cout << "init square draw elements gl error: " << error << std::endl;
    }
#else
    GLenum error = glGetError();
    if(error) {
        std::cout << "init square gl error: " << error << std::endl;
    }
#endif
}

void draw::square(const glm::mat4 &matrix, const glm::vec4 &color) {
    glUseProgram(shaderProgram);
    glUniform4f(uniformColor, color.x, color.y, color.z, color.w);
    glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

#ifdef DEBUG_GL
    GLenum error = glGetError();
    if(error) {
        std::cout << "draw square gl error: " << error << std::endl;
    }
#endif
}

void draw::destroySquare() {
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, buffers);
}
#endif