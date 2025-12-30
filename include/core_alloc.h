#pragma once

#include <core_API.h>
#include <core_macros.h>
#include <core_traits.h>
#include <core_types.h>

#include <plt/core_atomics.h>

// TODO2: I need to think about alignment in these allocators. Aligning every allocation to 8 bytes might not be the best idea for all cases.

namespace core {

using namespace coretypes;

using OOMHandlerFn = void (*)();

CORE_API_EXPORT OOMHandlerFn getDefaultOOMHandler();

template <typename T>
concept AllocatorConcept = requires(T a) {
    { a.alloc(std::declval<addr_size>(), std::declval<addr_size>()) } -> core::same_as<void*>;
    { a.calloc(std::declval<addr_size>(), std::declval<addr_size>()) } -> core::same_as<void*>;
    { a.clear() };
    { a.free(std::declval<void*>(), std::declval<addr_size>(), std::declval<addr_size>()) };
    { a.totalMemoryAllocated() } -> core::same_as<addr_size>;
    { a.inUseMemory() } -> core::same_as<addr_size>;
    { a.name() } -> core::same_as<const char*>;
    { a.tracksMemory() } -> core::same_as<bool>;
    { a.canDetectLeaks() } -> core::same_as<bool>;
};

struct CORE_API_EXPORT StdAllocator;
struct                 StdStatsAllocator;
struct CORE_API_EXPORT BumpAllocator;
struct CORE_API_EXPORT ThreadLocalStdArenaAllocator;
struct CORE_API_EXPORT ThreadLocalBumpAllocator;
struct CORE_API_EXPORT StdArenaAllocator;

struct CORE_API_EXPORT StdAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdAllocator);

    StdAllocator();
    StdAllocator(StdAllocator&&) = default;

    constexpr const char* name() { return "STD_ALLOCATOR"; }

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size);
    void clear(); // does nothing
    addr_size totalMemoryAllocated(); // always returns 0
    addr_size inUseMemory(); // always returns 0

    constexpr bool tracksMemory() { return false; }
    constexpr bool canDetectLeaks() { return false; }
};
static_assert(AllocatorConcept<StdAllocator>);

struct StdStatsAllocator {
    // NOTE: This type exports functions instead of the whole struct because the use of atomic types in the triggers C4251
    //       warning on MSVC. It's a valid warning and it's caused by the fact that the atomic types are not exported.

    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdStatsAllocator);

    CORE_API_EXPORT StdStatsAllocator();
    CORE_API_EXPORT StdStatsAllocator(StdStatsAllocator&& other);

    constexpr const char* name() { return "STD_STATS_ALLOCATOR"; }

    CORE_API_EXPORT void* alloc(addr_size count, addr_size size);
    CORE_API_EXPORT void* calloc(addr_size count, addr_size size);
    CORE_API_EXPORT void free(void* ptr, addr_size count, addr_size size);
    CORE_API_EXPORT void clear();
    CORE_API_EXPORT addr_size totalMemoryAllocated();
    CORE_API_EXPORT addr_size inUseMemory();

    constexpr bool tracksMemory() { return true; }
    constexpr bool canDetectLeaks() { return true; }

private:
    core::AtomicU64 m_totalMemoryAllocated;
    core::AtomicU64 m_inUseMemory;
};
static_assert(AllocatorConcept<StdStatsAllocator>);

/**
 * @note This allocator is not thread-safe.
*/
struct CORE_API_EXPORT BumpAllocator {
    OOMHandlerFn oomHandler;

    NO_COPY(BumpAllocator);

    BumpAllocator();
    BumpAllocator(void* data, addr_size cap);
    BumpAllocator(BumpAllocator&& other);

    constexpr const char* name() { return "BUMP_ALLOCATOR"; }

    void setBuffer(void* data, addr_size cap);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated(); // same as inUseMemory
    addr_size inUseMemory();

    constexpr bool tracksMemory() { return true; }
    constexpr bool canDetectLeaks() { return false; }

private:
    void* m_startAddr;
    void* m_currentAddr;
    addr_size m_cap;
};
static_assert(AllocatorConcept<BumpAllocator>);

/**
 * @note This allocator is thread-safe only if the data pointer is thread-local.
 *       Only one of these can exist per thread!
*/
struct CORE_API_EXPORT ThreadLocalBumpAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(ThreadLocalBumpAllocator);
    NO_MOVE(ThreadLocalBumpAllocator);

    constexpr const char* name() { return "THREAD_LOCAL_BUMP_ALLOCATOR"; }

    static ThreadLocalBumpAllocator create(void* data, addr_size cap); // calling this twice in the same thread will panic

    /**
     * @note Changing the buffer will force a clear operation.
    */
    void setBuffer(void* data, addr_size cap);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated(); // same as inUseMemory
    addr_size inUseMemory();

    constexpr bool tracksMemory() { return true; }
    constexpr bool canDetectLeaks() { return false; }

private:
    ThreadLocalBumpAllocator();
};
static_assert(AllocatorConcept<ThreadLocalBumpAllocator>);

struct ArenaBlock;

/**
 * @note This allocator is not thread-safe.
*/
struct CORE_API_EXPORT StdArenaAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(StdArenaAllocator);

    explicit StdArenaAllocator(addr_size blockSize);
    StdArenaAllocator(StdArenaAllocator&& other);

    constexpr const char* name() { return "ARENA_ALLOCATOR"; }

    /**
     * @note Setting the block size will force a clear operation.
    */
    void setBlockSize(addr_size blockSize);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

    constexpr bool tracksMemory() { return true; }
    constexpr bool canDetectLeaks() { return false; }

    void reset();

private:
    ArenaBlock* m_blocks;
    addr_size m_blockCount;
    addr_size m_blockSize;
};
static_assert(AllocatorConcept<StdArenaAllocator>);

/**
 * @note This allocator is thread-safe only if the data pointer is thread-local.
 *       Only one of these can exist per thread!
*/
struct CORE_API_EXPORT ThreadLocalStdArenaAllocator {
    OOMHandlerFn oomHandler = nullptr;

    NO_COPY(ThreadLocalStdArenaAllocator);
    NO_MOVE(ThreadLocalStdArenaAllocator);

    static ThreadLocalStdArenaAllocator create(addr_size blockSize);

    constexpr const char* name() { return "THREAD_LOCAL_ARENA_ALLOCATOR"; }

    /**
     * @note Changing the block size will clear the allocator.
    */
    void setBlockSize(addr_size blockSize);

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

    constexpr bool tracksMemory() { return true; }
    constexpr bool canDetectLeaks() { return false; }

    void reset();

private:
    ThreadLocalStdArenaAllocator();
};
static_assert(AllocatorConcept<ThreadLocalStdArenaAllocator>);

} // namespace core
