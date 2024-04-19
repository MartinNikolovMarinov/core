#include <core_alloc.h>
#include <core_mem.h>

#include <cstdlib>

namespace core {

StdStatsAllocator::StdStatsAllocator() : oomHandler(defaultOOMHandler), m_totalMemoryAllocated(0), m_inUseMemory(0) {}

StdStatsAllocator::StdStatsAllocator(StdStatsAllocator&& other) {
    m_totalMemoryAllocated.store(other.m_totalMemoryAllocated.load());
    m_inUseMemory.store(other.m_inUseMemory.load());

    other.m_totalMemoryAllocated.store(0);
    other.m_inUseMemory.store(0);
}

void* StdStatsAllocator::alloc(addr_size count, addr_size size) {
    Panic(count > 0 && size > 0, "Invalid allocation size");

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
    Panic(count > 0 && size > 0, "Invalid zero allocation size");

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

void StdStatsAllocator::free(void* ptr, addr_size count, addr_size size) {
    Panic(count > 0 && size > 0, "Invalid free size");

    addr_size effectiveSize = core::align(count * size);
    m_inUseMemory.fetch_sub(effectiveSize);
    std::free(ptr);
}

void StdStatsAllocator::clear() {}
addr_size StdStatsAllocator::totalMemoryAllocated() { return m_totalMemoryAllocated.load(); }
addr_size StdStatsAllocator::inUseMemory() { return m_inUseMemory.load(); }

} // namespace core
