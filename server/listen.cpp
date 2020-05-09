//
// Created by isaac on 5/8/2020.
//

#include "shared.h"

std::unique_ptr<Packet> Listen::get() {
    std::unique_ptr<Packet> packet = nullptr;
    pthread_mutex_lock(&packet_queue_mutex);
    if(!packet_queue.empty()) {
        packet = std::move(packet_queue.front());
        packet_queue.pop();
    }
    pthread_mutex_unlock(&packet_queue_mutex);
    return std::move(packet);
}

void* listen_thread(void* args) {
    Shared& shared = *((Shared*) args);

    const int PACKET_LENGTH = 1024;
    char message[PACKET_LENGTH];
    struct sockaddr_in address;
    int message_length;
    int address_length;

    while(!shared.exit) {
        //clear the buffer by filling null, it might have previously received data
        memset(message, 0, PACKET_LENGTH);

        //try to receive some data, this is a blocking call
        message_length = recvfrom(shared.socket, message, PACKET_LENGTH, 0,
                                  (struct sockaddr *) &address, &address_length);
        if(message_length == SOCKET_ERROR)
        {
            // timeout
            continue;
        }

        pthread_mutex_lock(&shared.listen.packet_queue_mutex);
        shared.listen.packet_queue.push(std::make_unique<Packet>(Packet{
                std::string(message, message_length),
                address,
                address_length
        }));
        pthread_mutex_unlock(&shared.listen.packet_queue_mutex);

        //now reply the client with the same data
        /*
        if (sendto(shared.socket, packet.message, packet.message_length, 0,
                (struct sockaddr*) &packet.address, packet.address_length) == SOCKET_ERROR)
        {
            shared.cerr << "Could not send data: " << WSAGetLastError() << "\n" << FLUSH;
            continue;
        }*/
    }

    pthread_exit(args);
    return nullptr;
}