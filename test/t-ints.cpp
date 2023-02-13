void int_to_hex_signed_ints_test() {
    char buf[20];

    core::memset(buf, 0, 20);
    core::int_to_hex(u8(0), buf);
    Assert(core::cptr_cmp(buf, "00") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u8(0xF), buf);
    Assert(core::cptr_cmp(buf, "0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_U8, buf);
    Assert(core::cptr_cmp(buf, "FF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u16(0), buf);
    Assert(core::cptr_cmp(buf, "0000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u16(0xF), buf);
    Assert(core::cptr_cmp(buf, "000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u16(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "1D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u16(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "0F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_U16, buf);
    Assert(core::cptr_cmp(buf, "FFFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u32(0), buf);
    Assert(core::cptr_cmp(buf, "00000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u32(0xF), buf);
    Assert(core::cptr_cmp(buf, "0000000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u32(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "00001D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u32(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "00000F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u32(0x12345678), buf);
    Assert(core::cptr_cmp(buf, "12345678") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_U32, buf);
    Assert(core::cptr_cmp(buf, "FFFFFFFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0), buf);
    Assert(core::cptr_cmp(buf, "0000000000000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0xF), buf);
    Assert(core::cptr_cmp(buf, "000000000000000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "0000000000001D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "0000000000000F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0x12345678), buf);
    Assert(core::cptr_cmp(buf, "0000000012345678") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(u64(0x123456789ABCDEF0), buf);
    Assert(core::cptr_cmp(buf, "123456789ABCDEF0") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_U64, buf);
    Assert(core::cptr_cmp(buf, "FFFFFFFFFFFFFFFF") == 0);
}

void int_to_hex_unsigned_ints_test() {
    char buf[20];

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MIN_I8, buf);
    Assert(core::cptr_cmp(buf, "80") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i8(0), buf);
    Assert(core::cptr_cmp(buf, "00") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i8(0xF), buf);
    Assert(core::cptr_cmp(buf, "0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i8(-1), buf);
    Assert(core::cptr_cmp(buf, "FF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_I8, buf);
    Assert(core::cptr_cmp(buf, "7F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MIN_I16, buf);
    Assert(core::cptr_cmp(buf, "8000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(0), buf);
    Assert(core::cptr_cmp(buf, "0000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(0xF), buf);
    Assert(core::cptr_cmp(buf, "000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "1D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "0F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(-1), buf);
    Assert(core::cptr_cmp(buf, "FFFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i16(-2), buf);
    Assert(core::cptr_cmp(buf, "FFFE") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_I16, buf);
    Assert(core::cptr_cmp(buf, "7FFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MIN_I32, buf);
    Assert(core::cptr_cmp(buf, "80000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(0), buf);
    Assert(core::cptr_cmp(buf, "00000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(0xF), buf);
    Assert(core::cptr_cmp(buf, "0000000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "00001D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "00000F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(0x12345678), buf);
    Assert(core::cptr_cmp(buf, "12345678") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(-1), buf);
    Assert(core::cptr_cmp(buf, "FFFFFFFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i32(-2), buf);
    Assert(core::cptr_cmp(buf, "FFFFFFFE") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_I32, buf);
    Assert(core::cptr_cmp(buf, "7FFFFFFF") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MIN_I64, buf);
    Assert(core::cptr_cmp(buf, "8000000000000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0), buf);
    Assert(core::cptr_cmp(buf, "0000000000000000") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0xF), buf);
    Assert(core::cptr_cmp(buf, "000000000000000F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0x1D49), buf);
    Assert(core::cptr_cmp(buf, "0000000000001D49") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0x0F0F), buf);
    Assert(core::cptr_cmp(buf, "0000000000000F0F") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0x12345678), buf);
    Assert(core::cptr_cmp(buf, "0000000012345678") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(i64(0x123456789ABCDEF0), buf);
    Assert(core::cptr_cmp(buf, "123456789ABCDEF0") == 0);

    core::memset(buf, 0, 20);
    core::int_to_hex(core::MAX_I64, buf);
    Assert(core::cptr_cmp(buf, "7FFFFFFFFFFFFFFF") == 0);
}

template <typename T>
struct __digit_count_test_case {
    T value;
    u32 expected;
};

void digit_count_test() {
    __digit_count_test_case<i32> cases1[] = {
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
    for (auto& c : cases1) {
        Assert(core::digit_count(c.value) == c.expected, "test case failed for i32");
    }

    __digit_count_test_case<u32> cases2[] = {
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
    for (auto& c : cases2) {
        Assert(core::digit_count(c.value) == c.expected, "test case failed for i32");
    }

    __digit_count_test_case<i64> cases3[] = {
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
    for (auto& c : cases3) {
        Assert(core::digit_count(c.value) == c.expected, "test case failed for i64");
    }

    __digit_count_test_case<u64> cases4[] = {
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
    for (auto& c : cases4) {
        Assert(core::digit_count(c.value) == c.expected, "test case failed for u64");
    }
}

void run_ints_tests_suite() {
    RunTest(int_to_hex_signed_ints_test);
    RunTest(int_to_hex_unsigned_ints_test);
    RunTest(digit_count_test);
}
