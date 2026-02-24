#include <core_alloc.h>
#include <core_assert.h>
#include <core_mem.h>

#include <cstdlib>

namespace core {

StdStatsAllocator::StdStatsAllocator() : oomHandler(getDefaultOOMHandler()), m_totalMemoryAllocated(0), m_inUseMemory(0) {}

StdStatsAllocator::StdStatsAllocator(StdStatsAllocator&& other) {
    m_totalMemoryAllocated.store(other.m_totalMemoryAllocated.load());
    m_inUseMemory.store(other.m_inUseMemory.load());

    other.m_totalMemoryAllocated.store(0);
    other.m_inUseMemory.store(0);
}

void* StdStatsAllocator::alloc(addr_size count, addr_size size) {
    Assert(count > 0 && size > 0, "Invalid Argument");

    addr_size effectiveSize = count * size;
    void* ret = std::malloc(effectiveSize);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }

    effectiveSize = core::align(effectiveSize);
    m_totalMemoryAllocated.fetch_add(effectiveSize);
    m_inUseMemory.fetch_add(effectiveSize);
    return ret;
}

void* StdStatsAllocator::calloc(addr_size count, addr_size size) {
    Assert(count > 0 && size > 0, "Invalid Argument");

    void* ret = std::calloc(count, size);

    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }

    addr_size effectiveSize = core::align(count * size);
    m_totalMemoryAllocated.fetch_add(effectiveSize);
    m_inUseMemory.fetch_add(effectiveSize);
    return ret;
}

void* StdStatsAllocator::realloc(
    void* ptr,
    addr_size newCount, addr_size newSize,
    addr_size oldCount, addr_size oldSize
) {
    Assert(newCount > 0 && newSize > 0, "Invalid Argument");

    addr_size newSizeBytes = newCount * newSize;
    void* ret = std::realloc(ptr, newSizeBytes);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }

    addr_size effectiveOldSize = core::align(oldCount * oldSize);
    addr_size effectiveNewSize = core::align(newSizeBytes);

    if (effectiveNewSize > effectiveOldSize) {
        addr_size effectiveDiff = effectiveNewSize - effectiveOldSize;
        m_totalMemoryAllocated.fetch_add(effectiveDiff);
        m_inUseMemory.fetch_add(effectiveDiff);
    }
    else if (effectiveOldSize > effectiveNewSize) {
        addr_size effectiveDiff = effectiveOldSize - effectiveNewSize;
        m_inUseMemory.fetch_sub(effectiveDiff);
    }

    return ret;
}

void StdStatsAllocator::free(void* ptr, addr_size count, addr_size size) {
    Assert(count > 0 && size > 0, "Invalid Argument");

    addr_size effectiveSize = core::align(count * size);
    m_inUseMemory.fetch_sub(effectiveSize);
    std::free(ptr);
}

void StdStatsAllocator::clear() {
    m_totalMemoryAllocated.store(0);
    m_inUseMemory.store(0);
}
addr_size StdStatsAllocator::totalMemoryAllocated() { return m_totalMemoryAllocated.load(); }
addr_size StdStatsAllocator::inUseMemory() { return m_inUseMemory.load(); }

} // namespace core
