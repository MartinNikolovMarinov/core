#pragma once

#include <core.h>

#include <iostream>

using namespace coretypes;

// This code is quite complex, because it is zero allocation, but it does somthing very simple.
// It iterates over a table of test cases, and executes the assertion function on each one.
// The assertion function, the test case table, and the error message prefix are all passed in as arguments.
// Every error message is pre-allocated on the stack, and the test case index is manipulated inside the char array.
template <addr_size PLen, typename TCase, addr_size NCases, typename Afunc>
constexpr void executeTestTable(const char (&errMsgPrefix)[PLen], const TCase (&cases)[NCases], Afunc assertionFn) {
    addr_size i = 0;
    char errMsg[PLen + 20] = {}; // The 20 is for the test case index number.
    for (addr_size j = 0; j < PLen; j++) { // NOTE: intentionally not using memcopy, because this needs to work in constexpr.
        errMsg[j] = errMsgPrefix[j];
    }
    char* appendIdx = &errMsg[PLen - 1];
    for (auto& c : cases) {
        core::intToCptr(i, appendIdx, 2);
        assertionFn(c, errMsg);
        i++;
    }
}

inline void coreInit() {
    core::setGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        constexpr u32 stackFramesToSkip = 3;
        constexpr addr_size stackTraceBufferSize = 4096;
        char trace[stackTraceBufferSize] = {};
        addr_size traceLen = 0;
        core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "[ASSERTION] [EXPR]: " << failedExpr
                  << " [FILE]: " << file
                  << " [LINE]: " << line
                  << " [MSG]: " << errMsg
                  << ANSI_RESET()
                  << std::endl;
        std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;
        throw std::runtime_error("Assertion failed!");
    });
}

inline i32 g_testCount = 0;

#define RunTest(test, ...)                                                                      \
    g_testCount++;                                                                              \
    std::cout << "\t[TEST " << "№ " << g_testCount << " RUNNING] " << ANSI_BOLD(#test) << '\n'; \
    { [[maybe_unused]] auto __notused__ = test(__VA_ARGS__); }                                  \
    std::cout << "\t[TEST " << "№ " << g_testCount << ANSI_BOLD(ANSI_GREEN(" PASSED")) << "] " << ANSI_BOLD(#test) << std::endl;

#if defined(CORE_RUN_COMPILETIME_TESTS) && CORE_RUN_COMPILETIME_TESTS == 1
    #define RunTestCompileTime(test, ...) \
        { [[maybe_unused]] constexpr auto __notused__ = core::force_consteval<test(__VA_ARGS__)>; }
#else
    #define RunTestCompileTime(...)
    #define CORE_RUN_COMPILETIME_TESTS 0
#endif

#define RunTestSuite(suite, ...)                                       \
    std::cout << "[SUITE RUNNING] " << ANSI_BOLD(#suite) << std::endl; \
    suite(__VA_ARGS__);                                                \
    std::cout << "[SUITE " << ANSI_BOLD(ANSI_GREEN("PASSED")) << "] " << ANSI_BOLD(#suite) << std::endl;

i32 runDeferTestsSuite();
i32 runCptrConvTestsSuite();
i32 runCptrTestsSuite();
i32 runMemTestsSuite();

inline i32 runAllTests() {
    coreInit();

    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    RunTestSuite(runDeferTestsSuite);
    RunTestSuite(runCptrConvTestsSuite);
    RunTestSuite(runCptrTestsSuite);
    RunTestSuite(runMemTestsSuite);

    std::cout << '\n';
    std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;

    if constexpr (CORE_RUN_COMPILETIME_TESTS != 1) {
        std::cout << ANSI_YELLOW_START() << ANSI_BOLD_START()
                  << "[WARN] DID NOT RUN COMPILETIME TESTS!"
                  << ANSI_RESET() << std::endl;
    }

    return 0;
}
