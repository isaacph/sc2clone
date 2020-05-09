//
// Created by isaac on 5/8/2020.
//

#include <pthread.h>
#include <string>
#include <iostream>
#include <vector>
#include "async_console.h"
#include <winsock2.h>
#include <memory>
#include "packet.h"
#include "shared.h"
#include "listen.h"
#include "response.h"
#include <sys/time.h>
#include "socket_init.h"

void* process_thread(void* args) {
    Shared& shared = *((Shared*) args);
    shared.cout << "Process thread started.\n" << FLUSH;

    while(!shared.exit) {
        std::unique_ptr<Packet> packet;
        while((packet = shared.listen.get()) != nullptr) {
            shared.cout << packet->message << "\n" << FLUSH;
            shared.response.send(std::move(packet));
        }
    }

    shared.cout << "Process thread exiting.\n" << FLUSH;
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

    pthread_create(&listen_pthread, NULL, listen_thread, &shared);
    pthread_create(&process_pthread, NULL, process_thread, &shared);
    pthread_create(&response_pthread, NULL, response_thread, &shared);
    pthread_create(&console_pthread, NULL, output_thread, &shared.consoleOut);

    shared.cout << "Main thread waiting for exit command. Press enter to exit.\n" << FLUSH;
    std::string line;
    std::getline(std::cin, line);

    shared.response.send(std::make_unique<Packet>(Packet{
        "hello",
        get_address(3800, "127.0.0.1")
    }));

    shared.cout << "Console input has requested exit.\n" << FLUSH;
    shared.exit = true;
    shared.consoleOut.exit();

    for(pthread_t& thread : threads) {
        if(thread != -1) {
            pthread_join(thread, NULL);
        }
    }

    close_socket(shared.socket);

    std::cout << "Program has terminated." << std::endl;
    return 0;
}