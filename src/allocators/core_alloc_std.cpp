#include <core_alloc.h>
#include <plt/core_threading.h>

#include <cstdlib>

namespace core {

namespace {

OOMCallback g_oomCb = nullptr;
AtomicU64 g_totalAllocatedMem = 0;
AtomicU64 g_totalUsedMem = 0;

} // namespace

void* StdAllocator::alloc(addr_size size) noexcept {
    void* ret = std::malloc(size);
    if (ret == nullptr && g_oomCb != nullptr) g_oomCb(nullptr);

    g_totalAllocatedMem.fetch_add(size);
    g_totalUsedMem.fetch_add(size);
    return ret;
}

void* StdAllocator::calloc(addr_size count, addr_size size) noexcept {
    void* ret = std::calloc(count, size);
    if (ret == nullptr && g_oomCb != nullptr) g_oomCb(nullptr);

    g_totalAllocatedMem.fetch_add(count * size);
    g_totalUsedMem.fetch_add(count * size);
    return ret;
}

void StdAllocator::free(void* ptr, addr_size size) noexcept {
    std::free(ptr);
    g_totalUsedMem.fetch_sub(size);
}

void StdAllocator::clear() noexcept {
    g_totalAllocatedMem.store(0);
    g_totalUsedMem.store(0);
}

addr_size StdAllocator::usedMem() noexcept {
    return g_totalUsedMem.load();
}

addr_size StdAllocator::totalAllocatedMem() noexcept {
    return g_totalAllocatedMem.load();
}

bool StdAllocator::isThredSafe() noexcept {
    // Not exactly true because technically setting the global oom handler is not thread safe, but it should be set
    // exactly once in any sane program.
    return true;
}

void StdAllocator::init(OOMCallback cb) noexcept {
    g_oomCb = cb ? cb : DEFAULT_OOM_CALLBACK;
    g_totalAllocatedMem.store(0);
    g_totalUsedMem.store(0);
}

} // namespace core
