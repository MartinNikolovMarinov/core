#include <plt/core_threading.h>
#include <plt/unix/unix_threading.h>

#include <core_system_checks.h>
#include <core_cptr.h>

#include <unistd.h>
#include <signal.h>

namespace core {

expected<i32, PltErrCode> threadingGetNumCores() {
    auto n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 0) {
        // sysconf sets errno on errors.
        return core::unexpected(PltErrCode(errno));
    }
    return i32(n);
}

expected<PltErrCode> threadingGetCurrent(Thread& out) {
    // FIXME: Mutex Lock Needed here.
    out.handle = pthread_self();
    out.isRunning = false;
    return {};
}

expected<PltErrCode> threadingSleep(u64 ms) {
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

expected<PltErrCode> threadingSetName(const char* name) {
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

expected<PltErrCode> threadingGetName(char out[MAX_THREAD_NAME_LENGTH]) {
    i32 res = pthread_getname_np(pthread_self(), out, MAX_THREAD_NAME_LENGTH);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }
    return {};
}

core::expected<PltErrCode> threadInit(Thread&) {
    // FIXME: Initialize the mutex here.
    return {};
}

bool threadIsRunning(const Thread& t) {
    // FIXME: Mutex Lock Needed here.

    if (t.handle == pthread_t()) {
        return false;
    }
    i32 res = pthread_kill(t.handle, 0);
    return res == 0;
}

expected<PltErrCode> threadStart(Thread& out, void* arg, ThreadRoutine routine) {
    // FIXME: Mutex Lock Needed here.

    if (threadIsRunning(out)) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }

    i32 res = pthread_create(&out.handle, nullptr, routine, arg);
    if (res != 0) {
        out.handle = pthread_t();
        return core::unexpected(PltErrCode(res));
    }

    out.isRunning = true;
    return {};
}

expected<bool, PltErrCode> threadEq(const Thread& t1, const Thread& t2) {
    bool ret = (pthread_equal(t1.handle, t2.handle) != 0);
    return ret;
}

expected<PltErrCode> threadJoin(Thread& t) {
    // FIXME: Mutex Lock Needed here.

    if (!t.isRunning) {
        return core::unexpected(ERR_THREAD_IS_NOT_JOINABLE);
    }

    i32 res = pthread_join(t.handle, nullptr);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }

    t.handle = pthread_t();
    t.isRunning = false;
    // FIXME: Free the mutex here.
    return {};
}

expected<PltErrCode> threadDetach(Thread& t) {
    // FIXME: Mutex Lock Needed here.

    i32 res = pthread_detach(t.handle);
    if (res != 0) {
        return core::unexpected(PltErrCode(res));
    }

    t.handle = pthread_t();
    t.isRunning = false;
    // FIXME: Free the mutex here.
    return {};
}

} // namespace core
