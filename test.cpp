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

// This code is quite complex, because it is zero allocation, but it does somthing very simple.
// It iterates over a table of test cases, and executes the assertion function on each one.
// The assertion function, the test case table, and the error message prefix are all passed in as arguments.
// Every error message is pre-allocated on the stack, and the test case index is manipulated inside the char array.
template <i32 PLen, typename TCase, i32 NCases, typename Afunc>
inline constexpr void executeTestTable(const char (&errMsgPrefix)[PLen], const TCase (&cases)[NCases], Afunc assertionFn) {
    i32 i = 0;
    char errMsg[PLen + 20] = {}; // The 20 is for the test case index number.
    core::memcopy(errMsg, errMsgPrefix, PLen);
    char* appendIdx = &errMsg[PLen - 1];
    for (auto& c : cases) {
        core::int_to_cptr(i, appendIdx, 2);
        assertionFn(c, errMsg);
        i++;
    }
}

#include "test/run_tests.cpp"
#include "test/std/run_tests.cpp"

i32 main(i32, const char **) {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        constexpr u32 stackFramesToSkip = 3;
        std::string trace = core::stacktrace(200, stackFramesToSkip);
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "[ASSERTION] [EXPR]: " << failedExpr
                  << " [FILE]: " << file
                  << " [LINE]: " << line
                  << " [MSG]: " << errMsg
                  << ANSI_RESET()
                  << std::endl; // flush stream!
        std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl; // flush stream!
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
    if (OS_MAC == 1)     { std::cout << "[OS] OS_MAC" << std::endl; }
    if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

    std::cout << "\n" << "RUNNING COMMONG TESTS" << "\n\n";
    run_all_tests();
    std::cout << "\n" << "RUNNING STD TESTS" << "\n\n";
    run_all_std_tests();

    std::cout << '\n';
    std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;
    return 0;
}
