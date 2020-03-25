//
// Created by isaac on 11/11/2019.
//

#include "Simulation.h"
#include "Resolution.h"

template<typename T>
Handle<T> *Simulation::addRigidBody(T* rigidBody) {
    auto p = bodies.insert(rigidBody);
    return new Handle(*rigidBody, [&](){
        bodies.erase(p.first);
        delete rigidBody;
    });
}

void Simulation::update(double timeElapsed) {
    for(RigidBody* body1 : bodies) {
        for(RigidBody* body2 : bodies) {
            if(body1 == body2) {
                continue;
            }

            Duration aabbInt = boxIntersect({0, timeElapsed}, *body1, *body2);
            if(aabbInt.magnitude() > 0) {
                double stop = stoppingPoint(aabbInt, *body1, *body2);


            }
        }
    }
}

Simulation::Simulation() = default;
Simulation::~Simulation() = default;
