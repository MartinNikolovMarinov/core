#include <std/plt.h>

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

namespace core {

file_desc::file_desc()           : desc((void*)(-1)) {}
file_desc::file_desc(void* desc) : desc(desc) {}

u64 file_desc::to_u64() { return (u64)desc; }

expected<void*, plt_err_code> os_alloc_pages(ptr_size size) {
    // flags - memory is private copy-on-write and is not backed by a file, i.e. anonymous
    i32 flags = ( MAP_PRIVATE | MAP_ANONYMOUS );
    // protection - memory is mapped for reading and for writing.
    i32 prot = ( PROT_READ | PROT_WRITE );

    void* addr = mmap(nullptr, size, prot, flags, 0, 0);
    if (addr == MAP_FAILED || addr == nullptr) {
        return unexpected(u64(errno));
    }
    return addr;
}

expected<plt_err_code> os_dealloc_pages(void *addr, ptr_size size) {
    if (addr == nullptr) {
        return unexpected(OS_DEALLOC_NULL_ADDR_ERR);
    }
    i32 ret = munmap(addr, size);
    if (ret < 0) {
        // munmap returns -1 on error and sets errno
        return unexpected(u64(errno));
    }
    return {};
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    struct timeval now;
    i64 ret = gettimeofday(&now, 0);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    u64 timeInMs = ((u64) now.tv_sec) * 1000 + ((u64) now.tv_usec) / 1000;
    return timeInMs;
}

expected<plt_err_code> os_thread_sleep(u64 ms) {
    struct timespec rem;
    struct timespec req = {
        u32((ms / 1000)),/* secs (Must be Non-Negative) */
        u32((ms % 1000) * 1000000) /* nano (Must be in range of 0 to 999999999) */
    };
    i32 ret = nanosleep(&req , &rem);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<file_desc, plt_err_code> os_open(const char* path, i32 flag, i32 mode) {
    i32 ret = open(path, flag, mode);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    file_desc desc = file_desc{ reinterpret_cast<void*>((u64)(ret)) };
    return desc;
}

expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead) {
    bytesRead = read(i32(fd.to_u64()), buf, size);
    if (bytesRead < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten) {
    bytesWritten = write(i32(fd.to_u64()), buf, size);
    if (bytesWritten < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_close(file_desc fd) {
    i32 ret = close(i32(fd.to_u64()));
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_rmfile(const char* path) {
    i32 res = unlink(path);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_mkdir(const char* path, i32 mode) {
    i32 res = mkdir(path, mode);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_rmdir(const char* path) {
    i32 res = rmdir(path);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<bool, plt_err_code> os_exists(const char* path) {
    i32 res = access(path, F_OK);
    if (res < 0) {
        if (errno == ENOENT) {
            // File was simply not found.
            return false;
        }
        // Some other unexpected error occurred:
        return unexpected(u64(errno));
    }
    return true;
}

namespace {
AtExitCb g_atExit;
} // namespace

void os_exit(i32 exitCode) {
    if (g_atExit) g_atExit(exitCode);
    _exit(exitCode);
}

void at_exit(AtExitCb atExit) {
    g_atExit = atExit;
}

CORE_API_EXPORT const char* os_get_err_cptr(plt_err_code err) {
    return std::strerror(i32(err));
}

} // namespace core::plt
