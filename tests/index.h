#pragma once

#define CORE_DEFAULT_ALLOCATOR() StdAllocator

#include <core.h>

#include <iostream>

using namespace coretypes;

inline void coreInit() {
    core::setGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        constexpr u32 stackFramesToSkip = 3;
        constexpr addr_size stackTraceBufferSize = 4096;
        char trace[stackTraceBufferSize] = {};
        addr_size traceLen = 0;
        core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                  << "\n  [FILE]: " << file << ":" << line
                  << "\n  [MSG]: " << errMsg
                  << ANSI_RESET()
                  << std::endl;
        std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;
        throw std::runtime_error("Assertion failed!");
    });
}

struct ConstructorTester {
    static constexpr i32 defaultValue = 7;

    i32 a;
    ConstructorTester() : a(defaultValue) { g_defaultCtorCalled++; }
    ConstructorTester(const ConstructorTester& other) : a(other.a) { g_copyCtorCalled++; }
    ConstructorTester(ConstructorTester&& other) : a(core::move(other.a)) { g_moveCtorCalled++; }
    ~ConstructorTester() { g_destructorsCalled++; }

    ConstructorTester& operator=(const ConstructorTester& other) {
        a = other.a;
        g_assignmentsCopyCalled++;
        return *this;
    }

    ConstructorTester& operator=(ConstructorTester&& other) {
        a = core::move(other.a);
        g_assignmentsMoveCalled++;
        return *this;
    }

    static void resetCtors() {
        g_defaultCtorCalled = 0;
        g_copyCtorCalled = 0;
        g_moveCtorCalled = 0;
    }

    static void resetDtors() {
        g_destructorsCalled = 0;
    }

    static void resetAssignments() {
        g_assignmentsCopyCalled = 0;
        g_assignmentsMoveCalled = 0;
    }

    static void resetAll() {
        resetCtors();
        resetDtors();
        resetAssignments();
    }

    static i32 defaultCtorCalled()      { return g_defaultCtorCalled; }
    static i32 copyCtorCalled()         { return g_copyCtorCalled; }
    static i32 moveCtorCalled()         { return g_moveCtorCalled; }
    static i32 totalCtorsCalled()       { return g_defaultCtorCalled + g_copyCtorCalled + g_moveCtorCalled; }
    static i32 assignmentsCopyCalled()  { return g_assignmentsCopyCalled; }
    static i32 assignmentsMoveCalled()  { return g_assignmentsMoveCalled; }
    static i32 assignmentsTotalCalled() { return g_assignmentsCopyCalled + g_assignmentsMoveCalled; }
    static i32 dtorsCalled()            { return g_destructorsCalled; }
    static bool noCtorsCalled()         { return totalCtorsCalled() == 0; }

private:
    inline static i32 g_destructorsCalled;
    inline static i32 g_defaultCtorCalled;
    inline static i32 g_copyCtorCalled;
    inline static i32 g_moveCtorCalled;
    inline static i32 g_assignmentsCopyCalled;
    inline static i32 g_assignmentsMoveCalled;
};

using CT = ConstructorTester;

struct StaticVariableDefaultCtorTester {
    // NOTE: This is a terrible idea, but it still should work.
    inline static i32 nextId = 0;
    i32 a = nextId++;
};

using SVCT = StaticVariableDefaultCtorTester;

// ##################### Helper functions for testing ##################################################################

/**
 * \brief This code is quite complex, because it does zero allocations, but it's purpose is quite simple. It iterates over
 *        a table of test cases, and executes the assertion function on each one. The assertion function, the test case
 *        table, and the error message prefix are all passed in as arguments. Every error message is pre-allocated on
 *        the stack, and the test case index is manipulated inside the char array.
 *
 * \param errMsgPrefix The error message prefix.
 * \param cases The test case table.
 * \param assertionFn The assertion function.
*/
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

// ##################### Test suites ###################################################################################

i32 runArrTestsSuite();
i32 runBitsTestsSuite();
i32 runCptrConvTestsSuite();
i32 runCptrTestsSuite();
i32 runDeferTestsSuite();
i32 runExpectedTestsSuite();
i32 runHashTestsSuite();
i32 runIntrinsicsTestsSuite();
i32 runIntsTestsSuite();
i32 runMathTestsSuite();
i32 runMemTestsSuite();
i32 runTraitsTestsSuite();
i32 runTupleTestsSuite();

i32 runBumpAllocatorTestsSuite();
i32 runStdAllocatorTestsSuite();
i32 runStdStatsAllocatorTestsSuite();

i32 runPltStacktraceTestsSuite();

inline i32 runAllTests() {
    coreInit();

    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    RunTestSuite(runArrTestsSuite);
    RunTestSuite(runBitsTestsSuite);
    RunTestSuite(runCptrConvTestsSuite);
    RunTestSuite(runCptrTestsSuite);
    RunTestSuite(runDeferTestsSuite);
    RunTestSuite(runExpectedTestsSuite);
    RunTestSuite(runHashTestsSuite);
    RunTestSuite(runIntrinsicsTestsSuite);
    RunTestSuite(runIntsTestsSuite);
    RunTestSuite(runMathTestsSuite);
    RunTestSuite(runMemTestsSuite);
    RunTestSuite(runTraitsTestsSuite);
    RunTestSuite(runTupleTestsSuite);

    RunTestSuite(runBumpAllocatorTestsSuite);
    RunTestSuite(runStdAllocatorTestsSuite);
    RunTestSuite(runStdStatsAllocatorTestsSuite);

    #if defined(CORE_DEBUG) && CORE_DEBUG == 1
        // Stacktrace should only be expected to work in debug builds.
        RunTestSuite(runPltStacktraceTestsSuite);
    #endif

    std::cout << '\n';
    std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;

    if constexpr (CORE_RUN_COMPILETIME_TESTS != 1) {
        std::cout << ANSI_YELLOW_START() << ANSI_BOLD_START()
                  << "[WARN] DID NOT RUN COMPILETIME TESTS!"
                  << ANSI_RESET() << std::endl;
    }

    return 0;
}
