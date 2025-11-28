#include <core_exec_ctx.h>

#include <core_alloc.h>
#include <core_assert.h>
#include <core_assert_fmt.h>
#include <core_logger.h>

#include <utility> // std::move aperantly is here.

namespace core {

namespace {

AllocatorContext g_defaultAllocatorContext;
StdAllocator g_defaultStdAllocator;

constexpr u32 MAX_REGISTERABLE_ALLOCATORS = 20;
addr_size g_registeredAllocatorsCount = 0;
AllocatorContext g_registeredAllocators[MAX_REGISTERABLE_ALLOCATORS] = {};

void zeroOutAllocatorContext(AllocatorContext& actx) {
    actx.nameFn = nullptr;
    actx.allocFn = nullptr;
    actx.callocFn = nullptr;
    actx.freeFn = nullptr;
    actx.clearFn = nullptr;
    actx.totalMemoryAllocatedFn = nullptr;
    actx.inUseMemoryFn = nullptr;
    actx.allocatorData = nullptr;
}

} // namespace

AllocatorContext::AllocatorContext(void* _allocatorData) {
    zeroOutAllocatorContext(*this);
    allocatorData = _allocatorData;
}

AllocatorContext::AllocatorContext(AllocatorContext&& other) {
    nameFn = other.nameFn;
    allocFn = other.allocFn;
    callocFn = other.callocFn;
    freeFn = other.freeFn;
    clearFn = other.clearFn;
    totalMemoryAllocatedFn = other.totalMemoryAllocatedFn;
    inUseMemoryFn = other.inUseMemoryFn;
    allocatorData = other.allocatorData;

    zeroOutAllocatorContext(other);
}

AllocatorContext& AllocatorContext::operator=(AllocatorContext&& other) {
    nameFn = other.nameFn;
    allocFn = other.allocFn;
    callocFn = other.callocFn;
    freeFn = other.freeFn;
    clearFn = other.clearFn;
    totalMemoryAllocatedFn = other.totalMemoryAllocatedFn;
    inUseMemoryFn = other.inUseMemoryFn;
    allocatorData = other.allocatorData;

    zeroOutAllocatorContext(other);

    return *this;
}

const char* AllocatorContext::name() {
    return nameFn(allocatorData);
}

void* AllocatorContext::alloc(addr_size count, addr_size size) {
    return allocFn(allocatorData, count, size);
}

void* AllocatorContext::zeroAlloc(addr_size count, addr_size size) {
    return callocFn(allocatorData, count, size);
}

void AllocatorContext::free(void* ptr, addr_size count, addr_size size) {
    freeFn(allocatorData, ptr, count, size);
}

void AllocatorContext::clear() {
    clearFn(allocatorData);
}

addr_size AllocatorContext::totalMemoryAllocated() {
    return totalMemoryAllocatedFn(allocatorData);
}

addr_size AllocatorContext::inUseMemory() {
    return inUseMemoryFn(allocatorData);
}

void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                    LoggerCreateInfo* loggerCreateInfo) {
    core::setGlobalAssertHandler(assertHandler);
    g_defaultAllocatorContext = createAllocatorCtx(&g_defaultStdAllocator);

    if (loggerCreateInfo == nullptr) {
        Panic(core::loggerInit(), "Failed to init logger");
    }
    else {
        Panic(core::loggerInit(*loggerCreateInfo), "Failed to init logger");
    }
}

void initProgramCtx(GlobalAssertHandlerFn assertHandler,
                    LoggerCreateInfo* loggerCreateInfo,
                    AllocatorContext&& actx) {
    core::setGlobalAssertHandler(assertHandler);
    g_defaultAllocatorContext = std::move(actx);

    if (loggerCreateInfo == nullptr) {
        Panic(core::loggerInit(), "Failed to init logger");
    }
    else {
        Panic(core::loggerInit(*loggerCreateInfo), "Failed to init logger");
    }
}

void destroyProgramCtx(bool panicOnLeaks) {
    core::loggerDestroy();

    if (panicOnLeaks) {
        for (addr_size i = 0; i < g_registeredAllocatorsCount; i++) {
            PanicFmt(g_registeredAllocators[i].inUseMemory() == 0,
                "Detected memory leak for allocator: {}, bytes_in_use: {}",
                g_registeredAllocators[i].name(),
                g_registeredAllocators[i].inUseMemory());
        }
        PanicFmt(g_defaultAllocatorContext.inUseMemory() == 0,
                "Detected memory leak for allocator: {}, bytes_in_use: {}",
                g_defaultAllocatorContext.name(),
                g_defaultAllocatorContext.inUseMemory());
    }

    core::setGlobalAssertHandler(nullptr);
    g_defaultAllocatorContext = {};
}

void registerAllocator(AllocatorContext&& ctx, AllocatorId id) {
    Panic(id - 1 < MAX_REGISTERABLE_ALLOCATORS);
    g_registeredAllocators[id - 1] = std::move(ctx);
    g_registeredAllocatorsCount++;
}

AllocatorContext& getAllocator(AllocatorId id) {
    // IMPORTANT:
    // If you get a reference to an allocator by some id, MAKE SURE TO REGISTER IT before using it!
    // This function may be called at static initialization time, before any custom allocators have been registered,
    // which makes checking the `g_registeredAllocatorsCount` variable impossible.

    Panic(id <= MAX_REGISTERABLE_ALLOCATORS);

    if (id == 0) {
        return g_defaultAllocatorContext;
    }
    // the registered allocators have ids offset by one to account for the default allocator.
    return g_registeredAllocators[id - 1];
}

} // namespace core
