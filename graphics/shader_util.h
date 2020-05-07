//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_SHADER_UTIL_H
#define UNTITLED2_SHADER_UTIL_H
#include <GL/glew.h>
#include <vector>

GLint createShader(const std::string &srcFile, GLenum type);
GLint createShaderProgram(const std::string &vsrcFile, const std::string &fsrcFile, const std::string &gsrcFile,
        const std::vector<std::string> &attributes);/*
void createUniformBuffer(GLint byteSize, std::vector<GLfloat> contents, std::vector<GLubyte>& buffer) {
    if(contents.size() * sizeof(GLfloat) > byteSize) {
        std::cerr << "Attempted to create uniform buffer with contents size greater than block size\n";
    }
    std::vector<GLubyte> temp(sizeof(GLfloat), 0);
    for(size_t i = 0; i < contents.size() && i < byteSize / sizeof(GLfloat); ++i) {
        memcpy(&*temp.begin(), &contents.at(i), sizeof(GLfloat));
        buffer.insert(buffer.end(), temp.begin(), temp.end());
    }
    temp = std::vector<GLubyte>(byteSize - buffer.size(), 0);
    buffer.insert(buffer.end(), temp.begin(), temp.end());
}*/

#endif //UNTITLED2_SHADER_UTIL_H
