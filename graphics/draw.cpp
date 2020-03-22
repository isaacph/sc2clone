//
// Created by isaac on 9/23/2019.
//

//
// Created by isaac on 9/17/2019.
//

#include "draw.h"
#include <iostream>



void errorCheck(const std::string &err) {
    GLenum e = glGetError();
    if(e) {
        std::cerr << "GL Error Code: " << e << ", " << err << std::endl;
    }
}