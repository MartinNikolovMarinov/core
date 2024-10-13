#pragma once

#include <core_ansi_escape_codes.h>
#include <core_API.h>
#include <core_cstr_conv.h>
#include <core_exec_ctx.h>
#include <core_intrinsics.h>
#include <core_types.h>
#include <core_hash.h>

#include <iostream>
#include <chrono>

namespace core::testing {

using namespace coretypes;

struct ConstructorTester {
    static constexpr i32 defaultValue = 7;

    i32 a;
    ConstructorTester() : a(defaultValue) { g_defaultCtorCalled++; }
    ConstructorTester(const ConstructorTester& other) : a(other.a) { g_copyCtorCalled++; }
    ConstructorTester(ConstructorTester&& other) : a(std::move(other.a)) {
        g_moveCtorCalled++;
        other.a = defaultValue;
     }
    ~ConstructorTester() {
        g_destructorsCalled++;
        a = defaultValue;
    }

    ConstructorTester& operator=(const ConstructorTester& other) {
        a = other.a;
        g_assignmentsCopyCalled++;
        return *this;
    }

    ConstructorTester& operator=(ConstructorTester&& other) {
        a = std::move(other.a);
        g_assignmentsMoveCalled++;
        other.a = defaultValue;
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
    static bool noAssignmentsCalled()   { return assignmentsTotalCalled() == 0; }

private:
    inline static i32 g_destructorsCalled;
    inline static i32 g_defaultCtorCalled;
    inline static i32 g_copyCtorCalled;
    inline static i32 g_moveCtorCalled;
    inline static i32 g_assignmentsCopyCalled;
    inline static i32 g_assignmentsMoveCalled;
};

using CT = ConstructorTester;

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
[[nodiscard]] constexpr i32 executeTestTable(const char (&errMsgPrefix)[PLen],
                                             const TCase (&cases)[NCases],
                                             Afunc assertionFn) {
    addr_size i = 0;
    char errMsg[PLen + 20] = {}; // The 20 is for the test case index number.
    for (addr_size j = 0; j < PLen; j++) { // NOTE: intentionally not using memcopy, because this needs to work in constexpr.
        errMsg[j] = errMsgPrefix[j];
    }
    char* appendIdx = &errMsg[PLen - 1];
    for (auto& c : cases) {
        core::intToCstr(i, appendIdx, PLen - 1, 2);
        if (assertionFn(c, errMsg) != 0) return -1;
        i++;
    }
    return 0;
}

inline i32 g_testCount = 0;

CORE_API_EXPORT char* elapsedTimeToStr(char out[256], std::chrono::nanoseconds deltaTime);
CORE_API_EXPORT char* memoryUsedToStr(char out[128], addr_size deltaMemory);

struct TestInfo {
    const char* name = nullptr;
    const char* description = nullptr;
    bool useAnsiColors = true;
    bool trackMemory = true;
    bool trackTime = true;
    bool trackTicks = true;
    bool detectLeaks = false;
    bool exitOnFailure = false;

