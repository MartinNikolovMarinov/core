#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>

namespace core {

using namespace coretypes;

using plt_err_code = u64;
using plt_success_code = u64;
template<typename T>
using plt_err = expected<T, plt_err_code>;

constexpr plt_err_code OS_DEALLOC_NULL_ADDR_ERR = 1;

CORE_API_EXPORT plt_err<void*>            os_alloc_pages(ptr_size size);
CORE_API_EXPORT plt_err<plt_success_code> os_dealloc_pages(void *addr, ptr_size size);

CORE_API_EXPORT plt_err<u64> os_unix_time_stamp_in_ms();
CORE_API_EXPORT void         os_thread_sleep(u64 ms);

// file descriptor
struct file_desc {
    // Holds the OS specific file descriptor.
    // On Linux it's a file descriptor and on Windows it's a HANDLE.
    void* desc;

    u64 to_u64() { return (u64)desc; }
};

CORE_API_EXPORT plt_err<file_desc>        os_open(const char* path, u64 flag, u64 mode);
CORE_API_EXPORT plt_err<i64>              os_read(file_desc fd, void* buf, u64 size);
CORE_API_EXPORT plt_err<i64>              os_write(file_desc fd, const void* buf, u64 size);
CORE_API_EXPORT plt_err<plt_success_code> os_close(file_desc fd);

} // namespace core
