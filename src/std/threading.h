#pragma once

#include "API.h"
#include "types.h"
#include "expected.h"

#include <std/plt.h>

namespace core {

using namespace coretypes;

struct thread {
    void* native = nullptr;
};

using thread_routine = void* (*)(void*);
using thread_id = u64;

CORE_API_EXPORT expected<i32, plt_err_code> threading_get_num_cores();
CORE_API_EXPORT thread threading_get_current();
CORE_API_EXPORT void threading_exit(i32 exitCode);

CORE_API_EXPORT bool thread_is_running(const thread& t);
CORE_API_EXPORT expected<plt_err_code> thread_start(thread& out, void* arg, thread_routine routine);
CORE_API_EXPORT thread_id thread_get_id(const thread& t);
CORE_API_EXPORT expected<bool, plt_err_code> thread_eq(const thread& t1, const thread& t2);
CORE_API_EXPORT expected<plt_err_code> thread_join(thread& t);
CORE_API_EXPORT expected<plt_err_code> thread_detach(thread& t);

// TODO: I already have support for everything the c++ std library has for threads, but I want to consider:
//       - kill a.k.a. sending singals to threads.
//       - thread attributes (maybe*).

} // namespace core

