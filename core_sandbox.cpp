#include "tests/t-index.h"

#include <iostream>

void assertHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 2;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << (errMsg ? errMsg : "") // IMPORTANT: MSVC's std implementation will crash if errMsg is nullptr !
                << ANSI_RESET()
                << std::endl;
    std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;

    if (!ok) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "Failed to take full stacktrace. Consider resizing the stacktrace buffer size!"
                  << ANSI_RESET()
                  << std::endl;
    }

    throw std::runtime_error("Assertion failed!");
};

using AllocateFn             = void *(*)(addr_size count, addr_size size);
using ZeroAllocateFn         = void *(*)(addr_size count, addr_size size);
using FreeFn                 = void (*)(void *ptr, addr_size count, addr_size size);
using ClearFn                = void (*)();
using TotalMemoryAllocatedFn = addr_size (*)();
using InUseMemoryFn          = addr_size (*)();

struct AllocatorContext {
    AllocateFn alloc;
    ZeroAllocateFn calloc;
    FreeFn free;
    ClearFn clear;
    TotalMemoryAllocatedFn totalMemoryAllocated;
    InUseMemoryFn inUseMemory;
};

AllocatorContext g_registeredAllocators[5];

template <i32 TAllocId>
inline AllocatorContext& selectAllocator() {
    return g_registeredAllocators[TAllocId];
}

template <i32 TAllocId = 0>
struct Tester {
    inline static AllocatorContext& allocator = selectAllocator<TAllocId>();

    void* oneByte() {
        return allocator.alloc(1, 1);
    }

    void freeByte(void* addr) {
        allocator.free(addr, 1, 1);
    }

    i32 someMember;
};

static_assert(sizeof(Tester<0>) == sizeof(i32), "The allocator must not take any space!");

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);

    g_registeredAllocators[0].alloc = [](addr_size count, addr_size size) {
        void* ret = std::malloc(count * size);
        std::cout << "Called Malloc 0" << std::endl;
        return ret;
    };
    g_registeredAllocators[0].free = [](void* ptr, addr_size, addr_size) {
        std::free(ptr);
        std::cout << "Called Free 0" << std::endl;
    };

    g_registeredAllocators[1].alloc = [](addr_size count, addr_size size) {
        void* ret = std::malloc(count * size);
        std::cout << "Called Malloc 1" << std::endl;
        return ret;
    };
    g_registeredAllocators[1].free = [](void* ptr, addr_size, addr_size) {
        std::free(ptr);
        std::cout << "Called Free 1" << std::endl;
    };

    {
        Tester tester;
        char* byte = reinterpret_cast<char*>(tester.oneByte());
        tester.freeByte(byte);
    }

    {
        Tester<1> tester;
        char* byte = reinterpret_cast<char*>(tester.oneByte());
        tester.freeByte(byte);
    }

    return 0;
}
