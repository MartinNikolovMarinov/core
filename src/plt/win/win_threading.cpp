#include <plt/core_threading.h>
#include <plt/win/win_threading.h>

#include <windows.h>

namespace core {

expected<i32, PltErrCode> threadingGetNumCores() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    i32 n = i32(sysInfo.dwNumberOfProcessors);
    if (n <= 0) {
        // sysconf sets errno on errors.
        return core::unexpected(PltErrCode(GetLastError()));
    }

    return n;
}

expected<PltErrCode> threadingGetCurrent(Thread& out) {
    // For Windows, the GetCurrentThread() returns a pseudo handle to the current thread and not an actual handle.
    // To convert this pseudo handle into a real handle, DuplicateHandle() is used.
    HANDLE pseudoHandle = GetCurrentThread();
    bool ret = DuplicateHandle(GetCurrentProcess(), pseudoHandle, GetCurrentProcess(), &out.handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if (!ret) {
        out.handle = nullptr;
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}

void threadingExit(i32 exitCode) {
    ExitThread(DWORD(exitCode));
}

expected<PltErrCode> threadingSleep(u64 ms) {
    Panic(ms <= u64(DWORD_MAX) && "Sleep time is too large");
    Sleep(DWORD(ms));
    return {};
}

expected<PltErrCode> threadingSetName(const char* name) {
    if (name == nullptr) {
        return core::unexpected(ERR_THREADING_INVALID_THREAD_NAME);
    }
    addr_size len = core::cptrLen(name);
    if (len > MAX_THREAD_NAME_LENGTH) {
        return core::unexpected(ERR_THREADING_INVALID_THREAD_NAME);
    }

    wchar_t wname[MAX_THREAD_NAME_LENGTH + 1];
    int ret = MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, MAX_THREAD_NAME_LENGTH + 1);
    if (ret <= 0) { // Check for error in conversion
        return core::unexpected(PltErrCode(GetLastError()));
    }
    Assert(ret <= MAX_THREAD_NAME_LENGTH + 1, "MultiByteToWideChar() returned a value greater than the buffer size");

    HRESULT hr = SetThreadDescription(GetCurrentThread(), wname);
    if (FAILED(hr)) {
        return core::unexpected(PltErrCode(hr)); // Directly use HRESULT for error code
    }

    return {};
}

expected<PltErrCode> threadingGetName(char out[MAX_THREAD_NAME_LENGTH]) {
    PWSTR wname = nullptr;
    HRESULT hr = GetThreadDescription(GetCurrentThread(), &wname);
    if (FAILED(hr)) {
        return core::unexpected(PltErrCode(hr));
    }

    int ret = WideCharToMultiByte(CP_UTF8, 0, wname, -1, out, MAX_THREAD_NAME_LENGTH, nullptr, nullptr);

    LocalFree(wname); // Free the memory allocated by GetThreadDescription !

    if (ret <= 0) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    Assert(ret <= MAX_THREAD_NAME_LENGTH, "WideCharToMultiByte() returned a value greater than the buffer size");

    return {};
}

core::expected<PltErrCode> threadInit(Thread&) {
    // FIXME: Initialize the mutex here.
    return {};
}

bool threadIsRunning(const Thread& t) {
    // FIXME: Mutex Lock Needed here.

    if (t.handle == nullptr) {
        return false;
    }
    DWORD exitCode = 0;
    if (GetExitCodeThread(t.handle, &exitCode)) {
        return (exitCode == STILL_ACTIVE);
    }
    return false; // If GetExitCodeThread fails, assume the thread is not running.
}

expected<PltErrCode> threadStart(Thread& out, void* arg, ThreadRoutine routine) {
    // FIXME: Mutex Lock Needed here.

    if (threadIsRunning(out)) {
        return core::unexpected(ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD);
    }

    out.handle = CreateThread(nullptr, 0, routine, arg, 0, nullptr);
    if (out.handle == nullptr) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    out.isJoinable = true;
    return {};
}

expected<bool, PltErrCode> threadEq(const Thread& t1, const Thread& t2) {
    // FIXME: Mutex Lock Needed here.
    bool ret = (t1.handle == t2.handle);
    return ret;
}

expected<PltErrCode> threadJoin(Thread& t) {
    // FIXME: Mutex Lock Needed here.

    if (!t.isJoinable) {
        return core::unexpected(ERR_THREAD_IS_NOT_JOINABLE);
    }

    DWORD res = WaitForSingleObject(t.handle, INFINITE);
    if (res == WAIT_FAILED) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    CloseHandle(t.handle);
    t.handle = nullptr;
    t.isJoinable = false;
    // FIXME: Free the mutex here.
    return {};
}

expected<PltErrCode> threadDetach(Thread& t) {
    // FIXME: Mutex Lock Needed here.

    if (!CloseHandle(t.handle)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    t.handle = nullptr;
    t.isJoinable = false;
    // FIXME: Free the mutex here.
    return {};
}

} // namespace core
