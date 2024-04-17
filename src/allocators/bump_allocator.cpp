#include <core_alloc.h>
#include <core_mem.h>

namespace core {

BumpAllocator::BumpAllocator(void* data, addr_size cap)
    : oomHandler(defaultOOMHandler)
    , m_startAddr(data)
    , m_currentAddr(data)
    , m_cap(cap) {
        Panic(data != nullptr, "Invalid data pointer");
        Panic(cap > 0, "Invalid capacity");
    }

BumpAllocator::BumpAllocator(BumpAllocator&& other) {
    oomHandler = other.oomHandler;
    m_startAddr = other.m_startAddr;
    m_currentAddr = other.m_currentAddr;
    m_cap = other.m_cap;

    other.m_startAddr = nullptr;
    other.m_currentAddr = nullptr;
    other.m_cap = 0;
}

void* BumpAllocator::alloc(addr_size count, addr_size size) {
    addr_size effectiveSize = count * size;
    effectiveSize = core::align(effectiveSize);
    if (core::ptrDiff(m_currentAddr, m_startAddr) + effectiveSize > m_cap) {
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    void* ret = m_currentAddr;
    m_currentAddr = core::ptrAdvance(m_currentAddr, effectiveSize);
    return ret;
}

void* BumpAllocator::calloc(addr_size count, addr_size size) {
    addr_size effectiveSize = count * size;
    effectiveSize = core::align(effectiveSize);
    if (core::ptrDiff(m_currentAddr, m_startAddr) + effectiveSize > m_cap) {
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    void* ret = m_currentAddr;
    m_currentAddr = core::ptrAdvance(m_currentAddr, effectiveSize);
    core::memset(ret, 0, effectiveSize);
    return ret;
}

void BumpAllocator::free(void*, addr_size, addr_size) {}

void BumpAllocator::clear() {
    m_currentAddr = m_startAddr;
}

addr_size BumpAllocator::totalMemoryAllocated() {
    return inUseMemory();
}

addr_size BumpAllocator::inUseMemory() {
    addr_size ret = core::ptrDiff(m_currentAddr, m_startAddr);
    return ret;
}

} // namespace core
