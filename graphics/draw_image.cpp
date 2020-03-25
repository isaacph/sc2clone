//
// Created by isaac on 9/27/2019.
//

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "gl_config.h"
#include "shader_util.h"
#include "draw_image.h"
//#define DEBUG_GL

DrawImage::DrawImage() {
    std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
    int posAttrib, texAttrib;
    try {
        vsource = VERTEX_SHADER;
        fsource = FRAGMENT_SHADER;
        posAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("position");
        texAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("texture");

        assert(posAttrib >= 0);
        assert(texAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cout << "Resource not found! " << err.what() << std::endl;
        return;
    }

    shaderProgram = createShaderProgram(
            vsource,
            fsource,
            "",
            {"position", "texture"});
    uniformColor = glGetUniformLocation(shaderProgram, "color");
    uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");
    uniformSampler = glGetUniformLocation(shaderProgram, "sampler");

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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, false, (4 * sizeof(GLfloat)), nullptr);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, false, (4 * sizeof(GLfloat)), (void*) (2 * sizeof(GLfloat)));

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

void DrawImage::draw(const glm::mat4 &matrix, const glm::vec4 &color, int texture_index) {
    glUseProgram(shaderProgram);
    glUniform4f(uniformColor, color.x, color.y, color.z, color.w);
    glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(uniformSampler, texture_index);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

#ifdef DEBUG_GL
    GLenum error = glGetError();
    if(error) {
        std::cout << "draw square gl error: " << error << std::endl;
    }
#endif
}

DrawImage::~DrawImage() {
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, buffers);
}

#ifdef RENDER_0
namespace draw::unique::image {

    GLfloat vertexData[] = {
            -0.5f, -0.5f, 0.0f, 1.0f,
            +0.5f, -0.5f, 1.0f, 1.0f,
            +0.5f, +0.5f, 1.0f, 0.0f,
            -0.5f, +0.5f, 0.0f, 0.0f
    };

    GLuint elementData[] = {
            0, 1, 2,
            2, 3, 0
    };

    GLint shaderProgram;

    GLint uniformColor;
    GLint uniformMatrix;
    GLint uniformSampler;

    GLuint vao, vbo, ebo;
    GLuint buffers[2];
}

using namespace draw::unique::image;

void draw::initDrawImage() {
    std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
    int posAttrib = -1, texAttrib = -1;
    try {
        vsource = draw::RESOURCES.at("shaders/texture_v");
        fsource = draw::RESOURCES.at("shaders/texture_f");
        posAttrib = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at("position");
        texAttrib = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at("texture");

        assert(posAttrib >= 0);
        assert(texAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cout << "Resource not found! " << err.what() << std::endl;
        return;
    }

    shaderProgram = createShaderProgram(
            vsource,
            fsource,
            "",
            {"position", "texture"});
    uniformColor = glGetUniformLocation(shaderProgram, "color");
    uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");
    uniformSampler = glGetUniformLocation(shaderProgram, "sampler");

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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, false, (4 * sizeof(GLfloat)), nullptr);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, false, (4 * sizeof(GLfloat)), (void*) (2 * sizeof(GLfloat)));

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

void draw::image(const glm::mat4 &matrix, const glm::vec4 &color) {
    glUseProgram(shaderProgram);
    glUniform4f(uniformColor, color.x, color.y, color.z, color.w);
    glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform1i(uniformSampler, 0);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

#ifdef DEBUG_GL
    GLenum error = glGetError();
    if(error) {
        std::cout << "draw square gl error: " << error << std::endl;
    }
#endif
}

void draw::destroyDrawImage() {
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, buffers);
}
#endif