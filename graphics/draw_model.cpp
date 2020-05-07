//
// Created by isaac on 3/28/2020.
//

#include <cassert>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "draw_model.h"
#include "gl_config.h"
#include "shader_util.h"
#include "draw.h"

DrawModel::DrawModel() {
    {
        std::string vsource = "ILLEGAL", fsource = "ILLEGAL";
        vsource = VERTEX_SHADER;
        fsource = FRAGMENT_SHADER;

        shaderProgram = createShaderProgram(
                vsource,
                fsource,
                "",
                {"position", "normal"});
        uniformAmbiance = glGetUniformLocation(shaderProgram, "ambiance");
        uniformMatrix = glGetUniformLocation(shaderProgram, "matrix");
        uniformModel = glGetUniformLocation(shaderProgram, "model");
        uniformRotation = glGetUniformLocation(shaderProgram, "rotation");

        const int var_count = 4;
        const GLchar* vars[var_count] = {"Light.position", "Light.color", "Light.strength", "Material.diffuse"};
        GLuint indices[var_count];
        GLint offset[var_count];
        glGetUniformIndices(shaderProgram, var_count, vars, indices);
        glGetActiveUniformsiv(shaderProgram, var_count, indices, GL_UNIFORM_OFFSET, offset);
        for(int i = 0; i < var_count; ++i) {
            uboOffsets.insert({std::string(vars[i]), offset[i]});
        }

        glGenBuffers(SHADER_NUM_LIGHTS, uboLights);
        for(int i = 0; i < SHADER_NUM_LIGHTS; ++i) {
            std::vector<GLubyte> blockBuffer;
            GLuint blockIndex = glGetUniformBlockIndex(shaderProgram,
                                                       std::string(std::string("Light[") + std::to_string(i) +
                                                                   "]").c_str());
            ubiLights[i] = blockIndex;
            glUniformBlockBinding(shaderProgram, blockIndex, i + 1);

            glGetActiveUniformBlockiv(shaderProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE,
                                      &uboLightsSize[i]);
            blockBuffer = std::vector<GLubyte>(uboLightsSize[i], 0);

            glBindBuffer(GL_UNIFORM_BUFFER, uboLights[i]);
            glBufferData(GL_UNIFORM_BUFFER, uboLightsSize[i], &*blockBuffer.begin(), GL_DYNAMIC_DRAW);
        }

        ubiMaterial = glGetUniformBlockIndex(shaderProgram, "Material");
        glUniformBlockBinding(shaderProgram, ubiMaterial, 0);
        glGetActiveUniformBlockiv(shaderProgram, ubiMaterial, GL_UNIFORM_BLOCK_DATA_SIZE, &uboMaterialSize);
        std::vector<GLubyte> blockBuffer(uboMaterialSize, 0);
        glGenBuffers(1, &uboMaterial);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMaterial);
        glBufferData(GL_UNIFORM_BUFFER, uboMaterialSize, &*blockBuffer.begin(), GL_DYNAMIC_DRAW);
    }
}

DrawModel::ModelRef DrawModel::initModel(const std::string& path) {
    auto it = modelHistory.find(path);
    if(it != modelHistory.end()) {
        return it->second;
    }
    int posAttrib = 0, texAttrib = 0, normAttrib = 0;
    try {
        posAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("position");
        texAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("texture");
        normAttrib = GLOBAL_ATTRIBUTE_LOCATIONS.at("normal");
        assert(posAttrib >= 0);
        assert(texAttrib >= 0);
        assert(normAttrib >= 0);
    } catch(std::out_of_range& err) {
        std::cerr << "Resource not found! " << err.what() << std::endl;
        return {0, 0, 0, empty};
    }
    std::vector<float> vertexData = loadObj(path);

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    buffers.insert(vbo);
    vertexArrays.insert(vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData.at(0), GL_STATIC_DRAW);

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, false, (8 * sizeof(GLfloat)), nullptr);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, false, (8 * sizeof(GLfloat)), (void*) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normAttrib);
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, false, (8 * sizeof(GLfloat)), (void*) (5 * sizeof(GLfloat)));

    std::vector<Triangle>& triangles = modelHistoryTriangles.insert({path, getObjTriangles(vertexData)}).first->second;
    ModelRef ref = {vao, vbo, vertexData.size() / 8, triangles};
    modelHistory.insert({path, ref});
    return ref;
}

void DrawModel::draw(const DrawModel::ModelRef& model, DrawParameters& params) {
    glUseProgram(shaderProgram);
    glUniform3f(uniformAmbiance, params.ambient_color.x, params.ambient_color.y, params.ambient_color.z);
    glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(params.proj_view * params.model_matrix));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(params.model_matrix));
    glUniformMatrix3fv(uniformRotation, 1, GL_FALSE, glm::value_ptr(params.normal_matrix));
    for(int i = 0; i < SHADER_NUM_LIGHTS; ++i) {
        glm::vec3 pos;
        glm::vec3 col;
        float str = 0;
        if(i < params.light_positions.size()) {
            pos = params.light_positions[i];
            col = params.light_colors[i];
            str = params.light_strengths[i];
        }
        std::vector<GLubyte> buffer(uboLightsSize[i], 0);
        memcpy(&*buffer.begin() + uboOffsets["Light.position"] + sizeof(GLfloat) * 0, &pos.x, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.position"] + sizeof(GLfloat) * 1, &pos.y, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.position"] + sizeof(GLfloat) * 2, &pos.z, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.color"] + sizeof(GLfloat) * 0, &col.x, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.color"] + sizeof(GLfloat) * 1, &col.y, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.color"] + sizeof(GLfloat) * 2, &col.z, sizeof(GLfloat));
        memcpy(&*buffer.begin() + uboOffsets["Light.strength"], &str, sizeof(GLfloat));
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights[i]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, uboLightsSize[i], &*buffer.begin());
        glBindBufferBase(GL_UNIFORM_BUFFER, i + 1, uboLights[i]);
    }
    std::vector<GLubyte> buffer(uboMaterialSize, 0);
    memcpy(&*buffer.begin() + uboOffsets["Material.diffuse"] + sizeof(GLfloat) * 0, &params.diffuse_color.x, sizeof(GLfloat));
    memcpy(&*buffer.begin() + uboOffsets["Material.diffuse"] + sizeof(GLfloat) * 1, &params.diffuse_color.y, sizeof(GLfloat));
    memcpy(&*buffer.begin() + uboOffsets["Material.diffuse"] + sizeof(GLfloat) * 2, &params.diffuse_color.z, sizeof(GLfloat));
    glBindBuffer(GL_UNIFORM_BUFFER, uboMaterial);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, uboMaterialSize, &*buffer.begin());
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMaterial);
    glBindVertexArray(model.vao);
    glDrawArrays(GL_TRIANGLES, 0, model.vertices);
}

void DrawModel::clearModel(DrawModel::ModelRef& model) {
    //glDeleteBuffers(1, &model.buffer);
    //glDeleteVertexArrays(1, &model.vao);
    model.vao = -1;
    model.buffer = -1;
}

DrawModel::~DrawModel() {
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(SHADER_NUM_LIGHTS, uboLights);
    for(auto b : buffers) {
        glDeleteBuffers(1, &b);
    }
    for(auto v : vertexArrays) {
        glDeleteVertexArrays(1, &v);
    }
}