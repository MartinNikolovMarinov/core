#include <std/plt.h>

#include <stdlib.h>

// TODO: Implement missign functions

namespace core {

file_desc::file_desc()           : desc((void*)(-1)) {}
file_desc::file_desc(void* desc) : desc(desc) {}

u64 file_desc::to_u64() { return (u64)desc; }

expected<void*, plt_err_code> os_alloc_pages(ptr_size) {
    return unexpected(0);
}
expected<plt_err_code> os_dealloc_pages(void*, ptr_size) {
    return unexpected(0);
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    return unexpected(0);
}
void os_thread_sleep(u64) {
}

expected<file_desc, plt_err_code> os_open(const char*, u64, u64) {
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

namespace {
AtExitCb g_atExit;
} // namespace

void os_exit(i32 exitCode) {
    if (g_atExit) g_atExit(exitCode);
    return _exit(exitCode);
}

void at_exit(AtExitCb atExit) {
    g_atExit = atExit;
}

const char* os_get_err_cptr(plt_err_code) {
    return "Not implemented";
}

} // namespace core
