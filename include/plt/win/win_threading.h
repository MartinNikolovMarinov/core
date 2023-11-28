#pragma once

#include <plt/core_threading.h>

#include <windows.h>

namespace core {

struct Thread {
    HANDLE handle;
    bool isJoinable;
    // FIXME: add a mutex to protect the handle.

    NO_COPY(Thread);

    Thread() noexcept : handle(0), isJoinable(false) {}

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
