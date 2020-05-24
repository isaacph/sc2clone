//
// Created by isaac on 5/8/2020.
//

#include <pthread.h>
#include <string>
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <memory>
#include "packet.h"
#include "shared.h"
#include "socket_init.h"
#include "../logic/World.h"
#include "../math_util.h"
#include "Server.h"
#include <chrono>
#include <functional>

void* process_thread(void* args) {
    Shared& shared = *((Shared*) args);
    std::cout << "Process thread started.\n" << std::endl;

    Server server(shared);
    server.run();

    std::cout << "Process thread exiting." << std::endl;
    pthread_exit(args);
    return nullptr;
}

int main() {

    Shared shared;
    shared.port = 3800;

    int ret = init_socket(shared.port, shared.timeout_ms, shared.socket, shared.address);
    if(ret != 0) {
        return ret;
    }

    std::vector<pthread_t> threads(4, -1);
    pthread_t& listen_pthread = threads[0];
    pthread_t& process_pthread = threads[1];
    pthread_t& response_pthread = threads[2];
    pthread_t& console_pthread = threads[3];

    std::cout << "Main thread waiting for exit command. Press enter to exit." << std::endl;

    pthread_create(&listen_pthread, NULL, listen_thread, &shared);
    pthread_create(&process_pthread, NULL, process_thread, &shared);
    pthread_create(&response_pthread, NULL, response_thread, &shared);

    std::string line;
    std::getline(std::cin, line);

    shared.exit = true;

    for(int i = 0; i < threads.size(); ++i) {
        if(threads[i] != -1) {
            std::cout << "Joining thread " << i << std::endl;
            pthread_join(threads[i], NULL);
        }
    }

    close_socket(shared.socket);

    std::cout << "Program has terminated." << std::endl;
    return 0;
}