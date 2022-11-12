#include "../plt.h"
#include <utils.h> // TODO: remove when there is no need for Assert!

#include <windows.h>
#include <errhandlingapi.h>


// NOTE: GetLastError() is thread safe!
// Also Microsoft uses 64 bit integer for the error value, but the error codes do not exceed 16000.

namespace core::plt
{

PltErrValue<void*> OsAllocPages(ptr_size size) {
    void* addr = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (addr == nullptr) {
        return { nullptr, i32(GetLastError()) };
    }
    return { std::move(addr), PltErr::Ok };
}

PltErr OsDeallocPages(void *addr, ptr_size) {
    if (addr == nullptr) {
        return OsDeallocNullAddrErr;
    }
    // size is ignored for the win64 api. VirtualFree deallocates all memory at the address.
    BOOL ret = VirtualFree(addr, 0, MEM_RELEASE);
    if (ret == 0) {
        return i32(GetLastError());
    }
    return PltErr::Ok;
}

PltErrValue<FileDesc> OsOpen(const char*, u64, u64) {
    Assert(false, "Not implemented yet");
    return { };
}

PltErrValue<i64> OsRead(FileDesc, void*, u64) {
    Assert(false, "Not implemented yet");
    return { };
}

PltErrValue<i64> OsWrite(FileDesc, const void*, u64) {
    Assert(false, "Not implemented yet");
    return { };
}

PltErr OsClose(FileDesc) {
    Assert(false, "Not implemented yet");
    return { };
}

} // namespace core::plt
