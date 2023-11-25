#include "t-index.h"

constexpr i32 leadingZeroCountTest() {
    {
        struct TestCase {
            u32 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 31 },
            { 0b10, 30 },
            { 0b100, 29 },
            { 0b1000, 28 },
            { 0b10000, 27 },
            { 0b100000, 26 },
            { 0b1000000, 25 },
            { 0b10000000, 24 },
            { 0b100000000, 23 },
            { 0b1000000000, 22 },
            { 0b10000000000, 21 },
            { 0b100000000000, 20 },
            { 0b1000000000000, 19 },
            { 0b10000000000000, 18 },
            { 0b100000000000000, 17 },
            { 0b1000000000000000, 16 },
            { 0b10000000000000000, 15 },
            { 0b100000000000000000, 14 },
            { 0b1000000000000000000, 13 },
            { 0b10000000000000000000, 12 },
            { 0b100000000000000000000, 11 },
            { 0b1000000000000000000000, 10 },
            { 0b10000000000000000000000, 9 },
            { 0b100000000000000000000000, 8 },
            { 0b1000000000000000000000000, 7 },
            { 0b10000000000000000000000000, 6 },
            { 0b100000000000000000000000000, 5 },
            { 0b1000000000000000000000000000, 4 },
            { 0b10000000000000000000000000000, 3 },
            { 0b100000000000000000000000000000, 2 },
            { 0b1000000000000000000000000000000, 1 },
            { 0b10000000000000000000000000000000, 0 },
        };

        executeTestTable("leading zero count failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_countLeadingZeros(c.in) == c.expected, cErr);
        });
    }

    {
        struct TestCase {
            i32 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 31 },
            { 0b10, 30 },
            { 0b100, 29 },
            { 0b1000, 28 },
            { 0b10000, 27 },
            { 0b100000, 26 },
            { 0b1000000, 25 },
            { 0b10000000, 24 },
            { 0b100000000, 23 },
            { 0b1000000000, 22 },
            { 0b10000000000, 21 },
            { 0b100000000000, 20 },
            { 0b1000000000000, 19 },
            { 0b10000000000000, 18 },
            { 0b100000000000000, 17 },
            { 0b1000000000000000, 16 },
            { 0b10000000000000000, 15 },
            { 0b100000000000000000, 14 },
            { 0b1000000000000000000, 13 },
            { 0b10000000000000000000, 12 },
            { 0b100000000000000000000, 11 },
            { 0b1000000000000000000000, 10 },
            { 0b10000000000000000000000, 9 },
            { 0b100000000000000000000000, 8 },
            { 0b1000000000000000000000000, 7 },
            { 0b10000000000000000000000000, 6 },
            { 0b100000000000000000000000000, 5 },
            { 0b1000000000000000000000000000, 4 },
            { 0b10000000000000000000000000000, 3 },
            { 0b100000000000000000000000000000, 2 },
            { 0b1000000000000000000000000000000, 1 },
            { -1, 0 },
        };

        executeTestTable("leading zero count failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_countLeadingZeros(c.in) == c.expected, cErr);
        });
    }

    {
        struct TestCase {
            u64 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 63 },
            { 0b10, 62 },
            { 0b100, 61 },
            { 0b1000, 60 },
            { 0b10000, 59 },
            { 0b100000, 58 },
            { 0b1000000, 57 },
            { 0b10000000, 56 },
            { 0b100000000, 55 },
            { 0b1000000000, 54 },
            { 0b10000000000, 53 },
            { 0b100000000000, 52 },
            { 0b1000000000000, 51 },
            { 0b10000000000000, 50 },
            { 0b100000000000000, 49 },
            { 0b1000000000000000, 48 },
            { 0b10000000000000000, 47 },
            { 0b100000000000000000, 46 },
            { 0b1000000000000000000, 45 },
            { 0b10000000000000000000, 44 },
            { 0b100000000000000000000, 43 },
            { 0b1000000000000000000000, 42 },
            { 0b10000000000000000000000, 41 },
            { 0b100000000000000000000000, 40 },
            { 0b1000000000000000000000000, 39 },
            { 0b10000000000000000000000000, 38 },
            { 0b100000000000000000000000000, 37 },
            { 0b1000000000000000000000000000, 36 },
            { 0b10000000000000000000000000000, 35 },
            { 0b100000000000000000000000000000, 34 },
            { 0b1000000000000000000000000000000, 33 },
            { 0b10000000000000000000000000000000, 32 },
            { 0b100000000000000000000000000000000, 31 },
            { 0b1000000000000000000000000000000000, 30 },
            { 0b10000000000000000000000000000000000, 29 },
            { 0b100000000000000000000000000000000000, 28 },
            { 0b1000000000000000000000000000000000000, 27 },
            { 0b10000000000000000000000000000000000000, 26 },
            { 0b100000000000000000000000000000000000000, 25 },
            { 0b1000000000000000000000000000000000000000, 24 },
            { 0b10000000000000000000000000000000000000000, 23 },
            { 0b100000000000000000000000000000000000000000, 22 },
            { 0b1000000000000000000000000000000000000000000, 21 },
            { 0b10000000000000000000000000000000000000000000, 20 },
            { 0b100000000000000000000000000000000000000000000, 19 },
            { 0b1000000000000000000000000000000000000000000000, 18 },
            { 0b10000000000000000000000000000000000000000000000, 17 },
            { 0b100000000000000000000000000000000000000000000000, 16 },
            { 0b1000000000000000000000000000000000000000000000000, 15 },
            { 0b10000000000000000000000000000000000000000000000000, 14 },
            { 0b100000000000000000000000000000000000000000000000000, 13 },
            { 0b1000000000000000000000000000000000000000000000000000, 12 },
            { 0b10000000000000000000000000000000000000000000000000000, 11 },
            { 0b100000000000000000000000000000000000000000000000000000, 10 },
            { 0b1000000000000000000000000000000000000000000000000000000, 9 },
            { 0b10000000000000000000000000000000000000000000000000000000, 8 },
            { 0b100000000000000000000000000000000000000000000000000000000, 7 },
            { 0b1000000000000000000000000000000000000000000000000000000000, 6 },
            { 0b10000000000000000000000000000000000000000000000000000000000, 5 },
            { 0b100000000000000000000000000000000000000000000000000000000000, 4 },
            { 0b1000000000000000000000000000000000000000000000000000000000000, 3 },
            { 0b10000000000000000000000000000000000000000000000000000000000000, 2 },
            { 0b100000000000000000000000000000000000000000000000000000000000000, 1 },
            { 0b1000000000000000000000000000000000000000000000000000000000000000, 0 },
        };

        executeTestTable("leading zero count failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_countLeadingZeros(c.in) == c.expected, cErr);
        });
    }

    {
        struct TestCase {
            i64 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 63 },
            { 0b10, 62 },
            { 0b100, 61 },
            { 0b1000, 60 },
            { 0b10000, 59 },
            { 0b100000, 58 },
            { 0b1000000, 57 },
            { 0b10000000, 56 },
            { 0b100000000, 55 },
            { 0b1000000000, 54 },
            { 0b10000000000, 53 },
            { 0b100000000000, 52 },
            { 0b1000000000000, 51 },
            { 0b10000000000000, 50 },
            { 0b100000000000000, 49 },
            { 0b1000000000000000, 48 },
            { 0b10000000000000000, 47 },
            { 0b100000000000000000, 46 },
            { 0b1000000000000000000, 45 },
            { 0b10000000000000000000, 44 },
            { 0b100000000000000000000, 43 },
            { 0b1000000000000000000000, 42 },
            { 0b10000000000000000000000, 41 },
            { 0b100000000000000000000000, 40 },
            { 0b1000000000000000000000000, 39 },
            { 0b10000000000000000000000000, 38 },
            { 0b100000000000000000000000000, 37 },
            { 0b1000000000000000000000000000, 36 },
            { 0b10000000000000000000000000000, 35 },
            { 0b100000000000000000000000000000, 34 },
            { 0b1000000000000000000000000000000, 33 },
            { 0b10000000000000000000000000000000, 32 },
            { 0b100000000000000000000000000000000, 31 },
            { 0b1000000000000000000000000000000000, 30 },
            { 0b10000000000000000000000000000000000, 29 },
            { 0b100000000000000000000000000000000000, 28 },
            { 0b1000000000000000000000000000000000000, 27 },
            { 0b10000000000000000000000000000000000000, 26 },
            { 0b100000000000000000000000000000000000000, 25 },
            { 0b1000000000000000000000000000000000000000, 24 },
            { 0b10000000000000000000000000000000000000000, 23 },
            { 0b100000000000000000000000000000000000000000, 22 },
            { 0b1000000000000000000000000000000000000000000, 21 },
            { 0b10000000000000000000000000000000000000000000, 20 },
            { 0b100000000000000000000000000000000000000000000, 19 },
            { 0b1000000000000000000000000000000000000000000000, 18 },
            { 0b10000000000000000000000000000000000000000000000, 17 },
            { 0b100000000000000000000000000000000000000000000000, 16 },
            { 0b1000000000000000000000000000000000000000000000000, 15 },
            { 0b10000000000000000000000000000000000000000000000000, 14 },
            { 0b100000000000000000000000000000000000000000000000000, 13 },
            { 0b1000000000000000000000000000000000000000000000000000, 12 },
            { 0b10000000000000000000000000000000000000000000000000000, 11 },
            { 0b100000000000000000000000000000000000000000000000000000, 10 },
            { 0b1000000000000000000000000000000000000000000000000000000, 9 },
            { 0b10000000000000000000000000000000000000000000000000000000, 8 },
            { 0b100000000000000000000000000000000000000000000000000000000, 7 },
            { 0b1000000000000000000000000000000000000000000000000000000000, 6 },
            { 0b10000000000000000000000000000000000000000000000000000000000, 5 },
            { 0b100000000000000000000000000000000000000000000000000000000000, 4 },
            { 0b1000000000000000000000000000000000000000000000000000000000000, 3 },
            { 0b10000000000000000000000000000000000000000000000000000000000000, 2 },
            { 0b100000000000000000000000000000000000000000000000000000000000000, 1 },
            { -1, 0 },
        };

        executeTestTable("leading zero count failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_countLeadingZeros(c.in) == c.expected, cErr);
        });
    }

    return 0;
}

