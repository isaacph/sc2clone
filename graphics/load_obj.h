//
// Created by isaac on 5/2/2020.
//

#ifndef UNTITLED2_LOAD_OBJ_H
#define UNTITLED2_LOAD_OBJ_H
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Triangle {
    glm::vec3 p1, p2, p3;
};

std::vector<float> loadObj(const std::string& path);
std::vector<Triangle> getObjTriangles(std::vector<float> obj);

#endif //UNTITLED2_LOAD_OBJ_H
