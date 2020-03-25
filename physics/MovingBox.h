//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_MOVINGBOX_H
#define UNTITLED_MOVINGBOX_H
#include "Box.h"

struct MovingBox : public Box {
    MovingBox(glm::vec2 position, glm::vec2 scale, glm::vec2 velocity = glm::vec2(0));
    void resolve(glm::vec2 dp);

    glm::vec2 velocity;
    bool onGround = false;
};


#endif //UNTITLED_MOVINGBOX_H
