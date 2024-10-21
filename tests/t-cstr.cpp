#include "t-index.h"

constexpr i32 isDigitTest() {
    struct TestCase {
        char in;
        bool expected;
    };

    TestCase cases[] = {
        { ' ', false },
        { '-', false },
        { '/', false },
        { '0', true },
        { '1', true },
        { '2', true },
        { '3', true },
        { '4', true },
        { '5', true },
        { '6', true },
        { '7', true },
        { '8', true },
        { '9', true },
        { ':', false },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::isDigit(c.in) == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 isWhiteSpaceTest() {
    struct TestCase {
        char in;
        bool expected;
    };

    TestCase cases[] = {
        { ' ', true },
        { '\n', true },
        { '\t', true },
        { '\r', true },
        { 'a', false },
        { '\\', false }
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::isWhiteSpace(c.in) == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cstrLenTest() {
    struct TestCase {
        const char* in;
        addr_size expected;
    };

    TestCase cases[] = {
        { "", 0 },
        { "a", 1 },
        { "abc", 3 },
        { "1234567890", 10 },
        { "12345678901234567890", 20 },
        { "123456789012345678901234567890", 30 },
        { "1234567890123456789012345678901234567890", 40 },
        { "12345678901234567890123456789012345678901234567890", 50 },
        { "123456789012345678901234567890123456789012345678901234567890", 60 },
        { "1234567890123456789012345678901234567890123456789012345678901234567890", 70 },
        { "12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80 },

        { "asd\0aszxc", 3 } // This is where danger lies.
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_size len = core::cstrLen(c.in);
        CT_CHECK(len == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cstrSkipWhiteSpaceTest() {
    struct TestCase {
        const char* src;
        const char* expected;
    };

    TestCase cases[] = {
        { nullptr, nullptr },
        { "", "" },
        { " aa", "aa" },
        { "        aa", "aa" },
        { "\taa", "aa" },
        { "\taa  ", "aa  " },
        { "\naa", "aa" },
        { "\n\raa", "aa" },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        const char* res = core::cstrSkipSpace(c.src);
        CT_CHECK(core::memcmp(res, core::cstrLen(res), c.expected, core::cstrLen(c.expected)) == 0, cErr);
        return 0;
    });

    return ret;
};

constexpr i32 charToIntTest() {
    struct TestCase {
        char in;
        i32 expected;
    };

    constexpr TestCase cases[] = {
        { '0', 0 },
        { '1', 1 },
        { '2', 2 },
        { '3', 3 },
        { '4', 4 },
        { '5', 5 },
        { '6', 6 },
        { '7', 7 },
        { '8', 8 },
        { '9', 9 },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::toDigit<i8>(c.in) == i8(c.expected), cErr);
        CT_CHECK(core::toDigit<i16>(c.in) == i16(c.expected), cErr);
        CT_CHECK(core::toDigit<i32>(c.in) == i32(c.expected), cErr);
        CT_CHECK(core::toDigit<i64>(c.in) == i64(c.expected), cErr);
        CT_CHECK(core::toDigit<u8>(c.in) == u8(c.expected), cErr);
        CT_CHECK(core::toDigit<u16>(c.in) == u16(c.expected), cErr);
        CT_CHECK(core::toDigit<u32>(c.in) == u32(c.expected), cErr);
        CT_CHECK(core::toDigit<u64>(c.in) == u64(c.expected), cErr);
        CT_CHECK(core::toDigit<f32>(c.in) == f32(c.expected), cErr);
        CT_CHECK(core::toDigit<f64>(c.in) == f64(c.expected), cErr);
        return 0;
    });

    return ret;
}

i32 runCstrTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(isDigitTest);
    if (runTest(tInfo, isDigitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(isWhiteSpaceTest);
    if (runTest(tInfo, isWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrLenTest);
    if (runTest(tInfo, cstrLenTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrSkipWhiteSpaceTest);
    if (runTest(tInfo, cstrSkipWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(charToIntTest);
    if (runTest(tInfo, charToIntTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCstrTestsSuite() {
    RunTestCompileTime(isDigitTest);
    RunTestCompileTime(isWhiteSpaceTest);
    RunTestCompileTime(cstrLenTest);
    RunTestCompileTime(cstrSkipWhiteSpaceTest);
    RunTestCompileTime(charToIntTest);

    return 0;
}
