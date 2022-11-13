#include <core.h>

#include <iostream>

using namespace coretypes;

// #define RunTest(test)                                                       \
//     std::cout << "\t[TEST RUNNING] " << #test << '\n';                      \
//     test();                                                                 \
//     std::cout << "\t[TEST \x1b[32mPASSED\x1b[0m] " << #test << std::endl;

// #define RunTestSuite(suite)                                                 \
//     std::cout << "[SUITE RUNNING] " << #suite << std::endl;                         \
//     suite();                                                                \
//     std::cout << "[SUITE \x1b[32mPASSED\x1b[0m] " << #suite << std::endl;

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

template<typename T>
struct Arr {

    Arr() : data(nullptr), len(0), cap(0) {};

private:
    T* data;
    ptr_size len;
    ptr_size cap;
};

#include <vector>

int main(int argc, char const *argv[]) {
    return 0;
}
