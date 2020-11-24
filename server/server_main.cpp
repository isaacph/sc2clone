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
#include "socket_init.h"
#include "../logic/World.h"
#include "../math_util.h"
#include "Server.h"
#include <chrono>
#include <functional>
#include <atomic>
#include "WorldServer.h"

struct Shared {
    std::atomic<bool> running;
};

void* process_thread(void* args) {
    Shared& shared = *((Shared*) args);
    std::cout << "Process thread started.\n";

    Server server(shared.running);
    server.run();

    std::cout << "Process thread exiting.\n";
    pthread_exit(args);
    return nullptr;
}

int main() {

    Shared shared{true};

    std::vector<pthread_t> threads(1, -1);
    pthread_t& process_pthread = threads[0];

    std::cout << "Main thread waiting for exit command. Press enter to exit.\n";

    pthread_create(&process_pthread, nullptr, process_thread, &shared);

    // Make it so pressing enter a couple times exits the server cleanly
    std::string line;
    std::getline(std::cin, line);

    shared.running.store(false, std::memory_order_relaxed);

    for(int i = 0; i < threads.size(); ++i) {
        if(threads[i] != -1) {
            std::cout << "Joining thread " << i << std::endl;
            pthread_join(threads[i], nullptr);
        }
    }

    std::cout << "Program has terminated." << std::endl;
    return 0;
}