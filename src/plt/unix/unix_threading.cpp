#include <plt/core_threading.h>
#include <plt/unix/unix_threading.h>

#include <core_cstr.h>
#include <core_exec_ctx.h>
#include <core_system_checks.h>

#include <unistd.h>
#include <signal.h>

namespace core {

// Mutex Implementation

expected<PltErrCode> mutexInit(Mutex& out, MutexType type) noexcept {
    i32 res = 0;

    if (type == MutexType::ErrorCheck || type == MutexType::Recursive) {
        pthread_mutexattr_t attr;
        res = pthread_mutexattr_init(&attr);
        if (res != 0) {
            return core::unexpected(PltErrCode(res));
        }

        defer {
            // At the end of this scope the attr must be destroyed.
            res = pthread_mutexattr_destroy(&attr);
        };

        res = (type == MutexType::ErrorCheck) ?
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) :
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        if (res != 0) {
            return core::unexpected(PltErrCode(res));
        }

        res = pthread_mutex_init(&out.handle, &attr);
    }
    else {
        res = pthread_mutex_init(&out.handle, nullptr);
    }

    if (res != 0) {
        out = Mutex();
        return core::unexpected(PltErrCode(res));
    }

    return {};
}

expected<PltErrCode> mutexDestroy(Mutex& m) noexcept {
    i32 res = pthread_mutex_destroy(&m.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> mutexLock(Mutex& m) noexcept {
    i32 res = pthread_mutex_lock(&m.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> mutexTrylock(Mutex& m) noexcept {
    i32 res = pthread_mutex_trylock(&m.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> mutexUnlock(Mutex& m) noexcept {
    i32 res = pthread_mutex_unlock(&m.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

// Thread Implementation

expected<i32, PltErrCode> threadingGetNumCores() noexcept {
    auto n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 0) {
        // sysconf sets errno on errors.
        return core::unexpected(PltErrCode(errno));
    }
    return i32(n);
}

expected<PltErrCode> threadingGetCurrent(Thread& out) noexcept {
    if (out.isRunning) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }

    if (auto res = core::threadInit(out); res.hasErr()) {
        return core::unexpected(res.err());
    }

    out.handle = pthread_self();
    out.isRunning = true;
    return {};
}

namespace {

timespec timespecFromMiliseconds(u64 ms) noexcept {
    timespec ts;
    ts.tv_sec = time_t(ms / u64(1000)); // ms to seconds
    ts.tv_nsec = time_t((ms % u64(1000)) * u64(1000000)); // takes the ms remainder and converts it to ns
    return ts;
}

} // namespace

expected<PltErrCode> threadingSleep(u64 ms) noexcept {
    timespec ts = timespecFromMiliseconds(ms);
    auto ret = nanosleep(&ts, nullptr);
    if (ret != 0) {
        return core::unexpected(PltErrCode(errno));
    }
    return {};
}

// TODO: [SYMBOL SUPPORT COMPILATION CHECK] Relying on _np functions is dangerous. I should detect if these are
//       supported on the current platform. This is tedious, but a good staring point is written here in the SRT
//       project: https://github.com/Haivision/srt/blob/4a8067cf38656926494ae6b150cf7d148213c024/scripts/FindPThreadGetSetName.cmake#L59

expected<PltErrCode> threadingSetName(const char* name) noexcept {
    if (name == nullptr) {
        return core::unexpected(ERR_THREADING_INVALID_THREAD_NAME);
    }
    addr_size len = core::cstrLen(name);
    if (len > MAX_THREAD_NAME_LENGTH) {
        return core::unexpected(ERR_THREADING_INVALID_THREAD_NAME);
    }

#if defined(OS_MAC) && OS_MAC == 1
    i32 res = pthread_setname_np(name);
#else
    i32 res = pthread_setname_np(pthread_self(), name);
#endif

    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }

    return {};
}

expected<PltErrCode> threadingGetName(char out[MAX_THREAD_NAME_LENGTH]) noexcept {
    i32 res = pthread_getname_np(pthread_self(), out, MAX_THREAD_NAME_LENGTH);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

void threadingExit(i32 code) noexcept {
    pthread_exit(reinterpret_cast<void*>(code));
}

core::expected<PltErrCode> threadInit(Thread& t) noexcept {
    t.handle = pthread_t();
    t.isRunning = false;
    auto ret = mutexInit(t.mu);
    if (ret.hasErr()) {
        return core::unexpected(ret.err());
    }
    return ret;
}

namespace {

struct ThreadInfo {
    ThreadRoutine routine;
    void* arg;
    void (*destroy)(void*);
};

inline void* proxy(void* arg) {

    ThreadInfo* tinfo = reinterpret_cast<ThreadInfo*>(arg);
    tinfo->routine(tinfo->arg);
    tinfo->destroy(tinfo);

    // NOTE: If the routine crashes or throws an unhandled exception the thread info will be leaked. It is better to
    //       prevent such cases in the routine itself.

    return nullptr;
}

bool threadIsRunningImpl(const Thread& t) noexcept {
    if (t.handle == pthread_t()) {
        return false;
    }
    i32 res = pthread_kill(t.handle, 0);
    return res == 0;
}

} // namespace

expected<PltErrCode> threadStart(Thread& out, void* arg, ThreadRoutine routine) noexcept {
    if (routine == nullptr) {
        return core::unexpected(ERR_INVALID_ARGUMENT);
    }

    Expect(mutexLock(out.mu));
    defer { Expect(mutexUnlock(out.mu)); };

    if (out.isRunning) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }
    if (threadIsRunningImpl(out)) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }

    auto& actx = core::getAllocator(core::DEFAULT_ALLOCATOR_ID);
    ThreadInfo* tinfo = reinterpret_cast<ThreadInfo*>(actx.alloc(1, sizeof(ThreadInfo)));
    tinfo->routine = routine;
    tinfo->arg = arg;
    tinfo->destroy = [] (void* ptr) {
        auto& aactx = core::getAllocator(core::DEFAULT_ALLOCATOR_ID);
        ThreadInfo* info = reinterpret_cast<ThreadInfo*>(ptr);
        aactx.free(info, 1, sizeof(ThreadInfo));
    };
    if (tinfo == nullptr) {
        return core::unexpected(ERR_ALLOCATOR_DEFAULT_NO_MEMORY);
    }

    // If isRunning is set after the thread is actually created it is possible that the thread will be joined before
    // the isRunning flag is set. So setting it here and in the exceptional case of pthread_create failing it will be
    // set to false again.
    out.isRunning = true;
    i32 res = pthread_create(&out.handle, nullptr, proxy, reinterpret_cast<void*>(tinfo));
    if (res != 0) {
        out.handle = pthread_t();
        out.isRunning = false;
        actx.free(tinfo, 1, sizeof(ThreadInfo));
        return core::unexpected(PltErrCode(res));
    }

    return {};
}

bool threadIsRunning(const Thread& t) noexcept {
    if (!t.isRunning) return false;
    Expect(mutexLock(t.mu));
    defer { Expect(mutexUnlock(t.mu)); };
    if (!t.isRunning) return false; // looks schizophrenic, but it is possible that the thread was joined in the meantime.
    return threadIsRunningImpl(t);
}

expected<bool, PltErrCode> threadEq(const Thread& t1, const Thread& t2) noexcept {
    bool ret = (pthread_equal(t1.handle, t2.handle) != 0);
    return ret;
}

expected<PltErrCode> threadJoin(Thread& t) noexcept {
    Expect(mutexLock(t.mu));

    if (!t.isRunning) {
        Expect(mutexUnlock(t.mu));
        return core::unexpected(ERR_THREAD_IS_NOT_JOINABLE_OR_DETACHABLE);
    }

    i32 res = pthread_join(t.handle, nullptr);
    if (res != 0) {
        Expect(mutexUnlock(t.mu));
        return core::unexpected(PltErrCode(res));
    }

    t.handle = pthread_t();
    t.isRunning = false;

    Expect(mutexUnlock(t.mu));
    Expect(mutexDestroy(t.mu));

    return {};
}

expected<PltErrCode> threadDetach(Thread& t) noexcept {
    Expect(mutexLock(t.mu));

    if (!t.isRunning) {
        Expect(mutexUnlock(t.mu));
        return core::unexpected(ERR_THREAD_IS_NOT_JOINABLE_OR_DETACHABLE);
    }

    i32 res = pthread_detach(t.handle);
    if (res != 0) {
        Expect(mutexUnlock(t.mu));
        return core::unexpected(PltErrCode(res));
    }

    t.handle = pthread_t();
    t.isRunning = false;

    Expect(mutexUnlock(t.mu));
    Expect(mutexDestroy(t.mu));

    return {};
}

// Condition Variable Implementation

expected<PltErrCode> condVarInit(CondVariable& out) noexcept {
    i32 res = pthread_cond_init(&out.handle, nullptr);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> condVarDestroy(CondVariable& cv) noexcept {
    i32 res = pthread_cond_destroy(&cv.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> condVarWaitTimed(CondVariable& cv, Mutex& m, u64 ms) noexcept {
    // I would like to use a monotonic clock here, but there are some inconsistency between MAC and Linux. MAC does not
    // support pthread_condattr_setclock, so realtime is the only cross-platform option.

    // Get the current time:
    timespec absolute;
    if (i32 res = clock_gettime(CLOCK_REALTIME, &absolute); res != 0) {
        return core::unexpected(PltErrCode(res));
    }

    // Convert the relative miliseconds to a timespec:
    timespec relative = timespecFromMiliseconds(ms);

    // Add the relative time interval:
    absolute.tv_sec += relative.tv_sec;
    absolute.tv_nsec += relative.tv_nsec;

    // tv_nsec can be at most 999999999 anything above that is overflow.
    // This is handled by adding the overflow in tv_nsec to tv_sec.
    absolute.tv_sec += absolute.tv_nsec / 1000000000;
    absolute.tv_nsec %= 1000000000; // remove the overflow from tv_nsec

    if (i32 res = pthread_cond_timedwait(&cv.handle, &m.handle, &absolute); res != 0) {
        return core::unexpected(PltErrCode(res));
    }

    return {};
}

expected<PltErrCode> condVarSignal(CondVariable& cv) noexcept {
    i32 res = pthread_cond_signal(&cv.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

expected<PltErrCode> condVarBroadcast(CondVariable& cv) noexcept {
    i32 res = pthread_cond_broadcast(&cv.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

} // namespace core