    TestInfo() = default;
    TestInfo(const char* _name) : name(_name) {}
    TestInfo(const char* _name, bool _useAnsiColors) : name(_name), useAnsiColors(_useAnsiColors) {}
};

namespace detail {

constexpr inline const char* passedOrFailedStr(bool passed, bool useAnsiColors) {
    if (useAnsiColors) {
        return passed ? ANSI_GREEN("PASSED") : ANSI_RED("FAILED");
    }
    return passed ? "PASSED" : "FAILED";
}

} // namespace detail

template <typename TFunc, typename... Args>
i32 runTest(const TestInfo& info, TFunc fn, Args... args) {
    g_testCount++;

    const char* testName = info.name;
    bool useAnsiColors = info.useAnsiColors;

    auto allocatedBefore = core::totalMemoryAllocated();
    auto inUseBefore = core::inUseMemory();
    auto startTicks = core::intrin_getCpuTicks();
    auto start = std::chrono::steady_clock::now();

    std::cout << "\t[TEST " << "№ " << g_testCount << " RUNNING] " << testName;
    if (info.description) {
        std::cout << " ( " << info.description << " )";
    }
    std::cout << '\n';

    auto returnCode = fn(args...);

    auto allocatedAfter = core::totalMemoryAllocated();
    auto inUseAfter = core::inUseMemory();
    auto endTicks = core::intrin_getCpuTicks();
    auto end = std::chrono::steady_clock::now();

    auto deltaAllocatedMemory = allocatedAfter - allocatedBefore;
    auto deltaInUseMemory = inUseAfter - inUseBefore;
    auto deltaTicks = endTicks - startTicks;
    auto deltaTimeNs = end - start;

    std::cout << "\t[TEST " << "№ " << g_testCount << " "
              << detail::passedOrFailedStr(returnCode == 0, useAnsiColors) << "] "
              << testName;
    if (info.description) {
        std::cout << " ( " << info.description << " )";
    }

    if (info.detectLeaks && deltaInUseMemory != 0) {
        std::cout << ANSI_RED(" !!LEAKED MEMORY!!");
        returnCode = -1;
    }

    bool isFirst = true;

    if (info.trackTime) {
        char elapsedTimeBuffer[256];
        if (isFirst) std::cout << " [ ";
        else std::cout << ", ";
        std::cout << "time: " << elapsedTimeToStr(elapsedTimeBuffer, deltaTimeNs);
        isFirst = false;
    }

    if (info.trackTicks) {
        if (isFirst) std::cout << " [ ";
        else std::cout << ", ";
        std::cout << "ticks: " << deltaTicks;
        isFirst = false;
    }

    if (info.trackMemory) {
        char buff[256];
        if (isFirst) std::cout << " [ ";
        else std::cout << ", ";
        std::cout << "memory: {"
                  << " allocated: " << memoryUsedToStr(buff, deltaAllocatedMemory)
                  << ", in_use: " << memoryUsedToStr(buff, deltaInUseMemory)
                  << " }";
        isFirst = false;
    }

    if (!isFirst) std::cout << " ]";

    if (info.exitOnFailure && returnCode != 0) {
        std::cout << std::endl;
        Panic(false, "Test failed, exiting...");
    }

    core::clearActiveAllocatorForThread();
    std::cout << std::endl;
    return returnCode;
}

struct TestSuiteInfo {
    const char* name = nullptr;
    bool useAnsiColors = true;

    TestSuiteInfo() = default;
    TestSuiteInfo(const char* _name) : name(_name), useAnsiColors(true) {}
    TestSuiteInfo(const char* _name, bool _useAnsiColors) : name(_name), useAnsiColors(_useAnsiColors) {}
};

template <typename TSuite>
i32 runTestSuite(const TestSuiteInfo& info, TSuite suite) {

    const char* suiteName = info.name;
    bool useAnsiColors = info.useAnsiColors;

    std::cout << "[SUITE RUNNING] " << suiteName << std::endl;
    i32 returnCode = suite();

    std::cout << "[SUITE " << detail::passedOrFailedStr(returnCode == 0, useAnsiColors) << "] " << suiteName << std::endl;
    return returnCode;
}

#if defined(CORE_RUN_COMPILETIME_TESTS) && CORE_RUN_COMPILETIME_TESTS == 1
    #define RunTestCompileTime(test, ...) \
        { [[maybe_unused]] constexpr auto __notused__ = core::force_consteval<test(__VA_ARGS__)>; }
#else
    #define RunTestCompileTime(...)
    #define CORE_RUN_COMPILETIME_TESTS 0
#endif

// CORE TEST CHECK Macro
#define CT_CHECK(...) C_VFUNC(CT_CHECK, __VA_ARGS__)
#define CT_CHECK1(expr) CT_CHECK2(expr, "")
#define CT_CHECK2(expr, msg)                                                     \
    if (!(expr)) {                                                               \
        std::cerr << "\t\t[Test failed] ";                                       \
        if (core::cstrLen((msg)) > 0) {                                          \
            std::cerr << "message: \"" << (msg) << "\" ";                        \
        }                                                                        \
        std::cerr << "at " << __FILE__ << ":" << __LINE__ << " in " << __func__; \
        std::cerr << std::endl;                                                  \
        volatile i32* __forceCrash = nullptr;                                    \
        [[maybe_unused]] i32 __ignored = *__forceCrash;                          \
        return -1;                                                               \
    }

} // namespace core::testing

namespace core {

using namespace coretypes;

template <>
inline addr_size hash(const testing::CT& key) {
    return addr_size(key.a);
}

template <>
inline bool eq(const testing::CT& a, const testing::CT& b) {
    return a.a == b.a;
}

static_assert(core::HashableConcept<testing::CT>, "CT must satisfy the HashableConcept.");

} // namespace core

