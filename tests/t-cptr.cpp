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

constexpr i32 cptrLenTest() {
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
        addr_size len = core::cptrLen(c.in);
        CT_CHECK(len == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cptrCmpTests() {
    struct TestCase {
        const char* a;
        const char* b;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    TestCase cases[] = {
        { nullptr, nullptr, TestCase::zero },
        { nullptr, "",      TestCase::negative},
        { "",      nullptr, TestCase::positive },
        { "",      "",      TestCase::zero },
        { "a",     "",      TestCase::positive },
        { "",      "a",     TestCase::negative },
        { "a",     "a",     TestCase::zero },
        { "a",     "b",     TestCase::negative },
        { "b",     "a",     TestCase::positive },
        { "abc",   "abc",   TestCase::zero },
        { "abd",   "abc",   TestCase::positive },
        { "abb",   "abc",   TestCase::negative },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case TestCase::positive:
                CT_CHECK(core::cptrCmp(c.a, core::cptrLen(c.a), c.b, core::cptrLen(c.b)) > 0, cErr);
                break;
            case TestCase::negative:
                CT_CHECK(core::cptrCmp(c.a, core::cptrLen(c.a), c.b, core::cptrLen(c.b)) < 0, cErr);
                break;
            case TestCase::zero:
                CT_CHECK(core::cptrCmp(c.a, core::cptrLen(c.a), c.b, core::cptrLen(c.b)) == 0, cErr);
                break;
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return ret;
}

constexpr i32 cptrEqTest() {
    struct TestCase {
        const char* a;
        const char* b;
        addr_size len;
        bool expected;
    };

    TestCase cases[] = {
        { nullptr, nullptr, 0, true },
        { nullptr, "",      0, false },
        { "",      nullptr, 0, false },
        { "",      "",      0, true },
        { "a",     "a",     1, true },
        { "a",     "b",     1, false },
        { "abc",   "abc",   3, true },
        { "abd",   "ab9",   2, true },
        { "a\0c",  "a\0c",  3, true },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::cptrEq(c.a, c.b, c.len) == c.expected, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cptrCopyTest() {
    const char* src = "1234567890";
    char dst[20] = {};
    core::cptrCopy(dst, src, core::cptrLen(src));

    CT_CHECK(core::cptrLen(src) == core::cptrLen(dst));
    CT_CHECK(core::cptrEq(src, dst, core::cptrLen(dst)));
    CT_CHECK(dst[0] == '1');
    CT_CHECK(dst[1] == '2');
    CT_CHECK(dst[2] == '3');
    CT_CHECK(dst[3] == '4');
    CT_CHECK(dst[4] == '5');
    CT_CHECK(dst[5] == '6');
    CT_CHECK(dst[6] == '7');
    CT_CHECK(dst[7] == '8');
    CT_CHECK(dst[8] == '9');
    CT_CHECK(dst[9] == '0');
    CT_CHECK(dst[10] == core::term_char);

    for (u32 i = 0; i < 20; ++i) dst[i] = 0;
    core::cptrCopy(dst, src, 5);

    CT_CHECK(core::cptrEq(dst, "12345", core::cptrLen("12345")));
    CT_CHECK(dst[0] == '1');
    CT_CHECK(dst[1] == '2');
    CT_CHECK(dst[2] == '3');
    CT_CHECK(dst[3] == '4');
    CT_CHECK(dst[4] == '5');
    CT_CHECK(dst[5] == core::term_char);

    return 0;
}

constexpr i32 cptrIdxOfCharTest() {
    struct TestCase {
        const char* src;
        char val;
        addr_off idx;
    };

    TestCase cases[] = {
        { "1234567890", '1', 0 },
        { "1234567890", '2', 1 },
        { "1234567890", '3', 2 },
        { "1234567890", '4', 3 },
        { "1234567890", '5', 4 },
        { "1234567890", '6', 5 },
        { "1234567890", '7', 6 },
        { "1234567890", '8', 7 },
        { "1234567890", '9', 8 },
        { "1234567890", '0', 9 },
        { "1234567890", 'z', -1 },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_off idx = core::cptrIdxOfChar(c.src, core::cptrLen(c.src), c.val);
        CT_CHECK(idx == c.idx, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cptrIdxOfTest() {
    struct TestCase {
        const char* src;
        const char* val;
        addr_off idx;
    };

    TestCase cases[] = {
        { "", "", 0 },
        { nullptr, "1", -1 },
        { nullptr, nullptr, -1 },
        { "1", nullptr, -1 },
        { "1234", "1", 0 },
        { "1234", "12", 0 },
        { "1234", "123", 0 },
        { "1234", "1234", 0 },
        { "1234", "234", 1 },
        { "1234", "34", 2 },
        { "1234", "4", 3 },
        { "1234", "5", -1 },
        { "1234", "45", -1 },
        { "1234", "345", -1 },
        { "1234", "2345", -1 },
        { "1234", "12345", -1 },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_off idx = core::cptrIdxOf(c.src, core::cptrLen(c.src), c.val, core::cptrLen(c.val));
        CT_CHECK(idx == c.idx, cErr);
        return 0;
    });

    return ret;
}

constexpr i32 cptrSkipWhiteSpaceTest() {
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
        const char* res = core::cptrSkipSpace(c.src);
        CT_CHECK(core::cptrEq(res, c.expected, core::cptrLen(c.expected)), cErr);
        return 0;
    });

    return ret;
};

i32 runCptrTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(isDigitTest);
    if (runTest(tInfo, isDigitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(isWhiteSpaceTest);
    if (runTest(tInfo, isWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrLenTest);
    if (runTest(tInfo, cptrLenTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrCmpTests);
    if (runTest(tInfo, cptrCmpTests) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrEqTest);
    if (runTest(tInfo, cptrEqTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrCopyTest);
    if (runTest(tInfo, cptrCopyTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrIdxOfCharTest);
    if (runTest(tInfo, cptrIdxOfCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrIdxOfTest);
    if (runTest(tInfo, cptrIdxOfTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrSkipWhiteSpaceTest);
    if (runTest(tInfo, cptrSkipWhiteSpaceTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCptrTestsSuite() {
    RunTestCompileTime(isDigitTest);
    RunTestCompileTime(isWhiteSpaceTest);
    RunTestCompileTime(cptrLenTest);
    RunTestCompileTime(cptrCmpTests);
    RunTestCompileTime(cptrEqTest);
    RunTestCompileTime(cptrCopyTest);
    RunTestCompileTime(cptrIdxOfCharTest);
    RunTestCompileTime(cptrIdxOfTest);
    RunTestCompileTime(cptrSkipWhiteSpaceTest);

    return 0;
}
