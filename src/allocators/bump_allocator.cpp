#include <core_alloc.h>
#include <core_mem.h>

namespace core {

namespace {

thread_local addr_size tl_capacity = core::MAX_U64;
thread_local void* tl_startAddr = nullptr;
thread_local void* tl_currentAddr = nullptr;

inline void _setBuffer(void** startAddr, void** currentAddr, addr_size* capacity,
                      void* data, addr_size cap) {
    *startAddr = data;
    *currentAddr = data;
    *capacity = cap;
}

inline void* _alloc(void** currentAddr, const void* startAddr, addr_size cap, const OOMHandlerFn oomHandler,
                    addr_size count, addr_size size) {
    addr_size effectiveSize = count * size;
    effectiveSize = core::align(effectiveSize);
    if (core::ptrDiff(*currentAddr, startAddr) + effectiveSize > cap) {
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    void* ret = *currentAddr;
    *currentAddr = core::ptrAdvance(*currentAddr, effectiveSize);
    return ret;
}

inline void* _calloc(void** currentAddr, const void* startAddr, addr_size cap, const OOMHandlerFn oomHandler,
                    addr_size count, addr_size size) {
    void* ret = _alloc(currentAddr, startAddr, cap, oomHandler, count, size);
    if (ret) {
        addr_size effectiveSize = count * size;
        effectiveSize = core::align(effectiveSize);
        core::memset(ret, 0, effectiveSize);
    }
    return ret;
}

} // namespace

// Bump Allocator

BumpAllocator::BumpAllocator()
    : oomHandler(getDefaultOOMHandler())
    , m_startAddr(nullptr)
    , m_currentAddr(nullptr)
    , m_cap(0) {}

BumpAllocator::BumpAllocator(void* data, addr_size cap)
    : oomHandler(getDefaultOOMHandler())
    , m_startAddr(data)
    , m_currentAddr(data)
    , m_cap(cap) {}

BumpAllocator::BumpAllocator(BumpAllocator&& other) {
    oomHandler = other.oomHandler;
    m_startAddr = other.m_startAddr;
    m_currentAddr = other.m_currentAddr;
    m_cap = other.m_cap;

    other.oomHandler = nullptr;
    other.m_startAddr = nullptr;
    other.m_currentAddr = nullptr;
    other.m_cap = 0;
}

void BumpAllocator::setBuffer(void* data, addr_size cap) {
    _setBuffer(&m_startAddr, &m_currentAddr, &m_cap, data, cap);
}

void* BumpAllocator::alloc(addr_size count, addr_size size) {
    return _alloc(&m_currentAddr, m_startAddr, m_cap, oomHandler, count, size);
}

void* BumpAllocator::calloc(addr_size count, addr_size size) {
    return _calloc(&m_currentAddr, m_startAddr, m_cap, oomHandler, count, size);
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

// Thread Local Bump Allocator

ThreadLocalBumpAllocator::ThreadLocalBumpAllocator() : oomHandler(getDefaultOOMHandler()) {}

ThreadLocalBumpAllocator ThreadLocalBumpAllocator::create(void* data, addr_size cap) {
    Panic(tl_capacity == core::MAX_U64, "ThreadLocalBumpAllocator::create() called twice in the same thread");
    _setBuffer(&tl_startAddr, &tl_currentAddr, &tl_capacity, data, cap);
    return ThreadLocalBumpAllocator{};
}

void ThreadLocalBumpAllocator::setBuffer(void* data, addr_size cap) {
    clear();
    _setBuffer(&tl_startAddr, &tl_currentAddr, &tl_capacity, data, cap);
}

void* ThreadLocalBumpAllocator::alloc(addr_size count, addr_size size) {
    return _alloc(&tl_currentAddr, tl_startAddr, tl_capacity, oomHandler, count, size);
}

void* ThreadLocalBumpAllocator::calloc(addr_size count, addr_size size) {
    return _calloc(&tl_currentAddr, tl_startAddr, tl_capacity, oomHandler, count, size);
}

void ThreadLocalBumpAllocator::free(void*, addr_size, addr_size) {}

void ThreadLocalBumpAllocator::clear() {
    tl_currentAddr = tl_startAddr;
}

addr_size ThreadLocalBumpAllocator::totalMemoryAllocated() {
    return inUseMemory();
}

addr_size ThreadLocalBumpAllocator::inUseMemory() {
    addr_size ret = core::ptrDiff(tl_currentAddr, tl_startAddr);
    return ret;
}

} // namespace core
