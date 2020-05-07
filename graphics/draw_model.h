//
// Created by isaac on 3/28/2020.
//

#ifndef UNTITLED2_DRAW_MODEL_H
#define UNTITLED2_DRAW_MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "constants.h"
#include "load_obj.h"

class DrawModel {
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
    };
    DrawModel();
    ModelRef initModel(const std::string& path);
    void clearModel(ModelRef& model);
    void draw(const ModelRef& model, DrawParameters& info);
    ~DrawModel();
private:
#ifndef USE_EXTERNAL_FOLDER_RESOURCES
    const std::string VERTEX_SHADER = "res/shaders/simple3d_v.glsl";
    const std::string FRAGMENT_SHADER = "res/shaders/simple3d_f.glsl";
#else
    const std::string VERTEX_SHADER = "../res/shaders/simple3d_v.glsl";
    const std::string FRAGMENT_SHADER = "../res/shaders/simple3d_f.glsl";
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

    std::set<GLuint> buffers;
    std::set<GLuint> vertexArrays;
    std::map<std::string, ModelRef> modelHistory;
    std::map<std::string, std::vector<Triangle>> modelHistoryTriangles;
    const std::vector<Triangle> empty;
};


#endif //UNTITLED2_DRAW_MODEL_H
