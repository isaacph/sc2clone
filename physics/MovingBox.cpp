//
// Created by isaac on 11/11/2019.
//

#include "MovingBox.h"

MovingBox::MovingBox(glm::vec2 position, glm::vec2 scale, glm::vec2 velocity)
: Box(position, scale), velocity(velocity) {}

void MovingBox::resolve(glm::vec2 dp) {
    if(dp.y > 0) {
        onGround = true;
        velocity.y = 0;
    } else if(dp.y < 0) {
        velocity.y = 0;
    }
    position += dp;
}