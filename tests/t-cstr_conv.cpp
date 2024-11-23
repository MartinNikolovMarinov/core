#include "t-index.h"

#include "common/ryu_reference_impl/ryu_parse.h"

// FIXME: Check error cases for all the tests. Add more tests if needed.
constexpr i32 cstrToIntTest() {
    {
        struct TestCase {
            const char* input;
            i8 expected;
            bool skipAtCompileTime = false;
        };

        constexpr TestCase cases[] = {
            { "123", 123 },
            { "-123", -123 },
            { "127", 127 },
            { "-127", -127 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i8 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cstrToInt<i8>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "254", 254 },
            { "255", 255 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u8 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToInt<u8>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "-123", -123 },
            { "32767", 32767 },
            { "-32767", -32767 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i16 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cstrToInt<i16>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "65534", 65534 },
            { "65535", 65535 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u16 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToInt<u16>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "-123", -123 },
            { "2147483647", 2147483647 },
            { "-2147483647", -2147483647 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cstrToInt<i32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "4294967294", 4294967294 },
            { "4294967295", 4294967295 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToInt<u32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "-123", -123 },
            { "9223372036854775807", 9223372036854775807ll },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return 0; }
            auto v = core::cstrToInt<i64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
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
            { "123", 123 },
            { "18446744073709551614", 18446744073709551614ull },
            { "18446744073709551615", 18446744073709551615ull },
        };

        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToInt<u64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr);
            CT_CHECK(v.value() == c.expected, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
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

constexpr i32 intToCstrTest() {
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
            auto res = core::intToCstr(c.in, buf, 12, c.digitCount);
            CT_CHECK(core::cstrLen(buf) == res, cErr);
            CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            auto res = core::intToCstr(c.in, buf, 21, c.digitCount);
            CT_CHECK(core::cstrLen(buf) == res, cErr);
            CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            auto res = core::intToCstr(c.in, buf, 11, c.digitCount);
            CT_CHECK(core::cstrLen(buf) == res, cErr);
            CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            auto res = core::intToCstr(c.in, buf, 20, c.digitCount);
            CT_CHECK(core::cstrLen(buf) == res, cErr);
            CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            { core::limitMin<i8>(), "80" },
            { i8(0), "00" },
            { i8(0xF), "0F" },
            { i8(-1), "FF" },
            { core::limitMax<i8>(), "7F" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i16 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::limitMin<i16>(), "8000" },
            { i16(0), "0000" },
            { i16(0xF), "000F" },
            { i16(0x1D49), "1D49" },
            { i16(0x0F0F), "0F0F" },
            { i16(-1), "FFFF" },
            { i16(-2), "FFFE" },
            { core::limitMax<i16>(), "7FFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i32 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::limitMin<i32>(), "80000000" },
            { i32(0), "00000000" },
            { i32(0xF), "0000000F" },
            { i32(0x1D49), "00001D49" },
            { i32(0x0F0F), "00000F0F" },
            { i32(0x12345678), "12345678" },
            { i32(-1), "FFFFFFFF" },
            { i32(-2), "FFFFFFFE" },
            { core::limitMax<i32>(), "7FFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { i64 in; const char* expected; };
        constexpr TestCase cases[] = {
            { core::limitMin<i64>(), "8000000000000000" },
            { i64(0), "0000000000000000" },
            { i64(0xF), "000000000000000F" },
            { i64(0x1D49), "0000000000001D49" },
            { i64(0x0F0F), "0000000000000F0F" },
            { i64(0x12345678), "0000000012345678" },
            { i64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::limitMax<i64>(), "7FFFFFFFFFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase { u8 in; const char* expected; };
        constexpr TestCase cases[] = {
            { u8(0), "00" },
            { u8(0xF), "0F" },
            { core::limitMax<u8>(), "FF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            { core::limitMax<u16>(), "FFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            { core::limitMax<u32>(), "FFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
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
            { core::limitMax<u64>(), "FFFFFFFFFFFFFFFF" },
        };
        i32 ret = core::testing::executeTestTable("test case failed for u64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::intToHex(c.in, buf);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 cstrToFloatTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Basic
            {"0", 0.0f, ParseError::None},
            {"0.0", 0.0f, ParseError::None},
            {"0.0000", 0.0f, ParseError::None},
            {"-0", -0.0f, ParseError::None},
            {"-0.0", -0.0f, ParseError::None},

            { "1", 1.0f, ParseError::None },
            { "-1", -1.0f, ParseError::None },
            {"2.0", 2.0f, ParseError::None},
            {"4.0", 4.0f, ParseError::None},
            { "123456789", 123456792.0f, ParseError::None },
            { "299792458", 299792448.0f, ParseError::None },
            { ".123", 0.123f, ParseError::None },
            { "123", 123.f, ParseError::None },
            { "00000000000000000000000000000", 0.f, ParseError::None },
            { "0.000000000000000000000000000", 0.f, ParseError::None },

            // Sign variations
            { "+1.0", 1.0f, ParseError::None },
            { "-123.456", -123.456f, ParseError::None },
            { "+0.0", 0.0f, ParseError::None },
            { "--1.0", 0.0f, ParseError::InputHasInvalidSymbol },

            // Non-Representable Decimal Fractions
            { "0.1", 0.100000001490116119384765625f, ParseError::None },
            { "0.2", 0.20000000298023223876953125f, ParseError::None },
            { "0.3", 0.300000011920928955078125f, ParseError::None },
            { "0.7", 0.699999988079071044921875f, ParseError::None },

            // Trailing zeroes
            { "26843549.5", 26843550.0f, ParseError::None },
            { "50000002.5", 50000004.0f, ParseError::None },
            { "99999989.5", 99999992.0f, ParseError::None },

            // Number exactly halfway between two representable floats
            { "1.00000011", 1.0000001f, ParseError::None }, // Between 1.0000001 and next float
            // Number requiring rounding up
            { "1.00000006", 1.0000001f, ParseError::None },
            // Number requiring rounding down
            { "1.00000004", 1.0f, ParseError::None },

            // Error cases
            { "", 0, ParseError::InputEmpty },
            { nullptr, 0, ParseError::InputEmpty },
            { ".12.3", 0, ParseError::InputHasMultipleDots },

            { "a123", 0, ParseError::InputHasInvalidSymbol },
            { "0.a123", 0, ParseError::InputHasInvalidSymbol },
            { ".123a", 0, ParseError::InputHasInvalidSymbol },
            { "123a", 0, ParseError::InputHasInvalidSymbol },
            { "12.3a", 0, ParseError::InputHasInvalidSymbol },

            { "0123456789", 123456789.f, ParseError::None },
            { "1234567890", 0.f, ParseError::InputNumberTooLarge },
            { "123456789.0", 0.f, ParseError::InputNumberTooLarge },
            { "12345678.00", 0.f, ParseError::InputNumberTooLarge },
            { "1234567.000", 0.f, ParseError::InputNumberTooLarge },

            { "9999999999", 0.f, ParseError::InputNumberTooLarge },
            { "-1234567890", 0.f, ParseError::InputNumberTooLarge },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ParseError::None) {
                CT_CHECK(v.hasValue(), cErr, true);
                CT_CHECK(v.value() == c.expected, cErr, true);

                CT_CHECK(!core::isnan(v.value()), cErr, true);
                CT_CHECK(!core::isinf(v.value()), cErr, true);
            }
            else {
                CT_CHECK(v.hasErr(), cErr, true);
                CT_CHECK(v.err() == c.err, cErr, true);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Basic
            {"0", 0.0, ParseError::None},
            {"0.0", 0.0, ParseError::None},
            {"0.0000", 0.0, ParseError::None},
            {"-0", -0.0, ParseError::None},
            {"-0.0", -0.0, ParseError::None},

            { "1", 1.0, ParseError::None },
            { "-1", -1.0, ParseError::None },
            // {"2.0", 2.0, ParseError::None},
            // {"4.0", 4.0, ParseError::None},
            // { "123456789", 123456792.0, ParseError::None }, // FIXME: bug in this case?
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ParseError::None) {
                CT_CHECK(v.hasValue(), cErr, true);
                CT_CHECK(v.value() == c.expected, cErr, true);

                CT_CHECK(!core::isnan(v.value()), cErr, true);
                CT_CHECK(!core::isinf(v.value()), cErr, true);
            }
            else {
                CT_CHECK(v.hasErr(), cErr, true);
                CT_CHECK(v.err() == c.err, cErr, true);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
};

constexpr i32 cstrToFloatNanAndInfTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f32>(), ParseError::None },
            { "-inf", -core::infinity<f32>(), ParseError::None },
            { "INF", core::infinity<f32>(), ParseError::None },
            { "-INF", -core::infinity<f32>(), ParseError::None },

            // None cases:
            { "nan", core::signalingNaN<f32>(), ParseError::None },
            { "NaN", core::signalingNaN<f32>(), ParseError::None },
            { "NAN", core::signalingNaN<f32>(), ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr, true);

            if (core::isinf(c.expected)) {
                CT_CHECK(v.value() == c.expected, cErr, true);
                CT_CHECK(core::isinf(v.value()), cErr, true);
            }
            else if (core::isnan(c.expected)) {
                CT_CHECK(core::isnan(v.value()), cErr, true);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f64>(), ParseError::None },
            { "-inf", -core::infinity<f64>(), ParseError::None },
            { "INF", core::infinity<f64>(), ParseError::None },
            { "-INF", -core::infinity<f64>(), ParseError::None },

            // None cases:
            { "nan", core::signalingNaN<f64>(), ParseError::None },
            { "NaN", core::signalingNaN<f64>(), ParseError::None },
            { "NAN", core::signalingNaN<f64>(), ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(v.hasValue(), cErr, true);

            if (core::isinf(c.expected)) {
                CT_CHECK(v.value() == c.expected, cErr, true);
                CT_CHECK(core::isinf(v.value()), cErr, true);
            }
            else if (core::isnan(c.expected)) {
                CT_CHECK(core::isnan(v.value()), cErr, true);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 cstrToFloatComparedToOriginalTest() {
    return 0;
}

i32 runCstrConvTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(cstrToIntTest);
    if (runTest(tInfo, cstrToIntTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(digitToCharTest);
    if (runTest(tInfo, digitToCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToCstrTest);
    if (runTest(tInfo, intToCstrTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(intHexTest);
    if (runTest(tInfo, intHexTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrToFloatTest);
    if (runTest(tInfo, cstrToFloatTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrToFloatNanAndInfTest);
    if (runTest(tInfo, cstrToFloatNanAndInfTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrToFloatComparedToOriginalTest);
    if (runTest(tInfo, cstrToFloatComparedToOriginalTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCstrConvTestsSuite() {
    RunTestCompileTime(cstrToIntTest);
    RunTestCompileTime(digitToCharTest);
    RunTestCompileTime(intToCstrTest);
    RunTestCompileTime(intHexTest);
    // RunTestCompileTime(cstrToFloatTest);
    // RunTestCompileTime(cstrToFloatNanAndInfTest);

    return 0;
}
