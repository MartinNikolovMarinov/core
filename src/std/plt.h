#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>

namespace core {

using namespace coretypes;

using plt_err_code = i64;

constexpr plt_err_code OS_DEALLOC_NULL_ADDR_ERR = 1;

CORE_API_EXPORT expected<void*, plt_err_code> os_alloc_pages(ptr_size size);
CORE_API_EXPORT expected<plt_err_code>        os_dealloc_pages(void *addr, ptr_size size);

CORE_API_EXPORT expected<u64, plt_err_code> os_unix_time_stamp_in_ms();
CORE_API_EXPORT void                        os_thread_sleep(u64 ms);

// file descriptor
struct CORE_API_EXPORT file_desc {
    // Holds the OS specific file descriptor.
    void* desc;

    file_desc();
    file_desc(void* desc);
    u64 to_u64();
};

using AtExitCb = void (*)(i64 exitCode);

CORE_API_EXPORT void os_exit(i64 exitCode);
CORE_API_EXPORT void at_exit(AtExitCb atExit);

CORE_API_EXPORT expected<file_desc, plt_err_code> os_open(const char* path, u64 flag, u64 mode);
CORE_API_EXPORT expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead);
CORE_API_EXPORT expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten);
CORE_API_EXPORT expected<plt_err_code> os_close(file_desc fd);
CORE_API_EXPORT expected<plt_err_code> os_rmfile(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_mkdir(const char* path, u64 mode);
CORE_API_EXPORT expected<plt_err_code> os_rmdir(const char* path);
CORE_API_EXPORT expected<bool, plt_err_code> os_exists(const char* path);

CORE_API_EXPORT const char* os_get_err_cptr(plt_err_code err);

} // namespace core
