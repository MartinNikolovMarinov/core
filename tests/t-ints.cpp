#include "t-index.h"

constexpr i32 digitCountTest() {
    {
        struct TestCase {
            i32 value;
            u32 expected;
        };

        TestCase cases[] = {
            { 0,           1 },
            { 1,           1 },
            { 11,          2 },
            { 111,         3 },
            { 1111,        4 },
            { 11111,       5 },
            { 111111,      6 },
            { 1111111,     7 },
            { 11111111,    8 },
            { 111111111,   9 },
            { 1111111111, 10 },

            { -1,           1 },
            { -11,          2 },
            { -111,         3 },
            { -1111,        4 },
            { -11111,       5 },
            { -111111,      6 },
            { -1111111,     7 },
            { -11111111,    8 },
            { -111111111,   9 },
            { -1111111111, 10 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            CT_CHECK(core::digitCount(c.value) == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }
    {
        struct TestCase {
            u32 value;
            u32 expected;
        };

        TestCase cases[] = {
            { 0,           1 },
            { 1,           1 },
            { 11,          2 },
            { 111,         3 },
            { 1111,        4 },
            { 11111,       5 },
            { 111111,      6 },
            { 1111111,     7 },
            { 11111111,    8 },
            { 111111111,   9 },
            { 1111111111, 10 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            CT_CHECK(core::digitCount(c.value) == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }
    {
        struct TestCase {
            i64 value;
            u32 expected;
        };

        TestCase cases[] = {
            { 0,                    1 },
            { 1,                    1 },
            { 11,                   2 },
            { 111,                  3 },
            { 1111,                 4 },
            { 11111,                5 },
            { 111111,               6 },
            { 1111111,              7 },
            { 11111111,             8 },
            { 111111111,            9 },
            { 1111111111,          10 },
            { 11111111111,         11 },
            { 111111111111,        12 },
            { 1111111111111,       13 },
            { 11111111111111,      14 },
            { 111111111111111,     15 },
            { 1111111111111111,    16 },
            { 11111111111111111,   17 },
            { 111111111111111111,  18 },
            { 1111111111111111111, 19 },

            { -1,                    1 },
            { -11,                   2 },
            { -111,                  3 },
            { -1111,                 4 },
            { -11111,                5 },
            { -111111,               6 },
            { -1111111,              7 },
            { -11111111,             8 },
            { -111111111,            9 },
            { -1111111111,          10 },
            { -11111111111,         11 },
            { -111111111111,        12 },
            { -1111111111111,       13 },
            { -11111111111111,      14 },
            { -111111111111111,     15 },
            { -1111111111111111,    16 },
            { -11111111111111111,   17 },
            { -111111111111111111,  18 },
            { -1111111111111111111, 19 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            CT_CHECK(core::digitCount(c.value) == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }
    {
        struct TestCase {
            u64 value;
            u32 expected;
        };

        TestCase cases[] = {
            { 0,                    1 },
            { 1,                    1 },
            { 11,                   2 },
            { 111,                  3 },
            { 1111,                 4 },
            { 11111,                5 },
            { 111111,               6 },
            { 1111111,              7 },
            { 11111111,             8 },
            { 111111111,            9 },
            { 1111111111,          10 },
            { 11111111111,         11 },
            { 111111111111,        12 },
            { 1111111111111,       13 },
            { 11111111111111,      14 },
            { 111111111111111,     15 },
            { 1111111111111111,    16 },
            { 11111111111111111,   17 },
            { 111111111111111111,  18 },
            { 1111111111111111111, 19 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            CT_CHECK(core::digitCount(c.value) == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 runIntsTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(digitCountTest);
    if (runTest(tInfo, digitCountTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeIntsTestsSuite() {
    RunTestCompileTime(digitCountTest);

    return 0;
}
