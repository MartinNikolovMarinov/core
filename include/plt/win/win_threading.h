#pragma once

#include <plt/core_threading.h>

#include <windows.h>

namespace core {

struct Mutex {
    CRITICAL_SECTION cs;
};

struct Thread {
    HANDLE handle;
    bool isRunning;
    mutable Mutex mu;
    AtomicBool canLock; // FIXME: The way I use this does not guarnatee a 100% thread safe behavior.

    NO_COPY(Thread)

    Thread() noexcept : handle(nullptr), isRunning(false), mu(), canLock(false) {}

    Thread(Thread&& other) noexcept // NOTE: Very much NOT thread safe!
        : handle(other.handle)
        , isRunning(other.isRunning)
        , mu(core::move(other.mu))
        , canLock(other.canLock.load()) {}

    Thread& operator=(Thread&& other) noexcept { // NOTE: Very much NOT thread safe!
        handle = other.handle;
        isRunning = other.isRunning;
        mu = core::move(other.mu);
        canLock = other.canLock.load();
        other.handle = nullptr;
        other.isRunning = false;
        return *this;
    }
};

namespace {

struct ThreadInfo {
    ThreadRoutine routine;
    void* arg;
    void (*destroy)(void*);
};

DWORD WINAPI proxy(void* arg) {

    ThreadInfo* tinfo = reinterpret_cast<ThreadInfo*>(arg);
    tinfo->routine(tinfo->arg);
    tinfo->destroy(tinfo);

    // NOTE: If the routine crashes or throws an unhandled exception the thread info will be leaked. It is better to
    //       prevent such cases in the routine itself.

    return 0;
}

} // namspace

template <typename TAlloc>
expected<PltErrCode> threadStart(Thread& out, void* arg, ThreadRoutine routine) noexcept {
    if (!out.canLock.load(std::memory_order_acquire)) {
        return core::unexpected(ERR_THREAD_IS_NOT_INITIALIZED);
    }

    Expect(mutexLock(out.mu));
    defer { Expect(mutexUnlock(out.mu)); };

    if (threadIsRunning(out)) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }

    ThreadInfo* tinfo = reinterpret_cast<ThreadInfo*>(TAlloc::alloc(sizeof(ThreadInfo)));
    tinfo->routine = routine;
    tinfo->arg = arg;
    tinfo->destroy = [] (void* ptr) {
        ThreadInfo* tinfo = reinterpret_cast<ThreadInfo*>(ptr);
        TAlloc::free(tinfo);
    };
    if (tinfo == nullptr) {
        return core::unexpected(ERR_ALLOCATOR_DEFAULT_NO_MEMORY);
    }

    out.handle = CreateThread(nullptr, 0, proxy, reinterpret_cast<void*>(tinfo), 0, nullptr);
    if (out.handle == nullptr) {
        TAlloc::free(tinfo);
        return core::unexpected(PltErrCode(GetLastError()));
    }

    out.isRunning = true;
    return {};
}

}; // namespace core
