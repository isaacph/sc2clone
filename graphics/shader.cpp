//
// Created by isaac on 9/11/2019.
//

#include "draw.h"
#include <iostream>
#include <fstream>

GLint createShader(const std::string &srcFile, GLenum type)
{
    // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    std::ifstream t(srcFile);
    std::string src;

    t.seekg(0, std::ios::end);
    std::streamsize pos = t.gcount();
    src.reserve(pos);
    t.seekg(0, std::ios::beg);

    src.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    const GLchar* csrc = src.c_str();
    GLint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint bufflen = 0, success = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufflen);
    if (bufflen > 1)
    {
        auto* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(shader, bufflen, 0, log_string);
        std::cout << "Shader info log for " << srcFile << ":\n" << log_string << std::endl;

        delete[] log_string;
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        std::cout << "Failed to compile shader " << srcFile << std::endl;
    }
    return shader;
}

GLint createShaderProgram(const std::string &vsrcFile, const std::string &fsrcFile, const std::string &gsrcFile, const std::vector<std::string> &attributes)
{
    bool geometryShader = !gsrcFile.empty();

    GLint vshader = createShader(vsrcFile, GL_VERTEX_SHADER);
    GLint fshader = createShader(fsrcFile, GL_FRAGMENT_SHADER);
    GLint gshader = 0;
    if(geometryShader) gshader = createShader(gsrcFile, GL_GEOMETRY_SHADER);

    GLint program = glCreateProgram();

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    if(geometryShader) glAttachShader(program, gshader);

    for(const std::string& attr : attributes) {
        assert(draw::GLOBAL_ATTRIBUTE_LOCATIONS.find(attr) != draw::GLOBAL_ATTRIBUTE_LOCATIONS.end());
        GLint location = draw::GLOBAL_ATTRIBUTE_LOCATIONS.at(attr);
        glBindAttribLocation(program, location, attr.c_str());
    }

    glLinkProgram(program);
    glValidateProgram(program);
    glUseProgram(program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);
    if(geometryShader) glDeleteShader(gshader);

    return program;
}
