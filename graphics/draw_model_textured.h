//
// Created by isaac on 5/2/2020.
//

#ifndef UNTITLED2_DRAW_MODEL_TEXTURED_H
#define UNTITLED2_DRAW_MODEL_TEXTURED_H
#include "constants.h"
#include "draw_model.h"
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <map>
#include <set>

class DrawTexturedModel {
public:
    struct ModelRef {
        GLuint vao, buffer;
        size_t vertices;
        const std::vector<Triangle>& triangles;
    };
    struct DrawParameters {
        const glm::mat4& proj_view;
        const glm::mat4& model_matrix;
        const glm::mat3& normal_matrix;
        const glm::vec3& diffuse_color;
        const glm::vec3& ambient_color;
        std::vector<glm::vec3>& light_positions;
        std::vector<glm::vec3>& light_colors;
        std::vector<float>& light_strengths;
        int sampler = 0;
    };
    DrawTexturedModel();
    DrawTexturedModel::ModelRef initModel(const std::string& path);
    void clearModel(ModelRef& model);
    void draw(const ModelRef& model, DrawParameters& info);
    ~DrawTexturedModel();
private:
#ifndef USE_EXTERNAL_FOLDER_RESOURCES
    const std::string VERTEX_SHADER = "res/shaders/texture3d_v.glsl";
    const std::string FRAGMENT_SHADER = "res/shaders/texture3d_f.glsl";
#else
    const std::string VERTEX_SHADER = "../res/shaders/texture3d_v.glsl";
    const std::string FRAGMENT_SHADER = "../res/shaders/texture3d_f.glsl";
#endif
    GLint shaderProgram;
    GLint uniformAmbiance;
    GLint uniformMatrix;
    GLint uniformModel;
    GLint uniformRotation;
    GLuint uboLights[SHADER_NUM_LIGHTS];
    GLuint ubiLights[SHADER_NUM_LIGHTS];
    GLint uboLightsSize[SHADER_NUM_LIGHTS];
    GLuint uboMaterial;
    GLuint ubiMaterial;
    GLint uboMaterialSize;
    std::map<std::string, GLint> uboOffsets;
    GLint uniformSampler;

    std::set<GLuint> buffers;
    std::set<GLuint> vertexArrays;
    std::map<std::string, ModelRef> modelHistory;
    std::map<std::string, std::vector<Triangle>> modelHistoryTriangles;
    const std::vector<Triangle> empty;
};

#endif //UNTITLED2_DRAW_MODEL_TEXTURED_H
