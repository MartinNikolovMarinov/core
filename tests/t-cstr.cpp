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

constexpr i32 isHexDigitTest() {
    struct TestCase {
        char in;
        bool expected;
    };

    TestCase cases[] = {
        { ' ', false }, { '-', false }, { 'O', false }, { 'x', false }, { 'X', false }, { '+', false },

        { '0', true }, { '1', true }, { '2', true }, { '3', true }, { '4', true }, { '5', true },
        { '6', true }, { '7', true }, { '8', true }, { '9', true }, { 'A', true }, { 'B', true },
        { 'C', true }, { 'D', true }, { 'E', true }, { 'F', true },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::isHexDigit(c.in) == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 digitToCharTest() {
    struct TestCase {
        i32 in;
        char expected;
    };

    constexpr TestCase cases[] = {
        { 0, '0' },
        { 1, '1' },
        { 2, '2' },
        { 3, '3' },
        { 4, '4' },
        { 5, '5' },
        { 6, '6' },
        { 7, '7' },
        { 8, '8' },
        { 9, '9' },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::digitToChar(i8(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(i16(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(i32(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(i64(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(u8(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(u16(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(u32(c.in)) == c.expected, cErr);
        CT_CHECK(core::digitToChar(u64(c.in)) == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 charToDigitTest() {
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
        CT_CHECK(core::charToDigit<i8>(c.in) == i8(c.expected), cErr);
        CT_CHECK(core::charToDigit<i16>(c.in) == i16(c.expected), cErr);
        CT_CHECK(core::charToDigit<i32>(c.in) == i32(c.expected), cErr);
        CT_CHECK(core::charToDigit<i64>(c.in) == i64(c.expected), cErr);
        CT_CHECK(core::charToDigit<u8>(c.in) == u8(c.expected), cErr);
        CT_CHECK(core::charToDigit<u16>(c.in) == u16(c.expected), cErr);
        CT_CHECK(core::charToDigit<u32>(c.in) == u32(c.expected), cErr);
        CT_CHECK(core::charToDigit<u64>(c.in) == u64(c.expected), cErr);
        CT_CHECK(core::charToDigit<f32>(c.in) == f32(c.expected), cErr);
        CT_CHECK(core::charToDigit<f64>(c.in) == f64(c.expected), cErr);
        return 0;
    });

    return ret;
}

constexpr i32 lowerAndUpperASCIITest() {
    struct TestCase {
        char in;
        char lower;
        char upper;
        bool isLower;
        bool isUpper;
    };

    // Behaviour is undefined for symbols that are not letters.
    constexpr TestCase cases[] = {
        { 'a', 'a', 'A', true, false },
        { 'b', 'b', 'B', true, false },
        { 'c', 'c', 'C', true, false },
        { 'y', 'y', 'Y', true, false },
        { 'z', 'z', 'Z', true, false },

        { 'A', 'a', 'A', false, true },
        { 'B', 'b', 'B', false, true },
        { 'C', 'c', 'C', false, true },
        { 'Y', 'y', 'Y', false, true },
        { 'Z', 'z', 'Z', false, true },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::toLowerCaseASCII(c.in) == c.lower, cErr);
        CT_CHECK(core::toUpperCaseASCII(c.in) == c.upper, cErr);
        CT_CHECK(core::isLowerCaseASCII(c.in) == c.isLower, cErr);
        CT_CHECK(core::isUpperCaseASCII(c.in) == c.isUpper, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cmpIgnoreCaseASCIITest() {
    struct TestCase {
        char a;
        char b;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    constexpr TestCase cases[] = {
        { 'a', 'a', TestCase::zero },
        { 'A', 'a', TestCase::zero },
        { 'a', 'A', TestCase::zero },
        { 'A', 'A', TestCase::zero },

        { 'z', 'z', TestCase::zero },
        { 'Z', 'z', TestCase::zero },
        { 'z', 'Z', TestCase::zero },
        { 'Z', 'Z', TestCase::zero },

        { 'b', 'c', TestCase::negative },
        { 'B', 'c', TestCase::negative },
        { 'b', 'C', TestCase::negative },
        { 'B', 'C', TestCase::negative },

        { 'c', 'b', TestCase::positive },
        { 'c', 'B', TestCase::positive },
        { 'C', 'b', TestCase::positive },
        { 'C', 'B', TestCase::positive },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case 1:  CT_CHECK(core::cmpIgnoreCaseANSI(c.a, c.b) > 0, cErr);  break;
            case -1: CT_CHECK(core::cmpIgnoreCaseANSI(c.a, c.b) < 0, cErr);  break;
            case 0:  CT_CHECK(core::cmpIgnoreCaseANSI(c.a, c.b) == 0, cErr); break;
        }

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
        {
            addr_size len = core::cstrLen(c.in);
            CT_CHECK(len == c.expected, cErr);
        }
        IS_NOT_CONST_EVALUATED {
            addr_size len = core::cstrLen(reinterpret_cast<const uchar*>(c.in));
            CT_CHECK(len == c.expected, cErr);
        }
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
        {
            const char* res = core::cstrSkipSpace(c.src);
            CT_CHECK(core::memcmp(res, core::cstrLen(res), c.expected, core::cstrLen(c.expected)) == 0, cErr);
        }
        IS_NOT_CONST_EVALUATED {
            const char* res = core::cstrSkipSpace(const_cast<char*>(c.src));
            CT_CHECK(core::memcmp(res, core::cstrLen(res), c.expected, core::cstrLen(c.expected)) == 0, cErr);
        }
        return 0;
    });

    return ret;
}

i32 cstrCmpUnsafeTest() {
    struct TestCase {
        const char* a;
        const char* b;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    constexpr TestCase cases[] = {
        { nullptr, nullptr, TestCase::zero },
        { nullptr, "", TestCase::zero }, // might want this to be false
        { "", nullptr, TestCase::zero },
        { "", "", TestCase::zero },
        { "asdzxcasd", "", TestCase::positive },
        { "abc", "abc", TestCase::zero },
        { "abc", "abd", TestCase::negative },
        { "abd", "abc", TestCase::positive },
        { "abc0", "abc", TestCase::positive },
        { "abc", "abc0", TestCase::negative },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case 1:  CT_CHECK(core::cstrCmpUnsafe(c.a, c.b) > 0, cErr);  break;
            case -1: CT_CHECK(core::cstrCmpUnsafe(c.a, c.b) < 0, cErr);  break;
            case 0:  CT_CHECK(core::cstrCmpUnsafe(c.a, c.b) == 0, cErr); break;
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 cstrEqUnsafeTest() {

    struct TestCase {
        const char* a;
        const char* b;
        bool expected;
    };

    constexpr TestCase cases[] = {
        { nullptr, nullptr, true },
        { nullptr, "", true }, // might want this to be false
        { "", nullptr, true },
        { "", "", true },
        { "asdzxcasd", "", false },
        { "abc", "abc", true },
        { "abc", "abd", false },
        { "abc", "abc0", false },
        { "abc0", "abc", false },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::cstrEqUnsafe(c.a, c.b) == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 runCstrTestsSuite() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(isDigitTest);
    if (runTest(tInfo, isDigitTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(isHexDigitTest);
    if (runTest(tInfo, isHexDigitTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(digitToCharTest);
    if (runTest(tInfo, digitToCharTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(charToDigitTest);
    if (runTest(tInfo, charToDigitTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(lowerAndUpperASCIITest);
    if (runTest(tInfo, lowerAndUpperASCIITest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cmpIgnoreCaseASCIITest);
    if (runTest(tInfo, cmpIgnoreCaseASCIITest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(isWhiteSpaceTest);
    if (runTest(tInfo, isWhiteSpaceTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrLenTest);
    if (runTest(tInfo, cstrLenTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrSkipWhiteSpaceTest);
    if (runTest(tInfo, cstrSkipWhiteSpaceTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrCmpUnsafeTest);
    if (runTest(tInfo, cstrCmpUnsafeTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrEqUnsafeTest);
    if (runTest(tInfo, cstrEqUnsafeTest) != 0) { return -1; }

    return 0;
}

constexpr i32 runCompiletimeCstrTestsSuite() {
    RunTestCompileTime(isDigitTest);
    RunTestCompileTime(isHexDigitTest);
    RunTestCompileTime(digitToCharTest);
    RunTestCompileTime(charToDigitTest);
    RunTestCompileTime(lowerAndUpperASCIITest);
    RunTestCompileTime(cmpIgnoreCaseASCIITest);
    RunTestCompileTime(isWhiteSpaceTest);
    RunTestCompileTime(cstrLenTest);
    RunTestCompileTime(cstrSkipWhiteSpaceTest);

    return 0;
}
