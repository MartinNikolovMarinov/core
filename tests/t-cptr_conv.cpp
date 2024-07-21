#include "t-index.h"

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
        CT_CHECK(core::charToInt<i8>(c.in) == i8(c.expected), cErr);
        CT_CHECK(core::charToInt<i16>(c.in) == i16(c.expected), cErr);
        CT_CHECK(core::charToInt<i32>(c.in) == i32(c.expected), cErr);
        CT_CHECK(core::charToInt<i64>(c.in) == i64(c.expected), cErr);
        CT_CHECK(core::charToInt<u8>(c.in) == u8(c.expected), cErr);
        CT_CHECK(core::charToInt<u16>(c.in) == u16(c.expected), cErr);
        CT_CHECK(core::charToInt<u32>(c.in) == u32(c.expected), cErr);
        CT_CHECK(core::charToInt<u64>(c.in) == u64(c.expected), cErr);
        CT_CHECK(core::charToInt<f32>(c.in) == f32(c.expected), cErr);
        CT_CHECK(core::charToInt<f64>(c.in) == f64(c.expected), cErr);
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
};

constexpr i32 intToCptrTest() {
    {
        struct TestCase {
            i32 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            { 0, 1, "0" },
            { 1, 1, "1" },
            { -1, 1, "-1" },
            { 123, 3, "123" },
            { -123, 3, "-123" },
            { 123456789, 9, "123456789" },
            { -123456789, 9, "-123456789" },
            { 2147483647, 10, "2147483647" },
            { -2147483647, 10, "-2147483647" },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[12] = {};
            auto res = core::intToCptr(c.in, buf, 12, c.digitCount);
            CT_CHECK(core::cptrLen(buf) == res, cErr);
            CT_CHECK(core::cptrLen(buf) == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }
    {
        struct TestCase {
            i64 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            { 0, 1, "0" },
            { 1, 1, "1" },
            { -1, 1, "-1" },
            { 123, 3, "123" },
            { -123, 3, "-123" },
            { 123456789, 9, "123456789" },
            { -123456789, 9, "-123456789" },
            { 2147483647, 10, "2147483647" },
            { -2147483647, 10, "-2147483647" },
            { 2147483648, 10, "2147483648" },
            { -2147483648, 10, "-2147483648" },
            { 9223372036854775807, 19, "9223372036854775807" },
            { -9223372036854775807, 19, "-9223372036854775807" },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[21] = {};
            auto res = core::intToCptr(c.in, buf, 21, c.digitCount);
            CT_CHECK(core::cptrLen(buf) == res, cErr);
            CT_CHECK(core::cptrLen(buf) == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            u32 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            { 0, 1, "0" },
            { 1, 1, "1" },
            { 123, 3, "123" },
            { 123456789, 9, "123456789" },
            { 2147483647, 10, "2147483647" },
            { 2147483648, 10, "2147483648" },
            { 4294967295, 10, "4294967295" },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[11] = {};
            auto res = core::intToCptr(c.in, buf, 11, c.digitCount);
            CT_CHECK(core::cptrLen(buf) == res, cErr);
            CT_CHECK(core::cptrLen(buf) == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            u64 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            { 0, 1, "0" },
            { 1, 1, "1" },
            { 123, 3, "123" },
            { 123456789, 9, "123456789" },
            { 2147483647, 10, "2147483647" },
            { 2147483648, 10, "2147483648" },
            { 4294967295, 10, "4294967295" },
            { 4294967296, 10, "4294967296" },
            { 9223372036854775807, 19, "9223372036854775807" },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[20] = {};
            auto res = core::intToCptr(c.in, buf, 20, c.digitCount);
            CT_CHECK(core::cptrLen(buf) == res, cErr);
            CT_CHECK(core::cptrLen(buf) == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 cptrToIntTest() {
    {
        struct TestCase {
            const char* input;
            i8 expected;
            bool skipAtCompileTime = false;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "-.123", 0 },
            { "123", 123 },
            { "-123", -123 },
            { "123asd", 123 },
            { "-123  ", -123 },
            { "    -123  ", -123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "127", 127 },
            { "-127", -127 },
            { "128", 127 },

            { "-128", -128, true }, // valid in runtime but fails in compiletime execution.
        };

        i32 ret = core::testing::executeTestTable("test case failed for i8 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cptrToInt<i8>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            u8 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "123", 123 },
            { "123asd", 123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "254", 254 },
            { "255", 255 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u8 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToInt<u8>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            i16 expected;
            bool skipAtCompileTime = false;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "-.123", 0 },
            { "123", 123 },
            { "-123", -123 },
            { "123asd", 123 },
            { "-123  ", -123 },
            { "    -123  ", -123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "32767", 32767 },
            { "-32767", -32767 },
            { "32768", 32767 },

            { "-32768", -32768, true },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i16 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cptrToInt<i16>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            u16 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "123", 123 },
            { "123asd", 123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "65534", 65534 },
            { "65535", 65535 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u16 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToInt<u16>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            i32 expected;
            bool skipAtCompileTime = false;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "-.123", 0 },
            { "123", 123 },
            { "-123", -123 },
            { "123asd", 123 },
            { "-123  ", -123 },
            { "    -123  ", -123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "2147483647", 2147483647 },
            { "-2147483647", -2147483647 },

            { "-2147483648", -2147483648, true },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cptrToInt<i32>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            u32 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "123", 123 },
            { "123asd", 123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "4294967294", 4294967294 },
            { "4294967295", 4294967295 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToInt<u32>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            i64 expected;
            bool skipAtCompileTime = false;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "-.123", 0 },
            { "123", 123 },
            { "-123", -123 },
            { "123asd", 123 },
            { "-123  ", -123 },
            { "    -123  ", -123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "9223372036854775807", 9223372036854775807ll },

            { "-9223372036854775807", -9223372036854775807ll, true },
            { "-9223372036854775808", core::MIN_I64, true },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cptrToInt<i64>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            u64 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0 },
            { nullptr, 0 },
            { "a123", 0 },
            { ".123", 0 },
            { "123", 123 },
            { "123asd", 123 },
            { "   \t123  ", 123 },
            { "   \n123  ", 123 },
            { "   \n\r123  ", 123 },
            { "18446744073709551614", 18446744073709551614ull },
            { "18446744073709551615", 18446744073709551615ull },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToInt<u64>(c.input);
            CT_CHECK(v == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 cptrToFloatTest() {

    {
        struct TestCase {
            const char* input;
            f32 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0.f },
            { nullptr, 0.f },
            { "aasdb", 0.f },
            { "aasdb1", 0.f },
            { "a1", 0.f },
            { ".1", .1f },
            { "-.1", -.1f },
            { "0.1", 0.1f },
            { ".00001", .00001f },
            { "-0.00001", -0.00001f },
            { "0.123", 0.123f },
            { "-0.123", -0.123f },
            { "123.789", 123.789f },
            { "-123.789", -123.789f },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToFloat<f32>(c.input);
            CT_CHECK(core::safeEq(v, c.expected, 0.000001f), cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
        };

        constexpr TestCase cases[] = {
            { "", 0. },
            { nullptr, 0. },
            { "aasdb", 0. },
            { "aasdb1", 0. },
            { "a1", 0. },
            { ".1", .1 },
            { "-.1", -.1 },
            { "0.1", 0.1 },
            { ".00001", .00001 },
            { "-0.00001", -0.00001 },
            { "0.123", 0.123 },
            { "-0.123", -0.123 },
            { "123.789", 123.789 },
            { "-123.789", -123.789 },
            { "123789.10111213", 123789.10111213 },
            { "-123789.10111213", -123789.10111213 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptrToFloat<f64>(c.input);
            CT_CHECK(core::safeEq(v, c.expected, 0.00000001), cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 intHexTest() {
    {
        struct TestCase { i8 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::MIN_I8, "80" },
            { i8(0), "00" },
            { i8(0xF), "0F" },
            { i8(-1), "FF" },
            { core::MAX_I8, "7F" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i16 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::MIN_I16, "8000" },
            { i16(0), "0000" },
            { i16(0xF), "000F" },
            { i16(0x1D49), "1D49" },
            { i16(0x0F0F), "0F0F" },
            { i16(-1), "FFFF" },
            { i16(-2), "FFFE" },
            { core::MAX_I16, "7FFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i32 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::MIN_I32, "80000000" },
            { i32(0), "00000000" },
            { i32(0xF), "0000000F" },
            { i32(0x1D49), "00001D49" },
            { i32(0x0F0F), "00000F0F" },
            { i32(0x12345678), "12345678" },
            { i32(-1), "FFFFFFFF" },
            { i32(-2), "FFFFFFFE" },
            { core::MAX_I32, "7FFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i64 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::MIN_I64, "8000000000000000" },
            { i64(0), "0000000000000000" },
            { i64(0xF), "000000000000000F" },
            { i64(0x1D49), "0000000000001D49" },
            { i64(0x0F0F), "0000000000000F0F" },
            { i64(0x12345678), "0000000012345678" },
            { i64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::MAX_I64, "7FFFFFFFFFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { u8 in; const char* expected; };
        constexpr TestCase cases[] = {
            { u8(0), "00" },
            { u8(0xF), "0F" },
            { core::MAX_U8, "FF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { u16 in; const char* expected; };
        constexpr TestCase cases[] = {
            { u16(0), "0000" },
            { u16(0xF), "000F" },
            { u16(0x1D49), "1D49" },
            { u16(0x0F0F), "0F0F" },
            { core::MAX_U16, "FFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { u32 in; const char* expected; };
        constexpr TestCase cases[] = {
            { u32(0), "00000000" },
            { u32(0xF), "0000000F" },
            { u32(0x1D49), "00001D49" },
            { u32(0x0F0F), "00000F0F" },
            { u32(0x12345678), "12345678" },
            { core::MAX_U32, "FFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { u64 in; const char* expected; };
        constexpr TestCase cases[] = {
            { u64(0), "0000000000000000" },
            { u64(0xF), "000000000000000F" },
            { u64(0x1D49), "0000000000001D49" },
            { u64(0x0F0F), "0000000000000F0F" },
            { u64(0x12345678), "0000000012345678" },
            { u64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::MAX_U64, "FFFFFFFFFFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::cptrCmp(buf, core::cptrLen(buf), c.expected, core::cptrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 floatToCptrTest() {
    {
        struct TestCase { f32 in; const char* expected; u32 precision; };
        constexpr TestCase cases[] = {
            { 0.f, "0", 0 },
            { 0.f, "0.0", 1 },
            { 0.f, "0.00", 2 },
            { 0.f, "0.000", 3 },
            { 0.f, "0.0000", 4 },
            { 0.f, "0.00000", 5 },
            { 0.f, "0.000000", 6 },
            { 0.f, "0.0000000", 7 },

            { 0.11111111f, "0", 0 },
            { 0.11111111f, "0.1", 1 },
            { 0.11111111f, "0.11", 2 },
            { 0.11111111f, "0.111", 3 },
            { 0.11111111f, "0.1111", 4 },
            { 0.11111111f, "0.11111", 5 },
            { 0.11111111f, "0.111111", 6 },
            { 0.11111111f, "0.1111111", 7 },

            // { 123.567f, "124", 0 }, // FIXME: Round the whole part !
            { 123.567f, "123.6", 1 },
            { 123.567f, "123.57", 2 },
            { 123.567f, "123.567", 3 },
            { 123.567f, "123.5670", 4 },
            { 123.567f, "123.56700", 5 },
            { 123.567f, "123.567001", 6 },
            { 123.567f, "123.5670014", 7 },

            // { 892.0055501f, "892", 0 }, // FIXME: Round the whole part !
            { 0.00102f, "0.0", 1 },
            { 0.00102f, "0.00", 2 },
            { 0.00102f, "0.001", 3 },
            { 0.00102f, "0.0010", 4 },
            { 0.00102f, "0.00102", 5 },
            { 0.00102f, "0.001020", 6 },
            { 0.00102f, "0.0010200", 7 },
        };
        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};

            u32 res = core::floatToCptr(c.in, buf, 20, c.precision);
            addr_size bufLen = core::cptrLen(buf);

            CT_CHECK(bufLen == addr_size(res), cErr);
            CT_CHECK(bufLen == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, bufLen, c.expected, core::cptrLen(c.expected)) == 0, cErr);

            // NOTE: snprintf promotes the input to f64 so it can't be used to verify the result here.

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { f64 in; const char* expected; u32 precision; };
        constexpr TestCase cases[] = {
            { 0., "0", 0 },
            { 0., "0.0", 1 },
            { 0., "0.00", 2 },
            { 0., "0.000", 3 },
            { 0., "0.0000", 4 },
            { 0., "0.00000", 5 },
            { 0., "0.000000", 6 },
            { 0., "0.0000000", 7 },
            { 0., "0.00000000", 8 },
            { 0., "0.000000000", 9 },
            { 0., "0.0000000000", 10 },
            { 0., "0.00000000000", 11 },
            { 0., "0.000000000000", 12 },
            { 0., "0.0000000000000", 13 },
            { 0., "0.00000000000000", 14 },
            { 0., "0.000000000000000", 15 },

            { 0.1111111111111111, "0", 0 },
            { 0.1111111111111111, "0.1", 1 },
            { 0.1111111111111111, "0.11", 2 },
            { 0.1111111111111111, "0.111", 3 },
            { 0.1111111111111111, "0.1111", 4 },
            { 0.1111111111111111, "0.11111", 5 },
            { 0.1111111111111111, "0.111111", 6 },
            { 0.1111111111111111, "0.1111111", 7 },
            { 0.1111111111111111, "0.11111111", 8 },
            { 0.1111111111111111, "0.111111111", 9 },
            { 0.1111111111111111, "0.1111111111", 10 },
            { 0.1111111111111111, "0.11111111111", 11 },
            { 0.1111111111111111, "0.111111111111", 12 },
            { 0.1111111111111111, "0.1111111111111", 13 },
            { 0.1111111111111111, "0.11111111111111", 14 },
            { 0.1111111111111111, "0.111111111111111", 15 },

            // { 123.567f, "124", 0 }, // FIXME: Round the whole part !
            { 123.567, "123.6", 1 },
            { 123.567, "123.57", 2 },
            { 123.567, "123.567", 3 },
            { 123.567, "123.5670", 4 },
            { 123.567, "123.56700", 5 },
            { 123.567, "123.567000", 6 },
            { 123.567, "123.5670000", 7 },
            { 123.567, "123.56700000", 8 },
            { 123.567, "123.567000000", 9 },
            { 123.567, "123.5670000000", 10 },
            { 123.567, "123.56700000000", 11 },
            { 123.567, "123.567000000000", 12 },
            { 123.567, "123.5670000000000", 13 },
            { 123.567, "123.56699999999999", 14 },
            { 123.567, "123.566999999999993", 15 },

            { 0.00102, "0.0", 1 },
            { 0.00102, "0.00", 2 },
            { 0.00102, "0.001", 3 },
            { 0.00102, "0.0010", 4 },
            { 0.00102, "0.00102", 5 },
            { 0.00102, "0.001020", 6 },
            { 0.00102, "0.0010200", 7 },
            { 0.00102, "0.00102000", 8 },
            { 0.00102, "0.001020000", 9 },
            { 0.00102, "0.0010200000", 10 },
            { 0.00102, "0.00102000000", 11 },
            { 0.00102, "0.001020000000", 12 },
            { 0.00102, "0.0010200000000", 13 },
            { 0.00102, "0.00102000000000", 14 },
            { 0.00102, "0.001020000000000", 15 },
        };
        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};

            u32 res = core::floatToCptr(c.in, buf, 20, c.precision);
            addr_size bufLen = core::cptrLen(buf);

            CT_CHECK(bufLen == addr_size(res), cErr);
            CT_CHECK(bufLen == core::cptrLen(c.expected));
            CT_CHECK(core::cptrCmp(buf, bufLen, c.expected, core::cptrLen(c.expected)) == 0, cErr);

            IS_NOT_CONST_EVALUATED {
                // Verify result to snprintf
                char expectedBuf[20] = {};
                i32 expectedRes = snprintf(expectedBuf, 20, "%.*f", c.precision, c.in);
                CT_CHECK(bufLen == addr_size(res), cErr);
                CT_CHECK(bufLen == addr_size(expectedRes), cErr);
                CT_CHECK(core::cptrCmp(buf, addr_size(bufLen), expectedBuf, addr_size(expectedRes)) == 0, cErr);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    // FIXME: test nan and infinity
    // FIXME: more tests are needed

    return 0;
}

i32 runCptrConvTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(charToIntTest);
    if (runTest(tInfo, charToIntTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(digitToCharTest);
    if (runTest(tInfo, digitToCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToCptrTest);
    if (runTest(tInfo, intToCptrTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(intHexTest);
    if (runTest(tInfo, intHexTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(floatToCptrTest);
    if (runTest(tInfo, floatToCptrTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrToIntTest);
    if (runTest(tInfo, cptrToIntTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cptrToFloatTest);
    if (runTest(tInfo, cptrToFloatTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCptrConvTestsSuite() {
    RunTestCompileTime(charToIntTest);
    RunTestCompileTime(digitToCharTest);
    RunTestCompileTime(intToCptrTest);
    RunTestCompileTime(intHexTest);
    // RunTestCompileTime(floatToCptrTest);
    RunTestCompileTime(cptrToIntTest);
    RunTestCompileTime(cptrToFloatTest);

    return 0;
}
