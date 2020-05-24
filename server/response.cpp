//
// Created by isaac on 5/8/2020.
//
#include "response.h"
#include "shared.h"
#include <sys/time.h>

void Response::send_async(std::unique_ptr<Packet> packet) {
    pthread_mutex_lock(&packet_queue_mutex);
    packet_queue.push(std::move(packet));
    pthread_mutex_unlock(&packet_queue_mutex);
}

void Response::send_sync(std::unique_ptr<Packet> packet) {
    sendto(socket, &*packet->message.begin(), packet->message.size(), 0,
           (struct sockaddr*) &packet->address, packet->address_length);
}

void* response_thread(void* args) {
    Shared& shared = *((Shared*) args);

    pthread_mutex_lock(&shared.response.packet_queue_mutex);
    while(true) {
        std::unique_ptr<Packet> packet;

        struct timespec timeToWait;
        struct timeval now;

        gettimeofday(&now, NULL);
        timeToWait.tv_sec = now.tv_sec;
        timeToWait.tv_nsec = (now.tv_usec + 500 * 1000) * 1000;

        if(shared.exit || !shared.response.packet_queue.empty() ||
           !pthread_cond_timedwait(&shared.response.packet_queue_signal, &shared.response.packet_queue_mutex, &timeToWait)) {
            while(!shared.response.packet_queue.empty()) {
                packet = std::move(shared.response.packet_queue.front());
                shared.response.packet_queue.pop();

                if (sendto(shared.socket, &*packet->message.begin(), packet->message.size(), 0,
                           (struct sockaddr*) &packet->address, packet->address_length) == SOCKET_ERROR)
                {
                    std::cerr << "Response thread could not send data: " << WSAGetLastError() << std::endl;
                    continue;
                }
            }

            if(shared.exit) {
                break;
            }
        }
    }
    pthread_mutex_unlock(&shared.response.packet_queue_mutex);
    pthread_exit(args);
    return nullptr;
}