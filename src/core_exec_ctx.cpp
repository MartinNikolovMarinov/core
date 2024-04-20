#include <core_exec_ctx.h>

#include <core_alloc.h>
#include <core_utils.h>
#include <plt/core_threading.h>

namespace core {

namespace {

AllocatorContext g_defaultAllocatorContext;
StdAllocator g_defaultStdAllocator;

thread_local AllocatorContext* tl_activeAllocatorContext = nullptr;

void zeroOutAllocatorContext(AllocatorContext& actx) {
    actx.alloc = nullptr;
    actx.calloc = nullptr;
    actx.free = nullptr;
    actx.clear = nullptr;
    actx.totalMemoryAllocated = nullptr;
    actx.inUseMemory = nullptr;
    actx.allocatorData = nullptr;
}

} // namespace

AllocatorContext::AllocatorContext(void* _allocatorData) {
    zeroOutAllocatorContext(*this);
    allocatorData = _allocatorData;
}
AllocatorContext::AllocatorContext(AllocatorContext&& other) {
    alloc = other.alloc;
    calloc = other.calloc;
    free = other.free;
    clear = other.clear;
    totalMemoryAllocated = other.totalMemoryAllocated;
    inUseMemory = other.inUseMemory;
    allocatorData = other.allocatorData;

    zeroOutAllocatorContext(other);
}

AllocatorContext& AllocatorContext::operator=(AllocatorContext&& other) {
    alloc = other.alloc;
    calloc = other.calloc;
    free = other.free;
    clear = other.clear;
    totalMemoryAllocated = other.totalMemoryAllocated;
    inUseMemory = other.inUseMemory;
    allocatorData = other.allocatorData;

    zeroOutAllocatorContext(other);

    return *this;
}

AllocatorContext::~AllocatorContext() {
    if (clear) clear(allocatorData);
    zeroOutAllocatorContext(*this);
}

void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                    const AllocatorContext* defaultAllocatorCtx) {
    core::setGlobalAssertHandler(assertHandler);

    if (defaultAllocatorCtx) {
        g_defaultAllocatorContext = *defaultAllocatorCtx;
    }
    else {
        g_defaultAllocatorContext = createAllocatorCtx(&g_defaultStdAllocator);
    }
}

void destroyProgramCtx() {
    core::setGlobalAssertHandler(nullptr);
}

void setActiveAllocatorForThread(AllocatorContext* activeContext) {
    tl_activeAllocatorContext = activeContext;
}

namespace {

inline AllocatorContext* getActiveAllocatorContext() {
    if (tl_activeAllocatorContext) {
        return tl_activeAllocatorContext;
    }
    return &g_defaultAllocatorContext;
}

} // namespace

void* alloc(addr_size count, addr_size size) {
    auto* actx = getActiveAllocatorContext();
    void* ret = actx->alloc(actx->allocatorData, count, size);
    return ret;
}

void* zeroAlloc(addr_size count, addr_size size) {
    auto* actx = getActiveAllocatorContext();
    void* ret = actx->calloc(actx->allocatorData, count, size);
    return ret;
}

void free(void* ptr, addr_size count, addr_size size) {
    auto* actx = getActiveAllocatorContext();
    actx->free(actx->allocatorData, ptr, count, size);
}

addr_size totalMemoryAllocated() {
    auto* actx = getActiveAllocatorContext();
    addr_size ret = actx->totalMemoryAllocated(actx->allocatorData);
    return ret;
}

addr_size inUseMemory() {
    auto* actx = getActiveAllocatorContext();
    addr_size ret = actx->inUseMemory(actx->allocatorData);
    return ret;
}

} // namespace core
