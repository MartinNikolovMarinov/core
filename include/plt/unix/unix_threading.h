#pragma once

#include <plt/core_threading.h>

#include <pthread.h>

namespace core {

struct Thread {
    pthread_t handle;
    bool isRunning;
    // FIXME: add a mutex to protect the handle.

    NO_COPY(Thread);

    Thread() noexcept : handle(pthread_t()), isRunning(false) {}

    // only move
    Thread(Thread&& other) noexcept : handle(other.handle) {
        other.handle = 0;
    }

    Thread& operator=(Thread&& other) noexcept {
        handle = other.handle;
        other.handle = 0;
        return *this;
    }
};

}; // namespace core
