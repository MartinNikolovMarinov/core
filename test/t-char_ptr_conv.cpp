constexpr i32 char_to_int_test() {
    struct test_case {
        char in;
        i32 expected;
    };

    constexpr test_case cases[] = {
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::char_to_int<i8>(c.in) == i8(c.expected), cErr);
        Assert(core::char_to_int<i16>(c.in) == i16(c.expected), cErr);
        Assert(core::char_to_int<i32>(c.in) == i32(c.expected), cErr);
        Assert(core::char_to_int<i64>(c.in) == i64(c.expected), cErr);
        Assert(core::char_to_int<u8>(c.in) == u8(c.expected), cErr);
        Assert(core::char_to_int<u16>(c.in) == u16(c.expected), cErr);
        Assert(core::char_to_int<u32>(c.in) == u32(c.expected), cErr);
        Assert(core::char_to_int<u64>(c.in) == u64(c.expected), cErr);
        Assert(core::char_to_int<f32>(c.in) == f32(c.expected), cErr);
        Assert(core::char_to_int<f64>(c.in) == f64(c.expected), cErr);
    });

    return 0;
}

constexpr i32 digit_to_char_test() {
    struct test_case {
        i32 in;
        char expected;
    };

    constexpr test_case cases[] = {
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::digit_to_char(i8(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(i16(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(i32(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(i64(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(u8(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(u16(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(u32(c.in)) == c.expected, cErr);
        Assert(core::digit_to_char(u64(c.in)) == c.expected, cErr);
    });

    return 0;
};

constexpr i32 int_to_cptr_test() {
    {
        struct test_case {
            i32 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[12] = {};
            core::int_to_cptr(c.in, buf, c.digitCount);
            Assert(core::cptr_len(buf) == core::cptr_len(c.expected));
            Assert(core::cptr_eq(buf, c.expected, core::cptr_len(c.expected)), cErr);
        });
    }
    {
        struct test_case {
            i64 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[21] = {};
            core::int_to_cptr(c.in, buf, c.digitCount);
            Assert(core::cptr_len(buf) == core::cptr_len(c.expected));
            Assert(core::cptr_eq(buf, c.expected, core::cptr_len(c.expected)), cErr);
        });
    }

    {
        struct test_case {
            u32 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr test_case cases[] = {
            { 0, 1, "0" },
            { 1, 1, "1" },
            { 123, 3, "123" },
            { 123456789, 9, "123456789" },
            { 2147483647, 10, "2147483647" },
            { 2147483648, 10, "2147483648" },
            { 4294967295, 10, "4294967295" },
        };

        executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[11] = {};
            core::int_to_cptr(c.in, buf, c.digitCount);
            Assert(core::cptr_len(buf) == core::cptr_len(c.expected));
            Assert(core::cptr_eq(buf, c.expected, core::cptr_len(c.expected)), cErr);
        });
    }

    {
        struct test_case {
            u64 in;
            u32 digitCount;
            const char* expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_cptr(c.in, buf, c.digitCount);
            Assert(core::cptr_len(buf) == core::cptr_len(c.expected));
            Assert(core::cptr_eq(buf, c.expected, core::cptr_len(c.expected)), cErr);
        });
    }

    return 0;
}

constexpr i32 cptr_to_int_test() {
    {
        struct test_case {
            const char* input;
            i8 expected;
            bool skipAtCompileTime = false;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for i8 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return; }
            auto v = core::cptr_to_int<i8>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            u8 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for u8 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_int<u8>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            i16 expected;
            bool skipAtCompileTime = false;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for i16 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return; }
            auto v = core::cptr_to_int<i16>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            u16 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for u16 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_int<u16>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            i32 expected;
            bool skipAtCompileTime = false;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return; }
            auto v = core::cptr_to_int<i32>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            u32 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for u32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_int<u32>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            i64 expected;
            bool skipAtCompileTime = false;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            IS_CONST_EVALUATED { if (c.skipAtCompileTime) return; }
            auto v = core::cptr_to_int<i64>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            u64 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_int<u64>(c.input);
            Assert(v == c.expected, cErr);
        });
    }

    return 0;
}

constexpr i32 cptr_to_float_test() {

    {
        struct test_case {
            const char* input;
            f32 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_float<f32>(c.input);
            Assert(core::safe_eq(v, c.expected, 0.000001f), cErr);
        });
    }

    {
        struct test_case {
            const char* input;
            f64 expected;
        };

        constexpr test_case cases[] = {
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

        executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cptr_to_float<f64>(c.input);
            Assert(core::safe_eq(v, c.expected, 0.00000001), cErr);
        });
    }

    return 0;
}