constexpr i32 numberOfSetBitsTest() {
    {
        struct TestCase {
            u32 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 1 },
            { 0b11, 2 },
            { 0b111, 3 },
            { 0b1101, 3 },
            { 0b10101, 3 },
            { 0b101011, 4 },
            { 0b1011011, 5 },
            { 0b11011011, 6 },
            { 0b110110110, 6 },
            { 0b1101101101, 7 },
            { 0b11011011011, 8 },
            { 0b110110110110, 8 },
            { 0b1101101101101, 9 },
            { 0b11011011011011, 10 },
            { 0b111011011011011, 11 },
            { 0b1110110111011011111011011101111, 24 },
            { 0b0000000000000000000000000000001, 1 },
            { 0b1000000000000000000000000000000, 1 },
            { 0b0000000000000100000000000000000, 1 },
            { 0b1000000000000000000000000000001, 2 },
        };

        executeTestTable("number of set bits failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_numberOfSetBits(c.in) == c.expected, cErr);
        });
    }

    {
        struct TestCase {
            u64 in;
            u32 expected;
        };

        TestCase cases[] = {
            { 0b0, 0 },
            { 0b1, 1 },
            { 0b11, 2 },
            { 0b111, 3 },
            { 0b1101, 3 },
            { 0b10101, 3 },
            { 0b101011, 4 },
            { 0b1011011, 5 },
            { 0b11011011, 6 },
            { 0b110110110, 6 },
            { 0b1101101101, 7 },
            { 0b11011011011, 8 },
            { 0b110110110110, 8 },
            { 0b1101101101101, 9 },
            { 0b11011011011011, 10 },
            { 0b111011011011011, 11 },
            { 0b1110110111011011111011011101111, 24 },
            { 0b0000000000000000000000000000001, 1 },
            { 0b1000000000000000000000000000000, 1 },
            { 0b0000000000000100000000000000000, 1 },
            { 0b1000000000000000000000000000001, 2 },
            { 0b100000000000000000000000000000000000000000000000000000000000000, 1 },
            { 0b000000000000000000000000000000000000000000000000000000000000001, 1 },
            { 0b100000000000000000000000000000000000000000000000000000000000001, 2 },
            { 0b111011011101101111101101110111111101101110110111110110111011111, 49 },
        };

        executeTestTable("number of set bits failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::intrin_numberOfSetBits(c.in) == c.expected, cErr);
        });
    }

    return 0;
}

