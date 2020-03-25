//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_SHADER_UTIL_H
#define UNTITLED2_SHADER_UTIL_H
#include <GL/glew.h>
#include <vector>

GLint createShader(const std::string &srcFile, GLenum type);
GLint createShaderProgram(const std::string &vsrcFile, const std::string &fsrcFile, const std::string &gsrcFile,
        const std::vector<std::string> &attributes);

#endif //UNTITLED2_SHADER_UTIL_H
