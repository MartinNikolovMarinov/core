#include <core.h>
#include <std/core.h>

#include <iostream>

using namespace coretypes;

#define RunTest(test, ...)                                                                               \
    std::cout << "\t[TEST RUNNING] " << ANSI_BOLD(#test) << '\n';                                        \
    test(__VA_ARGS__);                                                                                   \
    std::cout << "\t[TEST " << ANSI_BOLD(ANSI_GREEN("PASSED")) << "] " << ANSI_BOLD(#test) << std::endl;

#define RunTestSuite(suite, ...)                                                                         \
    std::cout << "[SUITE RUNNING] " << ANSI_BOLD(#suite) << std::endl;                                   \
    suite(__VA_ARGS__);                                                                                  \
    std::cout << "[SUITE " << ANSI_BOLD(ANSI_GREEN("PASSED")) << "] " << ANSI_BOLD(#suite) << std::endl;

#include "test/run_tests.cpp"
#include "test/std/run_tests.cpp"

i32 main(i32, const char **) {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "[ASSERTION] [EXPR]: " << failedExpr
                  << " [FILE]: " << file
                  << " [LINE]: " << line
                  << " [MSG]: " << errMsg
                  << ANSI_RESET()
                  << std::endl; // flush stream!
        throw std::runtime_error("Assertion failed!");
        return false;
    });

    // Print compiler
    if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
    if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
    if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
    if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

    // Print OS
    if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
    if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
    if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
    if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

    std::cout << "\n" << "RUNNING COMMONG TESTS" << "\n\n";
    run_all_tests();
    std::cout << "\n" << "RUNNING STD TESTS" << "\n\n";
    run_all_std_tests();

    std::cout << '\n';
    std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;
    return 0;
}
