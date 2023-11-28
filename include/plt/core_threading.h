#pragma once

#include <core_API.h>
#include <core_expected.h>
#include <core_types.h>
#include <plt/core_plt_error.h>

#include <atomic>

namespace core {

using namespace coretypes;

using AtomicChar = std::atomic<char>;
using AtomicI32  = std::atomic<i32>;
using AtomicI64  = std::atomic<i64>;
using AtomicU32  = std::atomic<u32>;
using AtomicU64  = std::atomic<u64>;
using AtomicBool = std::atomic<bool>;
using AtomicPtr  = std::atomic<void*>;

struct Thread;

using ThreadRoutine = void* (*)(void*);

// NOTE: The Linux kernel limits thread names to 16 characters (including the null terminator).
constexpr u32 MAX_THREAD_NAME_LENGTH = 16;

CORE_API_EXPORT expected<i32, PltErrCode> threadingGetNumCores();
CORE_API_EXPORT expected<PltErrCode>      threadingGetCurrent(Thread& out);
CORE_API_EXPORT void                      threadingExit(i32 exitCode);
CORE_API_EXPORT expected<PltErrCode>      threadingSleep(u64 ms);
CORE_API_EXPORT expected<PltErrCode>      threadingSetName(const char* name);
CORE_API_EXPORT expected<PltErrCode>      threadingGetName(char out[MAX_THREAD_NAME_LENGTH]);

CORE_API_EXPORT expected<PltErrCode>       threadInit(Thread& t);
CORE_API_EXPORT bool                       threadIsRunning(const Thread& t);
CORE_API_EXPORT expected<PltErrCode>       threadStart(Thread& out, void* arg, ThreadRoutine routine);
CORE_API_EXPORT expected<bool, PltErrCode> threadEq(const Thread& t1, const Thread& t2);
CORE_API_EXPORT expected<PltErrCode>       threadJoin(Thread& t);
CORE_API_EXPORT expected<PltErrCode>       threadDetach(Thread& t);

} // namespace core

#if defined(OS_WINDOWS) && OS_WINDOWS == 1
    #include <plt/win/win_threading.h>
#elif defined(OS_LINUX) && OS_LINUX == 1
    #include <plt/unix/unix_threading.h>
#elif defined(OS_MACOS) && OS_MACOS == 1
    #include <plt/unix/unix_threading.h>
#endif
