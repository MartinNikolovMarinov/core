#pragma once

#include <API.h>
#include <core.h>

namespace core::plt
{

using namespace coretypes;

struct CORE_API_EXPORT PltErr {
    static const PltErr Ok;
    static const PltErr DeallocNullAddr;

    i32 code;

    constexpr PltErr() : code(0) {}
    constexpr PltErr(i32 code) : code(code) {}

    constexpr explicit operator i32() const { return code; }

    constexpr bool isOk() const { return code == 0; }
};
constexpr PltErr PltErr::Ok              = PltErr(0);
constexpr PltErr PltErr::DeallocNullAddr = PltErr(-1);

// compare operators with int
constexpr bool operator==(i32 a, PltErr b)    { return a == b.code; }
constexpr bool operator==(PltErr a, i32 b)    { return b == a; }
constexpr bool operator==(PltErr a, PltErr b) { return a.code == b.code; }

CORE_API_EXPORT core::Tuple<void*, PltErr> OsAllocPages  (ptr_size size);
CORE_API_EXPORT PltErr                     OsDeallocPages(void *addr, ptr_size size);

CORE_API_EXPORT core::Tuple<u64, PltErr> UnixTimeStampInMs();
CORE_API_EXPORT void                     ThreadSleep(u64 ms);

struct FileDesc {
    // Holds the OS specific file descriptor.
    // On Linux it's a file descriptor and on Windows it's a HANDLE.
    void* desc;

    u64 ToU64() { return (u64)desc; }
};

CORE_API_EXPORT core::Tuple<FileDesc, PltErr> OsOpen(const char* path, u64 flag, u64 mode);
CORE_API_EXPORT core::Tuple<i64, PltErr>      OsRead(FileDesc fd, void* buf, u64 size);
CORE_API_EXPORT PltErr                        OsClose(FileDesc fd);

} // namespace core::plt
