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

template <i32 TAllocId = 0>
struct Tester {
    inline static core::AllocatorContext& allocator = core::getAllocator(TAllocId);

    void* oneByte() {
        return allocator.alloc(1, 1);
    }

    void freeByte(void* addr) {
        allocator.free(addr, 1, 1);
    }

    i32 someMember;
};

static_assert(sizeof(Tester<0>) == sizeof(i32), "The allocator must not take any space!");
static_assert(sizeof(Tester<1>) == sizeof(i32), "The allocator must not take any space!");
static_assert(sizeof(Tester<2>) == sizeof(i32), "The allocator must not take any space!");

struct A0 {
    void* alloc(addr_size count, addr_size size) {
        void* ret = std::malloc(count * size);
        std::cout << "Called Malloc 0" << std::endl;
        return ret;
    }

    void* calloc(addr_size, addr_size) { return nullptr; }

    void free(void* ptr, addr_size, addr_size) {
        std::free(ptr);
        std::cout << "Called Free 0" << std::endl;
    }

    void clear() {}

    addr_size totalMemoryAllocated() { return 0; }
    addr_size inUseMemory() { return 0; }
};

struct A1 {
    void* alloc(addr_size count, addr_size size) {
        void* ret = std::malloc(count * size);
        std::cout << "Called Malloc 1" << std::endl;
        return ret;
    }

    void* calloc(addr_size, addr_size) { return nullptr; }

    void free(void* ptr, addr_size, addr_size) {
        std::free(ptr);
        std::cout << "Called Free 1" << std::endl;
    }

    void clear() {}

    addr_size totalMemoryAllocated() { return 0; }
    addr_size inUseMemory() { return 0; }
};

struct A2 {
    void* alloc(addr_size count, addr_size size) {
        void* ret = std::malloc(count * size);
        std::cout << "Called Malloc 2" << std::endl;
        return ret;
    }

    void* calloc(addr_size, addr_size) { return nullptr; }

    void free(void* ptr, addr_size, addr_size) {
        std::free(ptr);
        std::cout << "Called Free 2" << std::endl;
    }

    void clear() {}

    addr_size totalMemoryAllocated() { return 0; }
    addr_size inUseMemory() { return 0; }
};

i32 main() {
    A0 a0;
    core::initProgramCtx(assertHandler, core::createAllocatorCtx(&a0));

    A1 a1;
    core::registerAllocator(core::createAllocatorCtx(&a1));

    A2 a2;
    core::registerAllocator(core::createAllocatorCtx(&a2));

    Tester tester;
    Tester<0> tester0;
    Tester<1> tester1;
    Tester<2> tester2;

    std::cout << "DEFAULT" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester.oneByte());
        tester.freeByte(byte);
    }

    std::cout << "ID=0" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester0.oneByte());
        tester0.freeByte(byte);
    }

    std::cout << "ID=1" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester1.oneByte());
        tester1.freeByte(byte);
    }

    std::cout << "ID=2" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester2.oneByte());
        tester2.freeByte(byte);
    }

    std::cout << "ID=2" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester2.oneByte());
        tester2.freeByte(byte);
    }

    std::cout << "ID=1" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester1.oneByte());
        tester1.freeByte(byte);
    }

    std::cout << "ID=0" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester0.oneByte());
        tester0.freeByte(byte);
    }

    std::cout << "DEFAULT" << std::endl;
    {
        char* byte = reinterpret_cast<char*>(tester.oneByte());
        tester.freeByte(byte);
    }

    return 0;
}
