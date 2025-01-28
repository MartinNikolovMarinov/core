#include <plt/core_process.h>

#include <unistd.h>
#include <errno.h>

namespace core {

expected<u64, PltErrCode> processGetId() noexcept {
    pid_t processId = getpid();

    if (processId <= 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return u64(processId);
}

} // namespace core
