#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_exec_ctx.h>
#include <core_alloc.h>
#include <core_utils.h>

#include <new>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT AllocatorContext;

using AllocateFn = void *(*)(addr_size count, addr_size size);
using ZeroAllocateFn = void *(*)(addr_size count, addr_size size);
using FreeFn = void (*)(void *ptr, addr_size count, addr_size size);
using ClearFn = void (*)();
using TotalMemoryAllocatedFn = addr_size (*)();
using InUseMemoryFn = addr_size (*)();
using DestroyAllocatorContextFn = void (*)(AllocatorContext&);

struct AllocatorContext {
    AllocateFn alloc;
    ZeroAllocateFn calloc;
    FreeFn free;
    ClearFn clear;
    TotalMemoryAllocatedFn totalMemoryAllocated;
    InUseMemoryFn inUseMemory;

    DestroyAllocatorContextFn destroy;
    void* userData;
};

CORE_API_EXPORT void initProgramCtx();
CORE_API_EXPORT void destroyProgramCtx();

CORE_API_EXPORT AllocatorContext createDefaultAllocatorCtx();
CORE_API_EXPORT AllocatorContext& getActiveAllocatorCtx();
CORE_API_EXPORT void setActiveAllocatorCtx(u32 idx);
CORE_API_EXPORT void registerAllocatorCtx(AllocatorContext&& ctx);

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
