//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_RESPONSE_H
#define UNTITLED2_RESPONSE_H
#include "packet.h"
#include <pthread.h>
#include <memory>
#include <queue>

class Response {
public:
    inline Response(SOCKET& socket) : socket(socket) {}
    void send_async(std::unique_ptr<Packet> packet);
    void send_sync(std::unique_ptr<Packet> packet);
private:
    std::queue<std::unique_ptr<Packet>> packet_queue;
    pthread_mutex_t packet_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t packet_queue_signal = PTHREAD_COND_INITIALIZER;
    friend void* response_thread(void* args);
    SOCKET& socket;
};

void* response_thread(void* args);

#endif //UNTITLED2_RESPONSE_H