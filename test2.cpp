//
// Created by isaac on 12/21/2020.
//

#include "./server/ServerNetworking.h"
#include "./logic/UniqueID.h"
#include <iostream>
#include <chrono>

int main() {
    UniqueIDGenerator gen;
    Networking networking;
    networking.initSocket(3801, true);
    struct sockaddr_in server = getNetworkingAddress(3800, "127.0.0.1");
    networking.send(std::make_unique<Packet>(gen.generate().to_string() + " ping", server));
    bool rec = false;
    std::chrono::steady_clock::time_point delta_begin = std::chrono::steady_clock::now();

    networking.pollAndSend();
    networking.pollAndSend();
    networking.pollAndSend();
    networking.pollAndSend();

    std::chrono::steady_clock::time_point point = delta_begin;
    int pollCount = 0;
    do {
        for(auto& in : networking.pollAndSend()) {
            std::chrono::steady_clock::time_point delta_end = std::chrono::steady_clock::now();
            double duration = std::chrono::duration<double>(delta_end - delta_begin).count();
            std::cout << "In (" << pollCount << ") " << duration << ": " << in->message << "\n";
            rec = true;
        }
        std::chrono::steady_clock::time_point intermediate = std::chrono::steady_clock::now();
        while(std::chrono::duration<double>(intermediate - point).count() < 1.0 / 60.0) {
            intermediate = std::chrono::steady_clock::now();
        }
        ++pollCount;
    } while(!rec);

    return 0;
}