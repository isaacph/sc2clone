//
// Created by isaac on 5/9/2020.
//

#ifndef UNTITLED2_WORLD_H
#define UNTITLED2_WORLD_H


struct Command {
    enum Type {
        NONE, MOVE
    } type = NONE;
    glm::vec2 destination;
};

struct Unit {
    enum Type {
        WORKER, BARRACKS, SOLDIER
    } type;
    int team;
    float radius;
    glm::vec2 position;
    float direction = 0;
    float health = 100;
    float speed = 4.0f;
    bool dead = false;
    Command command;
};

struct World {
    std::map<int, Unit> units;
    std::set<int> unitsSelected;
};

#endif //UNTITLED2_WORLD_H
