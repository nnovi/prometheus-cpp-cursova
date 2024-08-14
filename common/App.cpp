//
// Created by nickn on 26.07.2024.
//

#include "App.h"
#include "socket_compat.h"
#include <csignal>
#include <iostream>

std::latch shutdown_flag(1);
void sigint_handler(int signum) {
    shutdown_flag.count_down();
}

App::App() {
    std::signal(SIGINT, sigint_handler);
    Socket::initSockets();
}

void App::start() {
    try {
        run();
    } catch (std::exception& err) {
        std::cerr << "error occured: " << err.what() << std::endl;
        exit(1);
    }
}

App::~App() {
    Socket::cleanupSockets();
}

bool App::is_interrupted() {
    return shutdown_flag.try_wait();
}

void App::wait_interrupt() {
    shutdown_flag.wait();
}
