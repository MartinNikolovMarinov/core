#include <plt/core_threading.h>

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

} // namespace core
