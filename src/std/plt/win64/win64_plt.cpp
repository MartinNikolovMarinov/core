#include <std/plt.h>

namespace core {

file_desc::file_desc()           : desc((void*)(-1)) {}
file_desc::file_desc(void* desc) : desc(desc) {}

u64 file_desc::to_u64() { return (u64)desc; }

expected<void*, plt_err_code> os_alloc_pages(ptr_size) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_dealloc_pages(void*, ptr_size) {
    // TODO: not implemented yet.
    return unexpected(0);
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    // TODO: not implemented yet.
    return unexpected(0);
}
void os_thread_sleep(u64) {
    // TODO: not implemented yet.
}

expected<file_desc, plt_err_code> os_open(const char*, u64, u64) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_read(file_desc, void*, u64, i64&) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_write(file_desc, const void*, u64, i64&) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_close(file_desc) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_rmfile(const char*) {
    // TODO: not implemented yet.
    return unexpected(0);
}
expected<plt_err_code> os_rmdir(const char*) {
    // TODO: not implemented yet.
    return unexpected(0);
}

};