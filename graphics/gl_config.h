//
// Created by isaac on 3/22/2020.
//

#ifndef UNTITLED2_GL_CONFIG_H
#define UNTITLED2_GL_CONFIG_H
#include <map>

inline const std::map<std::string, GLint> GLOBAL_ATTRIBUTE_LOCATIONS = { // NOLINT(cert-err58-cpp)
        {"position", 0},
        {"texture",  1},
        {"normal",   2}
};

#endif //UNTITLED2_GL_CONFIG_H