constexpr i32 int_hex_test() {
    {
        struct test_case { i8 in; const char* expected; };
        constexpr test_case cases[] = {
            { core::MIN_I8, "80" },
            { i8(0), "00" },
            { i8(0xF), "0F" },
            { i8(-1), "FF" },
            { core::MAX_I8, "7F" },
        };

        executeTestTable("test case failed for i8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { i16 in; const char* expected; };
        constexpr test_case cases[] = {
            { core::MIN_I16, "8000" },
            { i16(0), "0000" },
            { i16(0xF), "000F" },
            { i16(0x1D49), "1D49" },
            { i16(0x0F0F), "0F0F" },
            { i16(-1), "FFFF" },
            { i16(-2), "FFFE" },
            { core::MAX_I16, "7FFF" },
        };
        executeTestTable("test case failed for i16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { i32 in; const char* expected; };
        constexpr test_case cases[] = {
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
        executeTestTable("test case failed for i32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { i64 in; const char* expected; };
        constexpr test_case cases[] = {
            { core::MIN_I64, "8000000000000000" },
            { i64(0), "0000000000000000" },
            { i64(0xF), "000000000000000F" },
            { i64(0x1D49), "0000000000001D49" },
            { i64(0x0F0F), "0000000000000F0F" },
            { i64(0x12345678), "0000000012345678" },
            { i64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::MAX_I64, "7FFFFFFFFFFFFFFF" },
        };
        executeTestTable("test case failed for i64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { u8 in; const char* expected; };
        constexpr test_case cases[] = {
            { u8(0), "00" },
            { u8(0xF), "0F" },
            { core::MAX_U8, "FF" },
        };
        executeTestTable("test case failed for u8 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { u16 in; const char* expected; };
        constexpr test_case cases[] = {
            { u16(0), "0000" },
            { u16(0xF), "000F" },
            { u16(0x1D49), "1D49" },
            { u16(0x0F0F), "0F0F" },
            { core::MAX_U16, "FFFF" },
        };
        executeTestTable("test case failed for u16 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { u32 in; const char* expected; };
        constexpr test_case cases[] = {
            { u32(0), "00000000" },
            { u32(0xF), "0000000F" },
            { u32(0x1D49), "00001D49" },
            { u32(0x0F0F), "00000F0F" },
            { u32(0x12345678), "12345678" },
            { core::MAX_U32, "FFFFFFFF" },
        };
        executeTestTable("test case failed for u32 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    {
        struct test_case { u64 in; const char* expected; };
        constexpr test_case cases[] = {
            { u64(0), "0000000000000000" },
            { u64(0xF), "000000000000000F" },
            { u64(0x1D49), "0000000000001D49" },
            { u64(0x0F0F), "0000000000000F0F" },
            { u64(0x12345678), "0000000012345678" },
            { u64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::MAX_U64, "FFFFFFFFFFFFFFFF" },
        };
        executeTestTable("test case failed for u64 at index: ", cases, [&](auto& c, const char* cErr) {
            char buf[20] = {};
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)) == 0, cErr);
        });
    }

    return 0;
}

i32 run_char_ptr_conv_tests_suite() {
    RunTest(char_to_int_test);
    RunTest(digit_to_char_test);
    RunTest(int_to_cptr_test);
    RunTest(int_hex_test);
    RunTest(cptr_to_int_test);
    RunTest(cptr_to_float_test);

    return 0;
}

constexpr i32 run_compiletime_char_ptr_conv_tests_suite() {
    RunTestCompileTime(char_to_int_test);
    RunTestCompileTime(digit_to_char_test);
    RunTestCompileTime(int_to_cptr_test);
    RunTestCompileTime(int_hex_test);
    RunTestCompileTime(cptr_to_int_test);
    RunTestCompileTime(cptr_to_float_test);

    return 0;
}
