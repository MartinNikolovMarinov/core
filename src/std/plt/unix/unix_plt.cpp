#include <std/plt/plt.h>

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

namespace core {

plt_err<void*> os_alloc_pages(ptr_size size) {
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

plt_err<plt_success_code> os_dealloc_pages(void *addr, ptr_size size) {
    if (addr == nullptr) {
        return unexpected(OS_DEALLOC_NULL_ADDR_ERR);
    }
    i32 ret = munmap(addr, size);
    if (ret < 0) {
        // munmap returns -1 on error and sets errno
        return unexpected(u64(errno));
    }
    return ret;
}

plt_err<u64> os_unix_time_stamp_in_ms() {
    struct timeval now;
    i64 ret = gettimeofday(&now, 0);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    u64 timeInMs = ((u64) now.tv_sec) * 1000 + ((u64) now.tv_usec) / 1000;
    return timeInMs;
}

void os_thread_sleep(u64 ms) {
    usleep(ms * 1000);
}

plt_err<file_desc> os_open(const char* path, u64 flag, u64 mode) {
    i32 ret = open(path, flag, mode);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    file_desc desc = { reinterpret_cast<void*>((u64)(ret)) };
    return desc;
}

plt_err<i64> os_read(file_desc fd, void* buf, u64 size) {
    i64 ret = read(i32(fd.to_u64()), buf, size);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return ret;
}

plt_err<i64> os_write(file_desc fd, const void* buf, u64 size) {
    i64 ret = write(i32(fd.to_u64()), buf, size);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return ret;
}

plt_err<plt_success_code> os_close(file_desc fd) {
    i32 ret = close(i32(fd.to_u64()));
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return ret;
}

} // namespace core::plt
