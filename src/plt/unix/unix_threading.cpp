#include <plt/core_threading.h>

#include <core_system_checks.h>

#include <unistd.h>

namespace core {

expected<i32, PltErrCode> threadingGetNumCores() {
    auto n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 0) {
        // sysconf sets errno on errors.
        return unexpected(PltErrCode(errno));
    }
    return i32(n);
}

} // namespace core
