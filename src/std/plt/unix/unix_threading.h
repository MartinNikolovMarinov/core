#pragma once

#include <std/threading.h>

#include <pthread.h>

namespace core {

struct thread {
    pthread_t native;
};

struct mutex {
    pthread_mutex_t native;
    mutex_type type;
};

} // namespace core
