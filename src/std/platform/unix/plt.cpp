#include <plt.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

// NOTE: POSIX requires that errno is threadsafe!

namespace core::plt
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

CORE_API_EXPORT core::Tuple<FileDesc, PltErr> OsOpen(const char* path, u64 flag, u64 mode) {
    i32 ret = open(path, flag, mode);
    if (ret < 0) {
        return {0, errno};
    }

    return { FileDesc{(void*)(u64)ret}, PltErr::Ok };
}

core::Tuple<i64, PltErr> OsRead(FileDesc fd, void* buf, u64 size) {
    i64 ret = read(i32(fd.ToU64()), buf, size);
    if (ret < 0) {
        return {0, errno};
    }

    return {ret, PltErr::Ok};
}


PltErr OsClose(FileDesc fd) {
    i32 ret = close(i32(fd.ToU64()));
    if (ret < 0) {
        return errno;
    }

    return PltErr::Ok;
}

} // namespace core::plt