i32 getCPUTicksTest() {
    auto res = core::intrin_getCpuTicks();
    Assert(res > 0);
    return 0;
}

constexpr i32 rotlTest() {
    struct TestCase {
        u32 in; i32 r; u32 expected;
    };

    TestCase cases[] = {
        { 0b0, 0, 0b0 },
        { 0b1, 1, 0b10 },
        { 0b1, 2, 0b100 },
        { 0b10, 1, 0b100 },
        { 0b11, 1, 0b110 },
        { 0b1001, 2, 0b100100 },

        { 0b10, -1, 0b1 },
        { 0b100, -2, 0b1 },
        { 0b100100, -2, 0b1001 },

        // Rotate in a circle:
        { 0b10000000000000000000000000000000, 1, 0b00000000000000000000000000000001 },
        { 0b10000000000000000000000000000001, 1, 0b00000000000000000000000000000011 },
        { 0b10000000000000000000000000000000, 2, 0b00000000000000000000000000000010 },
    };

    executeTestTable("rotl failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
        auto got = core::intrin_rotl(c.in, c.r);
        Assert(got == c.expected, cErr);
    });

    return 0;
}

constexpr i32 rotrTest() {
    struct TestCase {
        u32 in; i32 r; u32 expected;
    };

    TestCase cases[] = {
        { 0b0, 0, 0b0 },
        { 0b10, 1, 0b1 },
        { 0b100, 2, 0b1 },
        { 0b100, 1, 0b10 },
        { 0b110, 1, 0b11 },
        { 0b100100, 2, 0b1001 },

        { 0b1, -1, 0b10 },
        { 0b1, -2, 0b100 },
        { 0b1001, -2, 0b100100 },

        // Rotate in a circle:
        { 0b00000000000000000000000000000001, 1, 0b10000000000000000000000000000000 },
        { 0b00000000000000000000000000000011, 1, 0b10000000000000000000000000000001 },
        { 0b00000000000000000000000000000010, 2, 0b10000000000000000000000000000000 },
    };

    executeTestTable("rotl failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
        auto got = core::intrin_rotr(c.in, c.r);
        Assert(got == c.expected, cErr);
    });

    return 0;
}

i32 runIntrinsicsTestsSuite() {
    RunTest(leadingZeroCountTest);
    RunTest(numberOfSetBitsTest);
    RunTest(getCPUTicksTest);
    RunTest(rotlTest);
    RunTest(rotrTest);

    return 0;
}

constexpr i32 runCompiletimeIntrinsicsTestsSuite() {
    RunTestCompileTime(leadingZeroCountTest);
    RunTestCompileTime(numberOfSetBitsTest);
    RunTestCompileTime(rotlTest);
    RunTestCompileTime(rotrTest);

    return 0;
}
