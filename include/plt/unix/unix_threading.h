#pragma once

#include <plt/core_threading.h>

#include <pthread.h>

namespace core {

struct Mutex {
    pthread_mutex_t handle;
};

struct CondVariable {
    pthread_cond_t handle;
};

struct Thread {
    pthread_t handle;
    bool isRunning;
    mutable Mutex mu;

    NO_COPY(Thread);

    Thread() = default;

    // only move
    Thread(Thread&& other) noexcept {
        handle = other.handle;
        isRunning = other.isRunning;
        mu = std::move(other.mu);

        other.handle = 0;
        other.isRunning = false;
    }

    Thread& operator=(Thread&& other) noexcept {
        handle = other.handle;
        other.handle = 0;
        return *this;
    }
};

}; // namespace core
