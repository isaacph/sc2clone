//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_LISTEN_H
#define UNTITLED2_LISTEN_H

#include "packet.h"
#include <queue>
#include <memory>

class Listen {
public:
    std::unique_ptr<Packet> get();
private:
    std::queue<std::unique_ptr<Packet>> packet_queue;
    pthread_mutex_t packet_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    friend void* listen_thread(void* args);
};

void* listen_thread(void* args);

#endif //UNTITLED2_LISTEN_H
