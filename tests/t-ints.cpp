#include "t-index.h"

constexpr i32 digitCountTest() {
    {
        struct TestCase {
            i32 value;
            u32 expected;
        };

        TestCase cases[] = {
            { 0,           1 },
            { 2,           1 },
            { 21,          2 },
            { 214,         3 },
            { 2147,        4 },
            { 21474,       5 },
            { 214748,      6 },
            { 2147483,     7 },
            { 21474836,    8 },
            { 214748364,   9 },
            { 2147483647, 10 },

            { -1,           1 },
            { -2,           1 },
            { -21,          2 },
            { -214,         3 },
            { -2147,        4 },
            { -21474,       5 },
            { -214748,      6 },
            { -2147483,     7 },
            { -21474836,    8 },
            { -214748364,   9 },
            { -2147483647, 10 },
            { -2147483648, 10 },
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
            { 2,           1 },
            { 21,          2 },
            { 214,         3 },
            { 2147,        4 },
            { 21474,       5 },
            { 214748,      6 },
            { 2147483,     7 },
            { 21474836,    8 },
            { 214748364,   9 },
            { 2147483647, 10 },
            { 2147483648, 10 },
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
            { 0ll,                   1 },
            { 9ll,                   1 },
            { 92ll,                  2 },
            { 922ll,                 3 },
            { 9223ll,                4 },
            { 92233ll,               5 },
            { 922337ll,              6 },
            { 9223372ll,             7 },
            { 92233720ll,            8 },
            { 922337203ll,           9 },
            { 9223372036ll,          10 },
            { 92233720368ll,         11 },
            { 922337203685ll,        12 },
            { 9223372036854ll,       13 },
            { 92233720368547ll,      14 },
            { 922337203685477ll,     15 },
            { 9223372036854775ll,    16 },
            { 92233720368547758ll,   17 },
            { 922337203685477580ll,  18 },
            { 9223372036854775807ll, 19 },

            { -1ll,                         1 },
            { -9ll,                         1 },
            { -92ll,                        2 },
            { -922ll,                       3 },
            { -9223ll,                      4 },
            { -92233ll,                     5 },
            { -922337ll,                    6 },
            { -9223372ll,                   7 },
            { -92233720ll,                  8 },
            { -922337203ll,                 9 },
            { -9223372036ll,                10 },
            { -92233720368ll,               11 },
            { -922337203685ll,              12 },
            { -9223372036854ll,             13 },
            { -92233720368547ll,            14 },
            { -922337203685477ll,           15 },
            { -9223372036854775ll,          16 },
            { -92233720368547758ll,         17 },
            { -922337203685477580ll,        18 },
            { -9223372036854775807ll,       19 },
            { -9223372036854775807ll - 1ll, 19 }, // c++ number literals are soo stupid...
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
            { 0ull,                     1 },
            { 1ull,                     1 },
            { 18ull,                    2 },
            { 184ull,                   3 },
            { 1844ull,                  4 },
            { 18446ull,                 5 },
            { 184467ull,                6 },
            { 1844674ull,               7 },
            { 18446744ull,              8 },
            { 184467440ull,             9 },
            { 1844674407ull,           10 },
            { 18446744073ull,          11 },
            { 184467440737ull,         12 },
            { 1844674407370ull,        13 },
            { 18446744073709ull,       14 },
            { 184467440737095ull,      15 },
            { 1844674407370955ull,     16 },
            { 18446744073709551ull,    17 },
            { 184467440737095516ull,   18 },
            { 1844674407370955161ull,  19 },
            { 18446744073709551615ull, 20 }
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
