#pragma once

#include <core_alloc.h>
#include <core_API.h>
#include <core_assert.h>
#include <core_types.h>

#include <new>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT AllocatorContext;
struct LoggerCreateInfo;

using AllocatorId = u32;
constexpr AllocatorId DEFAULT_ALLOCATOR_ID = 0;

using AllocateFn             = void *(*)(void* allocatorData, addr_size count, addr_size size);
using ZeroAllocateFn         = void *(*)(void* allocatorData, addr_size count, addr_size size);
using FreeFn                 = void (*)(void* allocatorData, void *ptr, addr_size count, addr_size size);
using ClearFn                = void (*)(void* allocatorData);
using TotalMemoryAllocatedFn = addr_size (*)(void* allocatorData);
using InUseMemoryFn          = addr_size (*)(void* allocatorData);

struct CORE_API_EXPORT AllocatorContext {
    AllocateFn allocFn;
    ZeroAllocateFn callocFn;
    FreeFn freeFn;
    ClearFn clearFn;
    TotalMemoryAllocatedFn totalMemoryAllocatedFn;
    InUseMemoryFn inUseMemoryFn;
    void* allocatorData;

    AllocatorContext(void* allocatorData = nullptr);
    AllocatorContext(const AllocatorContext& other) = default;
    AllocatorContext(AllocatorContext&& other);
    ~AllocatorContext() = default; // NOTE: Don't set the function pointers to null in the destructor.

    AllocatorContext& operator=(const AllocatorContext& other) = default;
    AllocatorContext& operator=(AllocatorContext&& other);

    void* alloc(addr_size count, addr_size size);
    void* zeroAlloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size);
    void clear();
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

    template <typename T, typename... Args>
    T* construct(Args&&... args) {
        T* ptr = reinterpret_cast<T*>(alloc(1, sizeof(T)));
        return new(ptr) T(std::forward<Args>(args)...);
    }
};

template <AllocatorConcept TAllocator>
AllocatorContext createAllocatorCtx(TAllocator* allocator) {
    core::AllocatorContext ctx;

    ctx.allocatorData = allocator;
    ctx.allocFn = [](void* allocatorData, core::addr_size count, core::addr_size size) -> void* {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.alloc(count, size);
    };
    ctx.callocFn = [](void* allocatorData, core::addr_size count, core::addr_size size) -> void* {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.calloc(count, size);
    };
    ctx.freeFn = [](void* allocatorData, void* ptr, core::addr_size count, core::addr_size size) {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        a.free(ptr, count, size);
    };
    ctx.clearFn = [](void* allocatorData) {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        a.clear();
    };
    ctx.totalMemoryAllocatedFn = [](void* allocatorData) -> core::addr_size {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.totalMemoryAllocated();
    };
    ctx.inUseMemoryFn = [](void* allocatorData) -> core::addr_size {
        auto& a = *reinterpret_cast<TAllocator*>(allocatorData);
        return a.inUseMemory();
    };

    return ctx;
}

CORE_API_EXPORT void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                                    LoggerCreateInfo* loggerCreateInfo);
CORE_API_EXPORT void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                                    LoggerCreateInfo* loggerCreateInfo,
                                    AllocatorContext&& actx);

CORE_API_EXPORT void destroyProgramCtx();

CORE_API_EXPORT void registerAllocator(AllocatorContext&& ctx, AllocatorId id);
CORE_API_EXPORT AllocatorContext& getAllocator(AllocatorId id);

} // namespace core
