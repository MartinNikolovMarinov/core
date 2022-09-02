#include <core.h>
#include <platform/plt.h>

#include <iostream>

using namespace coretypes;

#define RunTest(test)                                                       \
    std::cout << "\t[TEST RUNNING] " << #test << '\n';                      \
    test();                                                                 \
    std::cout << "\t[TEST \x1b[32mPASSED\x1b[0m] " << #test << std::endl;

#define RunTestSuite(suite)                                                 \
    std::cout << "[SUITE] " << #suite << std::endl;                         \
    suite();                                                                \
    std::cout << "[SUITE \x1b[32mPASSED\x1b[0m] " << #suite << std::endl;

template<typename T>
T CheckTuple(core::Tuple<T, bool> t, const char* msg = "") {
    Assert(t.b, msg);
    return t.a;
}

#include "test/run_tests.cpp"
#include "test/std/run_std_tests.cpp"

i32 main() {
    core::SetGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        std::cout << "[ASSERTION] [EXPR]: " << failedExpr
                << " [FILE]: " << file
                << " [LINE]: " << line
                << " [MSG]: " << errMsg
                << std::endl; // flush stream!
        return true;
    });

    if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
    if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
    if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
    if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

    if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
    if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
    if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
    if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

    RunAllTests();
    RunAllSTDTests();

    std::cout << '\n';
    std::cout << "\x1b[32m\x1b[1mTests OK\x1b[0m\n";
    return 0;
}
