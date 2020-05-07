//
// Created by isaac on 3/27/2020.
//

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "load_obj.h"

std::vector<float> loadObj(const std::string& path) {
    struct Face {
        glm::ivec3 vertex[3];
    };

    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << path << " was not found." << std::endl;
        return {};
    }

    std::vector<glm::vec3> in_vert;
    std::vector<glm::vec2> in_tex;
    std::vector<glm::vec3> in_norm;
    std::vector<Face> in_face;

    std::string line;
    int linet = 0;
    while(std::getline(file, line)) {
        if(line.find("v ") == 0) {
            double v1, v2, v3;
            std::stringstream conv;
            size_t p1 = line.find(' ', 1);
            size_t p2 = line.find(' ', p1 + 1);
            size_t p3 = line.find(' ', p2 + 1);
            size_t end = line.size();
            conv << line.substr(p1 + 1, p2 - p1 - 1);
            conv >> v1;
            std::stringstream().swap(conv);
            conv << line.substr(p2 + 1, p3 - p2 - 1);
            conv >> v2;
            std::stringstream().swap(conv);
            conv << line.substr(p3 + 1, end - p3 - 1);
            conv >> v3;
            in_vert.emplace_back(v1, v2, v3);
        } else if(line.find("vt ") == 0) {
            double v1, v2;
            std::stringstream conv;
            size_t p1 = line.find(' ', 1);
            size_t p2 = line.find(' ', p1 + 1);
            size_t end = line.size();
            conv << line.substr(p1 + 1, p2 - p1 - 1);
            conv >> v1;
            std::stringstream().swap(conv);
            conv << line.substr(p2 + 1, end - p2 - 1);
            conv >> v2;
            in_tex.emplace_back(v1, v2);
        } else if(line.find("vn ") == 0) {
            double v1, v2, v3;
            std::stringstream conv;
            size_t p1 = line.find(' ', 1);
            size_t p2 = line.find(' ', p1 + 1);
            size_t p3 = line.find(' ', p2 + 1);
            size_t end = line.size();
            conv << line.substr(p1 + 1, p2 - p1 - 1);
            conv >> v1;
            std::stringstream().swap(conv);
            conv << line.substr(p2 + 1, p3 - p2 - 1);
            conv >> v2;
            std::stringstream().swap(conv);
            conv << line.substr(p3 + 1, end - p3 - 1);
            conv >> v3;
            in_norm.emplace_back(v1, v2, v3);
        } else if(line.find("f ") == 0) {
            double v1a1, v1a2, v1a3, v2a1, v2a2, v2a3, v3a1, v3a2, v3a3;
            std::stringstream conv;
            size_t p1 = line.find(' ', 1);
            size_t p1s1 = line.find('/', p1 + 1);
            size_t p1s2 = line.find('/', p1s1 + 1);
            size_t p2 = line.find(' ', p1 + 1);
            size_t p2s1 = line.find('/', p2 + 1);
            size_t p2s2 = line.find('/', p2s1 + 1);
            size_t p3 = line.find(' ', p2 + 1);
            size_t p3s1 = line.find('/', p3 + 1);
            size_t p3s2 = line.find('/', p3s1 + 1);
            size_t end = line.size();
            conv << line.substr(p1 + 1, p1s1 - p1 - 1);
            conv >> v1a1;
            std::stringstream().swap(conv);
            conv << line.substr(p1s1 + 1, p1s2 - p1s1 - 1);
            conv >> v1a2;
            std::stringstream().swap(conv);
            conv << line.substr(p1s2 + 1, p2 - p1s2 - 1);
            conv >> v1a3;
            std::stringstream().swap(conv);
            conv << line.substr(p2 + 1, p2s1 - p2 - 1);
            conv >> v2a1;
            std::stringstream().swap(conv);
            conv << line.substr(p2s1 + 1, p2s2 - p2s1 - 1);
            conv >> v2a2;
            std::stringstream().swap(conv);
            conv << line.substr(p2s2 + 1, p3 - p2s2 - 1);
            conv >> v2a3;
            std::stringstream().swap(conv);
            conv << line.substr(p3 + 1, p3s1 - p3 - 1);
            conv >> v3a1;
            std::stringstream().swap(conv);
            conv << line.substr(p3s1 + 1, p3s2 - p3s1 - 1);
            conv >> v3a2;
            std::stringstream().swap(conv);
            conv << line.substr(p3s2 + 1, end - p3s2 - 1);
            conv >> v3a3;
            in_face.push_back({{{v1a1, v1a2, v1a3}, {v2a1, v2a2, v2a3}, {v3a1, v3a2, v3a3}}});
        }
    }

    std::vector<float> vbo;


    try {
        for (const Face &face : in_face) {
            for (auto vertex : face.vertex) {
                const glm::vec3 &v = in_vert.at(vertex.x - 1);
                const glm::vec2 &t = in_tex.at(vertex.y - 1);
                const glm::vec3 &n = in_norm.at(vertex.z - 1);
                vbo.insert(vbo.end(), {v.x, v.y, v.z, t.x, t.y, n.x, n.y, n.z});
            }
        }
    }
    catch(std::exception& e) {
        std::cout << in_vert.size() << ", " << in_tex.size() << ", " << in_norm.size() << "\n";
        std::cout << e.what() << std::endl;
    }

    return vbo;
}

std::vector<Triangle> getObjTriangles(std::vector<float> obj) {
    std::vector<Triangle> triangles;
    for(size_t i = 0; i < obj.size();) {
        Triangle t;
        t.p1.x = obj[i + 0];
        t.p1.y = obj[i + 1];
        t.p1.z = obj[i + 2];
        i += 8;
        t.p2.x = obj[i + 0];
        t.p2.y = obj[i + 1];
        t.p2.z = obj[i + 2];
        i += 8;
        t.p3.x = obj[i + 0];
        t.p3.y = obj[i + 1];
        t.p3.z = obj[i + 2];
        i += 8;
        triangles.push_back(t);
    }
    return triangles;
}