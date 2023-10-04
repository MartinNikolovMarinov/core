#include <std/plt/unix/unix_threading.h>

namespace core {

#pragma region THREADING

// The _SC_NPROCESSORS_ONLN may be named _SC_NPROC_ONLN on some systems. I saw some codebases use somthing like this:
#ifndef _SC_NPROCESSORS_ONLN
#  ifdef _SC_NPROC_ONLN
#    define _SC_NPROCESSORS_ONLN _SC_NPROC_ONLN
#  endif
#endif

expected<i32, plt_err_code> threading_get_num_cores() {
    auto n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 0) {
        return unexpected(plt_err_code(errno)); // this does set errno!
    }
    return i32(n);
}

void threading_exit(i32 exitCode) {
    pthread_exit(reinterpret_cast<void*>(exitCode));
}

namespace {

thread thread_from_native(pthread_t id) {
    thread t;
    auto pthreadId = reinterpret_cast<pthread_t*>(&t.native);
    *pthreadId = id;
    return t;
}

pthread_t thread_to_native(const thread& t) {
    auto pthreadId = reinterpret_cast<const pthread_t*>(&t.native);
    return *pthreadId;
}

}

thread threading_get_current() {
    thread t = thread_from_native(pthread_self());
    return t;
}

bool thread_is_running(const thread& t) {
    // FIXME: This absolutely needs to be thread safe!
    return t.native != 0;
}

expected<plt_err_code> thread_start(thread& out, void* arg, thread_routine routine) {
    if (thread_is_running(out)) {
        // Thread is already running.
        return unexpected(ERR_THREADING_THREAD_IS_ALREADY_RUNNING);
    }

    pthread_t pthreadId;
    i32 res = pthread_create(&pthreadId, nullptr, routine, arg);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    out = thread_from_native(pthreadId);
    return {};
}

thread_id thread_get_id(const thread& t) {
    if (!thread_is_running(t)) return 0;
    auto pthreadId = thread_to_native(t);
    return pthreadId;
}

expected<plt_err_code> thread_join(thread& t) {
    if (!thread_is_running(t)) return unexpected(ERR_THREADING_THREAD_IS_NOT_STARTED);

    auto pthreadId = thread_to_native(t);
    void *retval = nullptr;
    i32 res = pthread_join(pthreadId, &retval);

    if (retval != nullptr && retval != PTHREAD_CANCELED) {
        // TODO: This check might be useless?
        return unexpected(plt_err_code(EINVAL));
    }

    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<plt_err_code> thread_detach(thread& t) {
    if (!thread_is_running(t)) return unexpected(ERR_THREADING_THREAD_IS_NOT_STARTED);

    auto pthreadId = thread_to_native(t);
    i32 res = pthread_detach(pthreadId);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<bool, plt_err_code> thread_eq(const thread& t1, const thread& t2) {
    if (!thread_is_running(t1) || !thread_is_running(t2)) return unexpected(ERR_THREADING_THREAD_IS_NOT_STARTED);
    auto t1native = thread_to_native(t1);
    auto t2native = thread_to_native(t2);
    i32 res = pthread_equal(t1native, t2native); // can't fail.
    return res != 0;
}

expected<plt_err_code> thread_cancel(thread& t) {
    if (!thread_is_running(t)) return unexpected(ERR_THREADING_THREAD_IS_NOT_STARTED);

    auto pthreadId = thread_to_native(t);
    i32 res = pthread_cancel(pthreadId);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

#pragma endregion

#pragma region MUTEX

expected<plt_err_code> mutex_init(mutex& out, mutex_type type) {
    i32 res = 0;

    if (out.type == mutex_type::Normal) {
        res = pthread_mutex_init(&out.native, nullptr);
    }
    else if (out.type == mutex_type::Recursive || out.type == mutex_type::ErrorCheck) {
        pthread_mutexattr_t attr;
        res = pthread_mutexattr_init(&attr);
        if (res != 0) {
            return unexpected(plt_err_code(res));
        }

        defer {
            // attribute is no longer needed.
            res = pthread_mutexattr_destroy(&attr);
        };

        auto type = out.type == mutex_type::Recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK;
        res = pthread_mutexattr_settype(&attr, type);
        if (res != 0) {
            return unexpected(plt_err_code(res));
        }

        res = pthread_mutex_init(&out.native, &attr);
        if (res != 0) {
            return unexpected(plt_err_code(res));
        }
    }
    else {
        return unexpected(ERR_THREADING_INVALID_MUTEX_TYPE);
    }

    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    out.type = type;
    return {};
}

expected<plt_err_code> mutex_destroy(mutex& m) {
    i32 res = pthread_mutex_destroy(&m.native);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<plt_err_code> mutex_lock(mutex& m) {
    i32 res = pthread_mutex_lock(&m.native);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<plt_err_code> mutex_trylock(mutex& m) {
    i32 res = pthread_mutex_trylock(&m.native);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<plt_err_code> mutex_unlock(mutex& m) {
    i32 res = pthread_mutex_unlock(&m.native);
    if (res != 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

#pragma endregion

} // namespace core

