#pragma once

#include <types.h>
#include <char_ptr.h>

using namespace coretypes;

// This code is quite complex, because it is zero allocation, but it does somthing very simple.
// It iterates over a table of test cases, and executes the assertion function on each one.
// The assertion function, the test case table, and the error message prefix are all passed in as arguments.
// Every error message is pre-allocated on the stack, and the test case index is manipulated inside the char array.
template <i32 PLen, typename TCase, i32 NCases, typename Afunc>
constexpr void executeTestTable(const char (&errMsgPrefix)[PLen], const TCase (&cases)[NCases], Afunc assertionFn) {
    i32 i = 0;
    char errMsg[PLen + 20] = {}; // The 20 is for the test case index number.
    for (i32 j = 0; j < PLen; j++) { // NOTE: intentionally not using memcopy, because this needs to work in constexpr.
        errMsg[j] = errMsgPrefix[j];
    }
    char* appendIdx = &errMsg[PLen - 1];
    for (auto& c : cases) {
        core::int_to_cptr(i, appendIdx, 2);
        assertionFn(c, errMsg);
        i++;
    }
}

struct ConstructorTester {
    static constexpr i32 defaultValue = 7;

    i32 a;
    ConstructorTester() : a(defaultValue) { g_defaultCtorCalled++; }
    ConstructorTester(const ConstructorTester& other) : a(other.a) { g_copyCtorCalled++; }
    ConstructorTester(ConstructorTester&& other) : a(core::move(other.a)) { g_moveCtorCalled++; }
    ~ConstructorTester() { g_destructorsCalled++; }

    static void resetCtors() {
        g_defaultCtorCalled = 0;
        g_copyCtorCalled = 0;
        g_moveCtorCalled = 0;
    }

    static void resetDtors() {
        g_destructorsCalled = 0;
    }

    static void resetAll() {
        resetCtors();
        resetDtors();
    }

    static i32 defaultCtorCalled() { return g_defaultCtorCalled; }
    static i32 copyCtorCalled()    { return g_copyCtorCalled; }
    static i32 moveCtorCalled()    { return g_moveCtorCalled; }
    static i32 totalCtorsCalled()  { return g_defaultCtorCalled + g_copyCtorCalled + g_moveCtorCalled; }
    static i32 dtorsCalled()       { return g_destructorsCalled; }
    static bool noCtorsCalled()    { return totalCtorsCalled() == 0; }

private:
    inline static i32 g_destructorsCalled;
    inline static i32 g_defaultCtorCalled;
    inline static i32 g_copyCtorCalled;
    inline static i32 g_moveCtorCalled;
};

using CT = ConstructorTester;
