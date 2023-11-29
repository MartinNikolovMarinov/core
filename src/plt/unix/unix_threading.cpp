#include <plt/core_threading.h>
#include <plt/unix/unix_threading.h>

#include <core_system_checks.h>
#include <core_cptr.h>

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
    out.isRunning = false;
    return {};
}

expected<PltErrCode> threadingSleep(u64 ms) noexcept {
    timespec ts;
    ts.tv_sec = time_t(ms / u64(1000));
    ts.tv_nsec = time_t((ms % u64(1000)) * u64(1000000));
    auto ret = nanosleep(&ts, nullptr);
    if (ret != 0) {
        return core::unexpected(PltErrCode(errno));
    }
    return {};
}

// TODO: Relying on _np functions is dangerous. I should detect if these are supported on the current platform.
//       This is tedious, but a good staring point is written here in the SRT project:
//       https://github.com/Haivision/srt/blob/4a8067cf38656926494ae6b150cf7d148213c024/scripts/FindPThreadGetSetName.cmake#L59

expected<PltErrCode> threadingSetName(const char* name) noexcept {
    if (name == nullptr) {
        return core::unexpected(ERR_THREADING_INVALID_THREAD_NAME);
    }
    addr_size len = core::cptrLen(name);
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

core::expected<PltErrCode> threadInit(Thread& t) noexcept {
    if (t.canLock.load(std::memory_order_acquire)) {
        return {};
    }
    auto ret = mutexInit(t.mu);
    if (ret.hasErr()) {
        return core::unexpected(ret.err());
    }
    t.canLock.store(true);
    return {};
}

bool threadIsRunning(const Thread& t) noexcept {
    if (!t.canLock.load(std::memory_order_acquire)) {
        return false;
    }

    Expect(mutexLock(t.mu));
    defer { Expect(mutexUnlock(t.mu)); };

    if (t.handle == pthread_t()) {
        return false;
    }
    i32 res = pthread_kill(t.handle, 0);
    return res == 0;
}

expected<bool, PltErrCode> threadEq(const Thread& t1, const Thread& t2) noexcept {
    bool ret = (pthread_equal(t1.handle, t2.handle) != 0);
    return ret;
}

expected<PltErrCode> threadJoin(Thread& t) noexcept {
    if (!t.canLock.load(std::memory_order_acquire)) {
        return core::unexpected(ERR_THREAD_IS_NOT_INITIALIZED);
    }

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

    t.canLock.store(false);

    Expect(mutexUnlock(t.mu));
    Expect(mutexDestroy(t.mu));

    return {};
}

expected<PltErrCode> threadDetach(Thread& t) noexcept {
    if (!t.canLock.load(std::memory_order_acquire)) {
        return core::unexpected(ERR_THREAD_IS_NOT_INITIALIZED);
    }

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

    t.canLock.store(false);

    Expect(mutexUnlock(t.mu));
    Expect(mutexDestroy(t.mu));

    return {};
}

} // namespace core
