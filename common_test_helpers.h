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
    static i32 nextId;
    i32 a = nextId++;
};
i32 StaticVariableDefaultCtorTester::nextId = 0;

using SVCT = StaticVariableDefaultCtorTester;

// Common types hash map related functions

template<>
addr_size core::hash(const i32& key) {
    addr_size h = addr_size(core::simple_hash_32(reinterpret_cast<const void*>(&key), sizeof(key)));
    return h;
}

template<>
bool core::eq(const i32& a, const i32& b) {
    return a == b;
}

template<>
addr_size core::hash(const core::str_view& key) {
    addr_size h = addr_size(core::simple_hash_32(key.data(), key.len()));
    return h;
}

template<>
bool core::eq(const core::str_view& a, const core::str_view& b) {
    return a.eq(b);
}