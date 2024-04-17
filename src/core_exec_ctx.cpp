#include <core_alloc.h>
#include <core_exec_ctx.h>
#include <core_utils.h>

namespace core {

namespace {

constexpr u32 MAX_ALLOCATOR_CONTEXTS = 16;
thread_local AllocatorContext g_allocatorContexts[MAX_ALLOCATOR_CONTEXTS] = {};
thread_local u32 g_allocatorContextsCount = 0;
thread_local u32 g_activeAllocatorCtx = 0;

StdAllocator g_stdAllocator;

} // namespace

AllocatorContext createDefaultAllocatorCtx() {
    AllocatorContext ctx;

    ctx.alloc = [](addr_size count, addr_size size) -> void* { return g_stdAllocator.alloc(count, size); };
    ctx.calloc = [](addr_size count, addr_size size) -> void* { return g_stdAllocator.calloc(count, size); };
    ctx.free = [](void* ptr, addr_size count, addr_size size) { g_stdAllocator.free(ptr, count, size); };
    ctx.clear = []() { g_stdAllocator.clear(); };
    ctx.totalMemoryAllocated = []() -> addr_size { return g_stdAllocator.totalMemoryAllocated(); };
    ctx.inUseMemory = []() -> addr_size { return g_stdAllocator.inUseMemory(); };
    ctx.destroy = [](AllocatorContext& _ctx) {
        _ctx.alloc = nullptr;
        _ctx.calloc = nullptr;
        _ctx.free = nullptr;
        _ctx.clear = nullptr;
        _ctx.totalMemoryAllocated = nullptr;
        _ctx.inUseMemory = nullptr;
    };

    return ctx;
}

void initProgramCtx() {
    g_allocatorContexts[0] = createDefaultAllocatorCtx();
    g_allocatorContextsCount = 1;
    g_activeAllocatorCtx = 0;
}

void destroyProgramCtx() {
    for (u32 i = 1; i < g_allocatorContextsCount; ++i) {
        auto& ctx = g_allocatorContexts[i];
        if (ctx.destroy) {
            ctx.destroy(ctx);
        }
    }
    g_allocatorContextsCount = 1;
    g_activeAllocatorCtx = 0;
}

AllocatorContext& getActiveAllocatorCtx() {
    auto& activeCtx = g_allocatorContexts[g_activeAllocatorCtx];
    return activeCtx;
}

void setActiveAllocatorCtx(u32 idx) {
    Panic(idx < g_allocatorContextsCount, "Invalid allocator context index");
    g_activeAllocatorCtx = idx;
}

void registerAllocatorCtx(AllocatorContext&& ctx) {
    Panic(g_allocatorContextsCount < MAX_ALLOCATOR_CONTEXTS, "Too many allocator contexts");
    g_allocatorContexts[g_allocatorContextsCount] = ctx;
    g_allocatorContextsCount++;
}

void* alloc(addr_size count, addr_size size) {
    auto& ctx = getActiveAllocatorCtx();
    void* ret = ctx.alloc(count, size);
    return ret;
}

void* zeroAlloc(addr_size count, addr_size size) {
    auto& ctx = getActiveAllocatorCtx();
    void* ret = ctx.calloc(count, size);
    return ret;
}

void free(void* ptr, addr_size count, addr_size size) {
    auto& ctx = getActiveAllocatorCtx();
    ctx.free(ptr, count, size);
}

addr_size totalMemoryAllocated() {
    auto& ctx = getActiveAllocatorCtx();
    addr_size ret = ctx.totalMemoryAllocated();
    return ret;
}

addr_size inUseMemory() {
    auto& ctx = getActiveAllocatorCtx();
    addr_size ret = ctx.inUseMemory();
    return ret;
}

} // namespace core
