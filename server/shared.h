//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_SHARED_H
#define UNTITLED2_SHARED_H
#include "packet.h"
#include "listen.h"
#include "response.h"
#include <winsock2.h>
#include <memory>
#include <pthread.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>

struct Shared {
    volatile bool exit = false;

    int port;
    const int timeout_ms = 500;
    SOCKET socket;
    struct sockaddr_in address;

    Listen listen;
    Response response = Response(socket);
};

#endif //UNTITLED2_SHARED_H
