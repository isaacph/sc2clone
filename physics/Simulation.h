//
// Created by isaac on 11/11/2019.
//

#ifndef UNTITLED_SIMULATION_H
#define UNTITLED_SIMULATION_H
#include "RigidBody.h"
#include "../Handle.h"
#include <vector>
#include <memory>
#include <set>

class Simulation {
public:
    Simulation();
    template<typename T> Handle<T> * addRigidBody(T* rigidBody);
    void update(double timeElapsed);
    ~Simulation();
private:
    std::set<RigidBody*> bodies;
};


#endif //UNTITLED_SIMULATION_H
