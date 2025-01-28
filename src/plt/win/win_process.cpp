#include <plt/core_process.h>

#include <windows.h>

namespace core {

expected<u64, PltErrCode> processGetId() noexcept {
    DWORD processId = GetCurrentProcessId();

    if (processId == 0) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    return u64(processId);
}

} // namespace core
