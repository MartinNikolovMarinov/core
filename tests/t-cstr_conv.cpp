#include "t-index.h"

constexpr i32 cstrToIntTest() {
    {
        struct TestCase {
            const char* input;
            i8 expected;
        };

        constexpr TestCase cases[] = {
            { "+123", 123 },
            { "-123", -123 },
            { "127", 127 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i8 at index: ", cases, [](auto& c, const char* cErr) {
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
            { "+254", 254 },
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
        };

        constexpr TestCase cases[] = {
            { "+123", 123 },
            { "-123", -123 },
            { "32767", 32767 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i16 at index: ", cases, [](auto& c, const char* cErr) {
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
            { "+65535", 65535 },
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
        };

        constexpr TestCase cases[] = {
            { "123", 123 },
            { "-123", -123 },
            { "+2147483647", 2147483647 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
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
            { "+4294967294", 4294967294 },
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
        };

        constexpr TestCase cases[] = {
            { "123", 123 },
            { "-123", -123 },
            { "+9223372036854775807", 9223372036854775807ll },
        };

        i32 ret = core::testing::executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
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
            { "+18446744073709551614", 18446744073709551614ull },
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

    IS_NOT_CONST_EVALUATED {
        // NOTE: These work, but not in a const evaluated context, because the multiplication results in overflow.

        {
            auto v = core::cstrToInt<i8>("-128", u32(core::cstrLen("-128")));
            CT_CHECK(v.hasValue());
            CT_CHECK(v.value() == -128);
        }
        {
            auto v = core::cstrToInt<i16>("-32768", u32(core::cstrLen("-32768")));
            CT_CHECK(v.hasValue());
            CT_CHECK(v.value() == -32768);
        }
        {
            auto v = core::cstrToInt<i32>("-2147483648", u32(core::cstrLen("-2147483648")));
            CT_CHECK(v.hasValue());
            CT_CHECK(v.value() == -2147483648);
        }
        {
            auto v = core::cstrToInt<i64>("-9223372036854775808", u32(core::cstrLen("-9223372036854775808")));
            CT_CHECK(v.hasValue());
            CT_CHECK(v.value() == -9223372036854775807ll - 1ll);
        }
    }

    return 0;
}

constexpr i32 cstrToIntErrorsTest() {
    using ConversionError = core::ConversionError;

    {
        auto res = core::cstrToInt<u64>(nullptr, 100);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        auto res = core::cstrToInt<u64>("1234", 0);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        auto res = core::cstrToInt<u64>("1%23", 5);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputHasInvalidSymbol);
    }
    {
        auto res = core::cstrToInt<u64>("123j", core::cstrLen("123j"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputHasInvalidSymbol);
    }
    {
        auto res = core::cstrToInt<u64>("++123", core::cstrLen("++123"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputHasInvalidSymbol);
    }

    {
        auto res = core::cstrToInt<i8>("128", core::cstrLen("128"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<u8>("256", core::cstrLen("256"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<i16>("32768", core::cstrLen("32768"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<u16>("65536", core::cstrLen("65536"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<i32>("2147483648", core::cstrLen("2147483648"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<u32>("4294967297", core::cstrLen("4294967297"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<i32>("9223372036854775808", core::cstrLen("9223372036854775808"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }
    {
        auto res = core::cstrToInt<i32>("18446744073709551616", core::cstrLen("18446744073709551616"));
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputNumberTooLarge);
    }

    return 0;
}

constexpr i32 intToCstrTest() {
    auto runTestCases = [](auto& cases) -> i32 {
        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            {
                char buf[32] = {};
                auto res = core::intToCstr(c.in, buf, 32, c.digitCount);
                CT_CHECK(res.hasValue(), cErr);
                CT_CHECK(core::cstrLen(buf) == res.value(), cErr);
                CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
                CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            }

            // All tests should work when digit count is not provided.
            {
                char buf[32] = {};
                auto res = core::intToCstr(c.in, buf, 32);
                CT_CHECK(res.hasValue(), cErr);
                CT_CHECK(core::cstrLen(buf) == res.value(), cErr);
                CT_CHECK(core::cstrLen(buf) == core::cstrLen(c.expected));
                CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            }

            return 0;
        });

        CT_CHECK(ret == 0);
        return 0;
    };

    {
        struct TestCase {
            i8 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { i8(0), 1, "0" },
            { i8(1), 1, "1" },
            { i8(-1), 1, "-1" },
            { i8(127), 3, "127" },
            { i8(-128), 3, "-128" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            u8 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { u8(0), 1, "0" },
            { u8(1), 1, "1" },
            { u8(255), 3, "255" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            i16 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { i16(0), 1, "0" },
            { i16(1), 1, "1" },
            { i16(-1), 1, "-1" },
            { i16(5123), 4, "5123" },
            { i16(-5123), 4, "-5123" },
            { i16(32767), 5, "32767" },
            { i16(-32768), 5, "-32768" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            u16 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { u16(0), 1, "0" },
            { u16(1), 1, "1" },
            { u16(5123), 4, "5123" },
            { u16(65535), 5, "65535" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            i32 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { i32(0), 1, "0" },
            { i32(1), 1, "1" },
            { i32(-1), 1, "-1" },
            { i32(5123), 4, "5123" },
            { i32(-5123), 4, "-5123" },
            { i32(32767), 5, "32767" },
            { i32(-32768), 5, "-32768" },
            { i32(2147483647), 10, "2147483647" },
            { i32(-2147483648), 10, "-2147483648" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            u32 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { u32(0), 1, "0" },
            { u32(1), 1, "1" },
            { u32(5123), 4, "5123" },
            { u32(65535), 5, "65535" },
            { u32(4294967295), 10, "4294967295" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            i64 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { i64(0ll), 1, "0" },
            { i64(1ll), 1, "1" },
            { i64(-1ll), 1, "-1" },
            { i64(5123ll), 4, "5123" },
            { i64(-5123ll), 4, "-5123" },
            { i64(32767ll), 5, "32767" },
            { i64(-32768ll), 5, "-32768" },
            { i64(2147483647ll), 10, "2147483647" },
            { i64(-2147483648ll), 10, "-2147483648" },
            { i64(9223372036854775807ll), 19, "9223372036854775807" },
            { i64(-9223372036854775807ll) - 1ll, 19, "-9223372036854775808" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase {
            u64 in;
            u32 digitCount;
            const char* expected;
        };
        constexpr TestCase cases[] = {
            { u64(0ull), 1, "0" },
            { u64(1ull), 1, "1" },
            { u64(5123ull), 4, "5123" },
            { u64(65535ull), 5, "65535" },
            { u64(4294967295ull), 10, "4294967295" },
            { u64(18446744073709551615ull), 20, "18446744073709551615" },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    return 0;
}

constexpr i32 intToCstrErrorsTest() {
    using ConversionError = core::ConversionError;

    {
        auto res = core::intToCstr(123, nullptr, 100, 5);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[5];
        auto res = core::intToCstr(123, buff, 0, 5);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[1];
        auto res = core::intToCstr(-1, buff, 1);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::OutputBufferTooSmall);
    }
    {
        // Smallest usable buffer for the function:
        char buff[3] = { 9, 9, 9 };
        auto res = core::intToCstr(-1, buff, 3);
        CT_CHECK(res.hasValue());
        buff[res.value()] = '\0';
        CT_CHECK(buff[0] == '-'); CT_CHECK(buff[1] == '1'); CT_CHECK(buff[2] == '\0');
    }

    return 0;
}

constexpr i32 intToHexTest() {
    auto runTestCases = [](auto& cases) -> i32 {
        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[20] = {};
            auto res = core::intToHex(c.in, buf, 20, c.upperCase, c.hexLen);
            CT_CHECK(res.hasValue(), cErr);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });

        CT_CHECK(ret == 0);
        return 0;
    };

    {
        struct TestCase { i8 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i8>(), "80", true, 2 },
            { i8(0), "00", true, 2 },
            { i8(0), "0", true, 1 },
            { i8(0), "", true, 0 },
            { i8(-13), "f3", false, 2 },
            { i8(0xF), "0F", true, 2 },
            { i8(0xF), "0f", false, 2 },
            { i8(0xF), "F", true, 1 },
            { i8(0xF), "f", false, 1 },
            { i8(-1), "FF", true, 2 },
            { core::limitMax<i8>(), "7F", true, 2 },
            { core::limitMax<i8>(), "7f", false, 2 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u8 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { u8(0), "00", true, 2 },
            { u8(0xF), "0F", true, 2 },
            { u8(0xF), "0f", false, 2 },
            { u8(0xF), "F", true, 1 },
            { u8(0xF), "f", false, 1 },
            { u8(0xF), "", true, 0 },
            { core::limitMax<u8>(), "FF", true, 2 },
            { core::limitMax<u8>(), "ff", false, 2 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i16 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i16>(), "8000", true, 4 },
            { i16(0), "0000", true, 4 },
            { i16(0), "000", true, 3 },
            { i16(0), "00", true, 2 },
            { i16(0), "0", true, 1 },
            { i16(0), "", true, 0 },
            { i16(0xF), "000F", true, 4 },
            { i16(0xF), "00f", false, 3 },
            { i16(0xF), "0f", false, 2 },
            { i16(0xF), "F", true, 1 },
            { i16(0x1D49), "1D49", true, 4 },
            { i16(0x1D49), "1d49", false, 4 },
            { i16(0x1D49), "D49", true, 3 },
            { i16(0x0F0F), "0F0F", true, 4 },
            { i16(-1), "FFFF", true, 4 },
            { i16(-2), "FFFE", true, 4 },
            { i16(-2), "fffe", false, 4 },
            { core::limitMax<i16>(), "7FFF", true, 4 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u16 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { u16(0), "0000", true, 4 },
            { u16(0xF), "000F", true, 4 },
            { u16(0xF), "00F", true, 3 },
            { u16(0xF), "0f", false, 2 },
            { u16(0xF), "f", false, 1 },
            { u16(0xF), "", false, 0 },
            { u16(0x1D49), "1D49", true, 4 },
            { u16(0x1D49), "1d49", false, 4 },
            { u16(0x1D49), "D49", true, 3 },
            { u16(0x0F0F), "0F0F", true, 4 },
            { u16(0x0F0F), "0f0f", false, 4 },
            { core::limitMax<u16>(), "FFFF", true, 4 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i32 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i32>(), "80000000", true, 8 },
            { i32(0), "00000000", true, 8 },

            { i32(0xF), "", true, 0 },
            { i32(0xF), "F", true, 1 },
            { i32(0xF), "0F", true, 2 },
            { i32(0xF), "00f", false, 3 },
            { i32(0xF), "000F", true, 4 },
            { i32(0xF), "0000f", false, 5 },
            { i32(0xF), "00000F", true, 6 },
            { i32(0xF), "000000f", false, 7 },
            { i32(0xF), "0000000f", false, 8 },

            { i32(0x1D49), "00001D49", true, 8 },
            { i32(0x1D49), "00001d49", false, 8 },
            { i32(0x1D49), "1d49", false, 4 },
            { i32(0x0F0F), "00000F0F", true, 8 },
            { i32(0x12345678), "12345678", true, 8 },
            { i32(0x12345678), "2345678", true, 7 },
            { i32(0x12345678), "345678", true, 6 },

            { i32(-1), "FFFFFFFF", true, 8 },
            { i32(-1), "ffffffff", false, 8 },
            { i32(-2), "FFFFFFFE", true, 8 },
            { core::limitMax<i32>(), "7FFFFFFF", true, 8 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u32 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { u32(0), "00000000", true, 8 },

            { u32(0xF), "", true, 0 },
            { u32(0xF), "F", true, 1 },
            { u32(0xF), "0F", true, 2 },
            { u32(0xF), "00f", false, 3 },
            { u32(0xF), "000F", true, 4 },
            { u32(0xF), "0000f", false, 5 },
            { u32(0xF), "00000F", true, 6 },
            { u32(0xF), "000000f", false, 7 },
            { u32(0xF), "0000000f", false, 8 },
            { u32(0xF), "0000000F", true, 8 },

            { u32(0x1D49), "00001D49", true, 8 },
            { u32(0x1D49), "00001d49", false, 8 },
            { u32(0x0F0F), "00000F0F", true, 8 },
            { u32(0x12345678), "12345678", true, 8 },
            { u32(0x12345678), "2345678", true, 7 },
            { u32(0x12345678), "345678", true, 6 },

            { core::limitMax<u32>(), "FFFFFFFF", true, 8 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i64 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i64>(), "8000000000000000", true, 16 },
            { i64(0), "0000000000000000", true, 16 },

            { i64(0xF), "", true, 0 },
            { i64(0xF), "f", false, 1 },
            { i64(0xF), "0F", true, 2 },
            { i64(0xF), "00F", true, 3 },
            { i64(0xF), "000F", true, 4 },
            { i64(0xF), "0000f", false, 5 },
            { i64(0xF), "00000F", true, 6 },
            { i64(0xF), "000000F", true, 7 },
            { i64(0xF), "0000000f", false, 8 },
            { i64(0xF), "00000000F", true, 9 },
            { i64(0xF), "000000000F", true, 10 },
            { i64(0xF), "0000000000F", true, 11 },
            { i64(0xF), "00000000000f", false, 12 },
            { i64(0xF), "000000000000F", true, 13 },
            { i64(0xF), "0000000000000F", true, 14 },
            { i64(0xF), "00000000000000F", true, 15 },
            { i64(0xF), "000000000000000f", false, 16 },

            { i64(0x1D49), "0000000000001D49", true, 16 },
            { i64(0x0F0F), "0000000000000F0F", true, 16 },
            { i64(0x12345678), "0000000012345678", true, 16 },
            { i64(0x123456789ABCDEF0), "123456789ABCDEF0", true, 16 },
            { i64(0x123456789ABCDEF0), "123456789abcdef0", false, 16 },

            { core::limitMax<i64>(), "7FFFFFFFFFFFFFFF", true, 16 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u64 in; const char* expected; bool upperCase; u32 hexLen; };
        constexpr TestCase cases[] = {
            { u64(0), "0000000000000000", true, 16 },

            { u64(0xF), "", true, 0 },
            { u64(0xF), "f", false, 1 },
            { u64(0xF), "0F", true, 2 },
            { u64(0xF), "00F", true, 3 },
            { u64(0xF), "000F", true, 4 },
            { u64(0xF), "0000f", false, 5 },
            { u64(0xF), "00000F", true, 6 },
            { u64(0xF), "000000F", true, 7 },
            { u64(0xF), "0000000f", false, 8 },
            { u64(0xF), "00000000F", true, 9 },
            { u64(0xF), "000000000F", true, 10 },
            { u64(0xF), "0000000000F", true, 11 },
            { u64(0xF), "00000000000f", false, 12 },
            { u64(0xF), "000000000000F", true, 13 },
            { u64(0xF), "0000000000000F", true, 14 },
            { u64(0xF), "00000000000000F", true, 15 },
            { u64(0xF), "000000000000000f", false, 16 },

            { u64(0x1D49), "0000000000001D49", true, 16 },
            { u64(0x1D49), "00000000001d49", false, 14 },
            { u64(0x0F0F), "0000000000000F0F", true, 16 },
            { u64(0x12345678), "0000000012345678", true, 16 },
            { u64(0x123456789ABCDEF0), "123456789ABCDEF0", true, 16 },
            { u64(0x123456789ABCDEF0), "123456789abcdef0", false, 16 },

            { core::limitMax<u64>(), "FFFFFFFFFFFFFFFF", true, 16 },
            { core::limitMax<u64>(), "ffffffffffffffff", false, 16 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    return 0;
}

constexpr i32 intToHexErrorsTest() {
    using ConversionError = core::ConversionError;

    {
        auto res = core::intToHex(7, nullptr, 20);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[4] = {};
        auto res = core::intToHex(7, buff, 0);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[2] = {};
        auto res = core::intToHex(7, buff, 2, true, 2);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::OutputBufferTooSmall);
    }

    return 0;
}

constexpr i32 intToBinaryTest() {
     auto runTestCases = [](auto& cases) -> i32 {
        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[65] = {};
            auto res = core::intToBinary(c.in, buf, CORE_C_ARRLEN(buf), c.binLen);
            CT_CHECK(res.hasValue(), cErr);
            CT_CHECK(core::memcmp(buf, core::cstrLen(buf), c.expected, core::cstrLen(c.expected)) == 0, cErr);
            return 0;
        });

        CT_CHECK(ret == 0);
        return 0;
    };

    {
        struct TestCase { i8 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i8>(), "10000000", 8 },
            { i8(0), "00000000", 8 },
            { i8(0), "0", 1 },
            { i8(0), "", 0 },
            { i8(5), "00000101", 8 },
            { i8(5), "101", 3 },
            { i8(-1), "11111111", 8 },
            { i8(15), "00001111", 8 },
            { i8(15), "1111", 4 },
            { core::limitMax<i8>(), "01111111", 8 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u8 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { u8(0), "00000000", 8 },
            { u8(0), "0", 1 },
            { u8(0), "", 0 },
            { u8(15), "00001111", 8 },
            { u8(15), "1111", 4 },
            { u8(255), "11111111", 8 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i16 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i16>(), "1000000000000000", 16 },
            { i16(0), "0000000000000000", 16 },
            { i16(0), "0", 1 },
            { i16(0), "", 0 },
            { i16(5), "0000000000000101", 16 },
            { i16(-1), "1111111111111111", 16 },
            { i16(15), "0000000000001111", 16 },
            { core::limitMax<i16>(), "0111111111111111", 16 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u16 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { u16(0), "0000000000000000", 16 },
            { u16(0), "0", 1 },
            { u16(0), "", 0 },
            { u16(15), "0000000000001111", 16 },
            { u16(15), "1111", 4 },
            { u16(65535), "1111111111111111", 16 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i32 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i32>(), "10000000000000000000000000000000", 32 },
            { i32(0), "00000000000000000000000000000000", 32 },
            { i32(0), "0", 1 },
            { i32(0), "", 0 },
            { i32(15), "00000000000000000000000000001111", 32 },
            { i32(15), "1111", 4 },
            { i32(-1), "11111111111111111111111111111111", 32 },
            { core::limitMax<i32>(), "01111111111111111111111111111111", 32 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u32 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { u32(0), "00000000000000000000000000000000", 32 },
            { u32(1), "00000000000000000000000000000001", 32 },
            { u32(15), "00000000000000000000000000001111", 32 },
            { u32(255), "00000000000000000000000011111111", 32 },
            { u32(1023), "00000000000000000000001111111111", 32 },
            { u32(4294967295), "11111111111111111111111111111111", 32 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { i64 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { core::limitMin<i64>(), "1000000000000000000000000000000000000000000000000000000000000000", 64 },
            { i64(0), "0000000000000000000000000000000000000000000000000000000000000000", 64 },
            { i64(1), "0000000000000000000000000000000000000000000000000000000000000001", 64 },
            { i64(15), "0000000000000000000000000000000000000000000000000000000000001111", 64 },
            { i64(-1), "1111111111111111111111111111111111111111111111111111111111111111", 64 },
            { core::limitMax<i64>(), "0111111111111111111111111111111111111111111111111111111111111111", 64 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    {
        struct TestCase { u64 in; const char* expected; u32 binLen; };
        constexpr TestCase cases[] = {
            { u64(0), "0000000000000000000000000000000000000000000000000000000000000000", 64 },
            { u64(1), "0000000000000000000000000000000000000000000000000000000000000001", 64 },
            { u64(15), "0000000000000000000000000000000000000000000000000000000000001111", 64 },
            { u64(255), "0000000000000000000000000000000000000000000000000000000011111111", 64 },
            { u64(1023), "0000000000000000000000000000000000000000000000000000001111111111", 64 },
            { core::limitMax<u64>(), "1111111111111111111111111111111111111111111111111111111111111111", 64 },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    return 0;
}

constexpr i32 intToBinaryErrorTest() {
    using ConversionError = core::ConversionError;

    {
        auto res = core::intToBinary(7, nullptr, 20);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[4] = {};
        auto res = core::intToBinary(7, buff, 0);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::InputEmpty);
    }
    {
        char buff[2] = {};
        auto res = core::intToBinary(7, buff, 2, 2);
        CT_CHECK(res.hasErr()); CT_CHECK(res.err() == ConversionError::OutputBufferTooSmall);
    }

    return 0;
}

i32 runCstrConvTestsSuite() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo();
    tInfo.name = FN_NAME_TO_CPTR(cstrToIntTest);
    if (runTest(tInfo, cstrToIntTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(cstrToIntErrorsTest);
    if (runTest(tInfo, cstrToIntErrorsTest)) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToCstrTest);
    if (runTest(tInfo, intToCstrTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToCstrErrorsTest);
    if (runTest(tInfo, intToCstrErrorsTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToHexTest);
    if (runTest(tInfo, intToHexTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToHexErrorsTest);
    if (runTest(tInfo, intToHexErrorsTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToBinaryTest);
    if (runTest(tInfo, intToBinaryTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(intToBinaryErrorTest);
    if (runTest(tInfo, intToBinaryErrorTest) != 0) { return -1; }

    return 0;
}

constexpr i32 runCompiletimeCstrConvTestsSuite() {
    RunTestCompileTime(cstrToIntTest);
    RunTestCompileTime(intToCstrTest);
    RunTestCompileTime(intToHexTest);
    RunTestCompileTime(intToHexErrorsTest);
    RunTestCompileTime(intToBinaryTest);
    RunTestCompileTime(intToBinaryErrorTest);

    return 0;
}
