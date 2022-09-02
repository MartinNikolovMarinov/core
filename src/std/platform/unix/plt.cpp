#include <platform/plt.h>

#include <sys/mman.h>
#include <errno.h>

// NOTE: POSIX requires that errno is threadsafe!

namespace plt
{

core::Tuple<void*, PltErr> OsAllocPages(ptr_size size) {
    // flags - memory is private copy-on-write and is not backed by a file, i.e. anonymous
    i32 flags = ( MAP_PRIVATE | MAP_ANONYMOUS );
    // port - memory is mapped for reading and for writing.
    i32 prot = ( PROT_READ | PROT_WRITE );

    void* addr = mmap(nullptr, size, prot, flags, 0, 0);
    if (addr == MAP_FAILED || addr == nullptr) {
        return {nullptr, errno};
    }

    return {addr, PltErr::Ok};
}

PltErr OsDeallocPages(void *addr, ptr_size size) {
    if (addr == nullptr) {
        return PltErr::DeallocNullAddr;
    }
    i32 ret = munmap(addr, size);
    if (ret < 0) {
        // munmap returns -1 on error and sets errno
        return errno;
    }
    return PltErr::Ok;
}

} // namespace plt
