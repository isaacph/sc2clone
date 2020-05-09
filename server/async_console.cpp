//
// Created by isaac on 5/8/2020.
//

#include "async_console.h"
#include <string>
#include <iostream>
#include <vector>
#include <sys/time.h>

ConsoleOut::Stream ConsoleOut::get_stream(std::ostream& os) {
    return Stream(*this, os);
}

ConsoleOut::Stream::Stream(ConsoleOut& co, std::ostream& os) : consoleOut(co), stream(os) {}

ConsoleOut::Stream& ConsoleOut::Stream::operator<<(const std::string& s) {
    pthread_mutex_lock(&consoleOut.queue_mutex);
    consoleOut.queue.push_back({s, stream});
    pthread_mutex_unlock(&consoleOut.queue_mutex);
    return *this;
}

ConsoleOut::Stream& ConsoleOut::Stream::operator<<(const char *s) {
    return operator<<(std::string(s));
}

ConsoleOut::Stream& ConsoleOut::Stream::operator<<(unsigned short s) {
    return operator<<(std::to_string(s));
}

ConsoleOut::Stream& ConsoleOut::Stream::operator<<(int s) {
    return operator<<(std::to_string(s));
}

ConsoleOut::Stream& ConsoleOut::Stream::operator<<(const Flush &flush) {
    pthread_mutex_lock(&consoleOut.queue_mutex);
    pthread_cond_signal(&consoleOut.queue_signal);
    pthread_mutex_unlock(&consoleOut.queue_mutex);
    return *this;
}

void ConsoleOut::exit() {
    pthread_mutex_lock(&queue_mutex);
    running = false;
    pthread_cond_signal(&queue_signal);
    pthread_mutex_unlock(&queue_mutex);
}

void* output_thread(void* args) {
    ConsoleOut& consoleOut = *((ConsoleOut*) args);
    std::cout << "Console output thread started." << std::endl;

    struct timespec timeToWait;
    struct timeval now;

    pthread_mutex_lock(&consoleOut.queue_mutex);
    while(true) {

        gettimeofday(&now, NULL);
        timeToWait.tv_sec = now.tv_sec;
        timeToWait.tv_nsec = (now.tv_usec + ConsoleOut::TIMED_WAIT_MILLISECONDS * 1000) * 1000;


        if(!consoleOut.running ||
        consoleOut.queue.size() > 0 ||
        !pthread_cond_timedwait(&consoleOut.queue_signal, &consoleOut.queue_mutex, &timeToWait)) {
            for (auto &s : consoleOut.queue) {
                s.stream << s.message;
            }

            consoleOut.queue.clear();
            if(!consoleOut.running) {
                break;
            }
        }
    }
    pthread_mutex_unlock(&consoleOut.queue_mutex);

    std::cout << "Console output thread exiting." << std::endl;
    pthread_exit(args);
    return nullptr;
}