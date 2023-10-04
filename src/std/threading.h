#pragma once

#include "API.h"
#include "types.h"
#include "expected.h"

#include <std/plt.h>

#include <atomic>

namespace core {

using namespace coretypes;

struct thread;

using core_atomic_char = std::atomic<char>;
using core_atomic_i32 = std::atomic<i32>;
using core_atomic_i64 = std::atomic<i64>;
using core_atomic_u32 = std::atomic<u32>;
using core_atomic_u64 = std::atomic<u64>;
using core_atomic_bool = std::atomic<bool>;
using core_atomic_ptr = std::atomic<void*>;

using thread_routine = void* (*)(void*);

CORE_API_EXPORT expected<i32, plt_err_code> threading_get_num_cores();
CORE_API_EXPORT thread threading_get_current();
CORE_API_EXPORT void threading_exit(i32 exitCode);

CORE_API_EXPORT bool thread_is_running(const thread& t);
CORE_API_EXPORT expected<plt_err_code> thread_start(thread& out, void* arg, thread_routine routine);
CORE_API_EXPORT thread_id thread_get_id(const thread& t);
CORE_API_EXPORT expected<bool, plt_err_code> thread_eq(const thread& t1, const thread& t2);
CORE_API_EXPORT expected<plt_err_code> thread_join(thread& t);
CORE_API_EXPORT expected<plt_err_code> thread_detach(thread& t);

// TODO2: I might consider an api for setting some threading attributes, like stack size, priority, etc.
//        At least the ones that are portable. I might leave that to the user to do through the native handle.

struct mutex;

enum struct mutex_type : u8 {
    Normal,
    Recursive,
    ErrorCheck
};

CORE_API_EXPORT expected<plt_err_code> mutex_init(mutex& out, mutex_type type = mutex_type::Normal);
CORE_API_EXPORT expected<plt_err_code> mutex_destroy(mutex& m);
CORE_API_EXPORT expected<plt_err_code> mutex_lock(mutex& m);
CORE_API_EXPORT expected<plt_err_code> mutex_trylock(mutex& m);
CORE_API_EXPORT expected<plt_err_code> mutex_unlock(mutex& m);

} // namespace core

#if OS_LINUX == 1 || OS_MAC == 1
#include <std/plt/unix/unix_threading.h>
#endif
