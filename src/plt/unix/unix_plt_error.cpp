#include <plt/core_plt_error.h>

#include <core_mem.h>
#include <core_cstr.h>

#include <string.h>

namespace core {

bool pltErrorDescribe(PltErrCode err, char out[MAX_SYSTEM_ERR_MSG_SIZE]) {
    const char* errCstr = strerror(i32(err));
    if (errCstr == nullptr) {
        errCstr = "Unknown error";
    }
    addr_size len = core::cstrLen(errCstr);
    if (len > MAX_SYSTEM_ERR_MSG_SIZE) {
        len = MAX_SYSTEM_ERR_MSG_SIZE;
    }
    core::memcopy(out, errCstr, len);
    out[len] = '\0';
    return true;
}

} // namespace core
