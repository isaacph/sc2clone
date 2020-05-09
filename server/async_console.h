//
// Created by isaac on 5/8/2020.
//

#ifndef UNTITLED2_ASYNC_CONSOLE_H
#define UNTITLED2_ASYNC_CONSOLE_H
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct Flush {
} const FLUSH;

class ConsoleOut {
public:
    static const int TIMED_WAIT_MILLISECONDS = 500;
    class Stream {
    public:
        Stream& operator<<(const std::string& s);
        Stream& operator<<(const char* s);
        Stream& operator<<(unsigned short s);
        Stream& operator<<(int s);
        Stream& operator<<(const Flush& flush);
    private:
        Stream(ConsoleOut& co, std::ostream& stream);
        ConsoleOut& consoleOut;
        std::ostream& stream;
        friend class ConsoleOut;
    };
    Stream get_stream(std::ostream& os);
    void exit();
private:
    struct Message {
        std::string message;
        std::ostream& stream;
    };
    bool running = true;
    std::vector<Message> queue;
    pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t queue_signal = PTHREAD_COND_INITIALIZER;
    friend void* output_thread(void* args);
};

void* output_thread(void* args);

#endif //UNTITLED2_ASYNC_CONSOLE_H
