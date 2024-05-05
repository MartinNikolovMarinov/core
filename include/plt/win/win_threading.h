#pragma once

#include <plt/core_threading.h>

#include <windows.h>

namespace core {

struct Mutex {
    CRITICAL_SECTION handle;
};

struct CondVariable {
    CONDITION_VARIABLE handle;
};

struct Thread {
    HANDLE handle;
    bool isRunning;
    mutable Mutex mu;

    NO_COPY(Thread);

    Thread() = default;

    Thread(Thread&& other) noexcept {
        handle = other.handle;
        isRunning = other.isRunning;
        mu = std::move(other.mu);

        other.handle = INVALID_HANDLE_VALUE;
        other.isRunning = false;
    }

    Thread& operator=(Thread&& other) noexcept {
        handle = other.handle;
        isRunning = other.isRunning;
        mu = std::move(other.mu);

        other.handle = nullptr;
        other.isRunning = false;

        return *this;
    }
};

}; // namespace core
