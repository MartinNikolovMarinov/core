#pragma once

#include <std/threading.h>

#include <pthread.h>

namespace core {

struct thread {
    mutable std::atomic<pthread_t> native = 0;
};

struct mutex {
    pthread_mutex_t native;
    mutex_type type;
};

struct cond_var {
    pthread_cond_t native;
};

struct once {
    pthread_once_t native;
};

template <typename T> struct tl_storage {
    pthread_key_t native;
};

template <typename T>
expected<tl_storage<T>, plt_err_code> tl_storage_init() {
    tl_storage<T> tls;
    i32 res = pthread_key_create(&tls.native, nullptr);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return tls;
}

template <typename T>
void tl_storage_destroy(tl_storage<T>& tls) {
    pthread_key_delete(tls.native);
}

template <typename T>
expected<plt_err_code> tl_storage_set(tl_storage<T>& tls, T* value) {
    i32 res = pthread_setspecific(tls.native, value);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

template <typename T>
expected<T*, plt_err_code> tl_storage_get(tl_storage<T>& tls) {
    T* res = reinterpret_cast<T*>(pthread_getspecific(tls.native));
    if (res == nullptr) {
        return unexpected(plt_err_code(ERR_THREADING_MISSING_TLS_VALUE));
    }

    return res;
}

} // namespace core
