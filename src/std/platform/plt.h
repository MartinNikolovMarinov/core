#pragma once

#include <API.h>
#include <types.h>
#include <core_error.h>

namespace core::plt
{

using namespace coretypes;
using PltErr = core::error::ErrorInt;
template<typename T>
using PltErrValue = core::error::ErrorValue<T, PltErr>;

constexpr PltErr OsDeallocNullAddrErr = 1;

CORE_API_EXPORT PltErrValue<void*> OsAllocPages(ptr_size size);
CORE_API_EXPORT PltErr             OsDeallocPages(void *addr, ptr_size size);

CORE_API_EXPORT PltErrValue<u64> OsUnixTimeStampInMs();
CORE_API_EXPORT void             OsThreadSleep(u64 ms);

struct FileDesc {
    // Holds the OS specific file descriptor.
    // On Linux it's a file descriptor and on Windows it's a HANDLE.
    void* desc;

    u64 ToU64() { return (u64)desc; }
};

CORE_API_EXPORT PltErrValue<FileDesc> OsOpen(const char* path, u64 flag, u64 mode);
CORE_API_EXPORT PltErrValue<i64>      OsRead(FileDesc fd, void* buf, u64 size);
CORE_API_EXPORT PltErr                OsClose(FileDesc fd);

} // namespace core::plt
