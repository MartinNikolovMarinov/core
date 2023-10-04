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

struct barrier {
    pthread_barrier_t native;
};

struct cond_var {
    pthread_cond_t native;
};

} // namespace core
