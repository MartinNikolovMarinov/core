#include <std/plt.h>

#include <stdlib.h>

// TODO: [Windows Suppport] Implement missign functions

namespace core {

file_desc::file_desc()             : native((void*)(-1)) {}
file_desc::file_desc(void* native) : native(native) {}

u64 file_desc::to_u64() { return (u64)native; }

expected<void*, plt_err_code> os_alloc_pages(addr_size) {
    return unexpected(0);
}
expected<plt_err_code> os_dealloc_pages(void*, addr_size) {
    return unexpected(0);
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    return unexpected(0);
}

expected<file_desc, plt_err_code> os_open(const char*, i32, i32) {
    return unexpected(0);
}
expected<plt_err_code> os_read(file_desc, void*, u64, i64&) {
    return unexpected(0);
}
expected<plt_err_code> os_write(file_desc, const void*, u64, i64&) {
    return unexpected(0);
}
expected<plt_err_code> os_close(file_desc) {
    return unexpected(0);
}
expected<plt_err_code> os_rmfile(const char*) {
    return unexpected(0);
}
expected<plt_err_code> os_rmdir(const char*) {
    return unexpected(0);
}
expected<plt_err_code> os_flush(file_desc fd) {
    // TODO: Use FlushFileBuffers for this !
    return unexpected(0);
}

const char* os_get_err_cptr(plt_err_code) {
    return "Not implemented";
}

} // namespace core
