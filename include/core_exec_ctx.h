#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_alloc.h>

#include <new>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT AllocatorContext;

using AllocateFn             = void *(*)(void* allocatorData, addr_size count, addr_size size);
using ZeroAllocateFn         = void *(*)(void* allocatorData, addr_size count, addr_size size);
using FreeFn                 = void (*)(void* allocatorData, void *ptr, addr_size count, addr_size size);
using ClearFn                = void (*)(void* allocatorData);
using TotalMemoryAllocatedFn = addr_size (*)(void* allocatorData);
using InUseMemoryFn          = addr_size (*)(void* allocatorData);

struct CORE_API_EXPORT AllocatorContext {
    AllocateFn alloc;
    ZeroAllocateFn calloc;
    FreeFn free;
    ClearFn clear;
    TotalMemoryAllocatedFn totalMemoryAllocated;
    InUseMemoryFn inUseMemory;
    void* allocatorData;

    AllocatorContext(void* allocatorData = nullptr);
    AllocatorContext(const AllocatorContext& other) = default;
    AllocatorContext(AllocatorContext&& other);
    ~AllocatorContext();

    AllocatorContext& operator=(const AllocatorContext& other) = default;
    AllocatorContext& operator=(AllocatorContext&& other);
};

template <AllocatorConcept TAllocator>
AllocatorContext createAllocatorCtx(TAllocator* allocator) {
    core::AllocatorContext ctx;

    ctx.allocatorData = allocator;
    ctx.alloc = [](void* allocatorData, core::addr_size count, core::addr_size size) -> void* {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.alloc(count, size);
    };
    ctx.calloc = [](void* allocatorData, core::addr_size count, core::addr_size size) -> void* {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.calloc(count, size);
    };
    ctx.free = [](void* allocatorData, void* ptr, core::addr_size count, core::addr_size size) {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        a.free(ptr, count, size);
    };
    ctx.clear = [](void* allocatorData) {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        a.clear();
    };
    ctx.totalMemoryAllocated = [](void* allocatorData) -> core::addr_size {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.totalMemoryAllocated();
    };
    ctx.inUseMemory = [](void* allocatorData) -> core::addr_size {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.inUseMemory();
    };

    return ctx;
}

/**
 * The init function should be called by the main thread, only once and before starting any other threads.
*/
CORE_API_EXPORT void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                                    const AllocatorContext* defaultAllocatorCtx);
/**
 * The destroy function should be called by the main thread when the program is about to terminate.
*/
CORE_API_EXPORT void destroyProgramCtx();

CORE_API_EXPORT void setActiveAllocatorForThread(AllocatorContext* activeContext);
CORE_API_EXPORT void clearActiveAllocatorForThread();

CORE_API_EXPORT void* alloc(addr_size count, addr_size size);
CORE_API_EXPORT void* zeroAlloc(addr_size count, addr_size size);
CORE_API_EXPORT void free(void* ptr, addr_size count, addr_size size);
CORE_API_EXPORT addr_size totalMemoryAllocated();
CORE_API_EXPORT addr_size inUseMemory();

template <typename T, typename... Args>
T* construct(Args&&... args) {
    T* ptr = reinterpret_cast<T*>(alloc(1, sizeof(T)));
    return new(ptr) T(std::forward<Args>(args)...);
}

} // namespace core
