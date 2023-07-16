i32 int_hex_test() {
    char buf[20];

    {
        struct test_case { i8 in; const char* expected; };
        test_case cases[] = {
            { core::MIN_I8, "80" },
            { i8(0), "00" },
            { i8(0xF), "0F" },
            { i8(-1), "FF" },
            { core::MAX_I8, "7F" },
        };
        executeTestTable("test case failed for i8 at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { i16 in; const char* expected; };
        test_case cases[] = {
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
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { i32 in; const char* expected; };
        test_case cases[] = {
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
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { i64 in; const char* expected; };
        test_case cases[] = {
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
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { u8 in; const char* expected; };
        test_case cases[] = {
            { u8(0), "00" },
            { u8(0xF), "0F" },
            { core::MAX_U8, "FF" },
        };
        executeTestTable("test case failed for u8 at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { u16 in; const char* expected; };
        test_case cases[] = {
            { u16(0), "0000" },
            { u16(0xF), "000F" },
            { u16(0x1D49), "1D49" },
            { u16(0x0F0F), "0F0F" },
            { core::MAX_U16, "FFFF" },
        };
        executeTestTable("test case failed for u16 at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { u32 in; const char* expected; };
        test_case cases[] = {
            { u32(0), "00000000" },
            { u32(0xF), "0000000F" },
            { u32(0x1D49), "00001D49" },
            { u32(0x0F0F), "00000F0F" },
            { u32(0x12345678), "12345678" },
            { core::MAX_U32, "FFFFFFFF" },
        };
        executeTestTable("test case failed for u32 at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    {
        struct test_case { u64 in; const char* expected; };
        test_case cases[] = {
            { u64(0), "0000000000000000" },
            { u64(0xF), "000000000000000F" },
            { u64(0x1D49), "0000000000001D49" },
            { u64(0x0F0F), "0000000000000F0F" },
            { u64(0x12345678), "0000000012345678" },
            { u64(0x123456789ABCDEF0), "123456789ABCDEF0" },
            { core::MAX_U64, "FFFFFFFFFFFFFFFF" },
        };
        executeTestTable("test case failed for u64 at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buf, 0, 20);
            core::int_to_hex(c.in, buf);
            Assert(core::cptr_cmp(buf, c.expected) == 0, cErr);
        });
    }

    return 0;
}

i32 run_char_ptr_conv_tests_suite() {
    RunTest(int_hex_test);

    return 0;
}
