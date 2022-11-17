#include <core.h>

#include <iostream>

using namespace coretypes;

#define RunTest(test)                                                       \
    std::cout << "\t[TEST RUNNING] " << #test << '\n';                      \
    test();                                                                 \
    std::cout << "\t[TEST \x1b[32mPASSED\x1b[0m] " << #test << std::endl;

#define RunTestSuite(suite)                                                 \
    std::cout << "[SUITE RUNNING] " << #suite << std::endl;                 \
    suite();                                                                \
    std::cout << "[SUITE \x1b[32mPASSED\x1b[0m] " << #suite << std::endl;

// #include "test/run_tests.cpp"
// #include "test/std/run_std_tests.cpp"

// i32 main() {
//     core::SetGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
//         std::cout << "[ASSERTION] [EXPR]: " << failedExpr
//                 << " [FILE]: " << file
//                 << " [LINE]: " << line
//                 << " [MSG]: " << errMsg
//                 << std::endl; // flush stream!
//         return true;
//     });

//     if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
//     if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
//     if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
//     if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

//     if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
//     if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
//     if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
//     if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

//     RunAllTests();
//     RunAllSTDTests();

//     std::cout << '\n';
//     std::cout << "\x1b[32m\x1b[1mTests OK\x1b[0m\n";
//     return 0;
// }

#include <alloc/interface.h>
#include <std/alloc/std.h>

// FIXME: How do I handle allocator switching?
//        Maybe just register all allocators globally and use tags to switch between them in templates?
//        BUT I might need to be able to switch between allocators at runtime, which won't be possible.
//        Or maybe I don't actually care about that ?

// template <typename TAlloc>
// TAlloc GetDefaultAllocator() {
//     static core::alloc::StdAllocator stdalloc;
//     return stdalloc;
// }

using DefaultAllocator = core::alloc::StdAllocator;
constexpr u32 ALLOC_REGISTRY_SIZE = 2;
void *gAllocRegistry[ALLOC_REGISTRY_SIZE];

template <i32 TAllocId>
auto* GetAllocator() {
    if constexpr (TAllocId == 0) {
        return reinterpret_cast<DefaultAllocator*>(gAllocRegistry[0]);
    }
    else if constexpr (TAllocId == 1) {
        return reinterpret_cast<core::alloc::StaticBumpAllocator<1048>*>(gAllocRegistry[1]);
    }
    else {
        static_assert(TAllocId < ALLOC_REGISTRY_SIZE, "Invalid allocator id");
        return DefaultAllocator();
    }
}

void SetAllocator(i32 allocId, void *alloc) {
    gAllocRegistry[allocId] = alloc;
}

template<typename T, i32 TAllocId = 0>
struct Arr {

    Arr() : data(nullptr), len(0), cap(0) {};

    void Testing() {
        if (data == nullptr) {
            data = reinterpret_cast<T*>(core::alloc::Alloc(*GetAllocator<TAllocId>(), sizeof(T) * 10));
        }
    }

private:
    T* data;
    ptr_size len;
    ptr_size cap;
};

#include <vector>

int main() {
    SetAllocator(0, new DefaultAllocator());
    SetAllocator(1, new core::alloc::StaticBumpAllocator<1048>());

    Arr<i32> darr = Arr<i32>();
    darr.Testing();

    Arr<i32, 1> sarr = Arr<i32, 1>();
    sarr.Testing();

    constexpr i32 test = sizeof(Arr<i32>);
    std::cout << test << std::endl;
    constexpr i32 test2 = sizeof(std::vector<i32>);
    std::cout << test2 << std::endl;
    return 0;
}
