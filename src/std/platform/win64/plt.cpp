#include <platform/plt.h>

#include <windows.h>
#include <errhandlingapi.h>

// NOTE: GetLastError() is thread safe!

namespace plt
{

core::Tuple<void*, PltErr> OsAllocPages(ptr_size _size) {
    void* addr = VirtualAlloc(null, _size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (addr == nullptr) {
        return {nullptr, GetLastError()};
    }
    return {addr, PltErr::Ok};
}

PltErr OsDeallocPages(void *_addr, ptr_size _size) {
    if (addr == nullptr) {
        return PltErr::DeallocNullAddr;
    }
    // size is ignred for the win64 api. VirtualFree deallocates all memory at the address.
    BOOL ret = VirtualFree(_addr, 0, MEM_RELEASE);
    if (ret == 0) {
        return GetLastError();
    }
    return PltErr::Ok;
}

} // namespace plt
