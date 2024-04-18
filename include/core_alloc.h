#pragma once

#include <core_API.h>
#include <core_traits.h>
#include <core_types.h>
#include <core_utils.h>

#include <plt/core_atomics.h>

namespace core {

using namespace coretypes;

using OOMHandlerFn = void (*)();

CORE_API_EXPORT void defaultOOMHandler();

template <typename T>
concept AllocatorConcept = requires(T a) {
    { a.alloc(std::declval<addr_size>(), std::declval<addr_size>()) } -> core::same_as<void*>;
    { a.calloc(std::declval<addr_size>(), std::declval<addr_size>()) } -> core::same_as<void*>;
    { a.clear() };
    { a.free(std::declval<void*>(), std::declval<addr_size>(), std::declval<addr_size>()) };
    { a.totalMemoryAllocated() } -> core::same_as<addr_size>;
    { a.inUseMemory() } -> core::same_as<addr_size>;
};

struct CORE_API_EXPORT StdAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdAllocator);

    StdAllocator();
    StdAllocator(StdAllocator&&) = default;

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size);
    void clear(); // does nothing
    addr_size totalMemoryAllocated(); // always returns 0
    addr_size inUseMemory(); // always returns 0
};
static_assert(AllocatorConcept<StdAllocator>);

struct CORE_API_EXPORT StdStatsAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdStatsAllocator);

    StdStatsAllocator();
    StdStatsAllocator(StdStatsAllocator&& other);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size);
    void clear(); // does nothing
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

private:
    core::AtomicU64 m_totalMemoryAllocated;
    core::AtomicU64 m_inUseMemory;
};
static_assert(AllocatorConcept<StdStatsAllocator>);

/**
 * @note This allocator is not thread-safe.
*/
struct CORE_API_EXPORT BumpAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(BumpAllocator);

    BumpAllocator(void* data, addr_size cap);
    BumpAllocator(BumpAllocator&& other);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated(); // same as inUseMemory
    addr_size inUseMemory();

private:
    void* m_startAddr;
    void* m_currentAddr;
    addr_size m_cap;
};
static_assert(AllocatorConcept<BumpAllocator>);

/**
 * @note This allocator is not thread-safe.
*/
struct CORE_API_EXPORT StdArenaAllocator {
    struct Block {
        void* begin;
        void* curr;
    };

    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdArenaAllocator);

    StdArenaAllocator(addr_size blockSize);
    StdArenaAllocator(StdArenaAllocator&& other);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

    void reset();

private:
    Block* m_blocks;
    addr_size m_blockCount;
    const addr_size m_blockSize;
};
static_assert(AllocatorConcept<StdArenaAllocator>);

} // namespace core
