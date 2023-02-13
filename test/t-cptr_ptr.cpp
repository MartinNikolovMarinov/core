void int_to_cptr_test() {
    struct test_case {
        i64 in;
        const char* expected;
    };

    test_case cases[] = {
        { 0, "0" },
        { 1, "1" },
        { -1, "-1" },
        { 123, "123" },
        { -123, "-123" },
        { 123456789, "123456789" },
        { -123456789, "-123456789" },
        { 2147483647, "2147483647" },
        { -2147483647, "-2147483647" },
        { 2147483648, "2147483648" },
        { -2147483648, "-2147483648" }
    };

    for (auto& c : cases) {
        char buf[20] = {};
        u32 digitCount = core::digit_count(c.in);
        core::int_to_cptr(c.in, buf, digitCount);
        Assert(core::cptr_eq(buf, core::cptr_len(buf), c.expected, core::cptr_len(c.expected)));
    }
}

constexpr void cptr_len_test() {
    struct test_case {
        const char* in;
        ptr_size expected;
    };

    test_case cases[] = {
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

    for (auto& c : cases) {
        ptr_size len = core::cptr_len(c.in);
        Assert(len == c.expected);
    }
}

constexpr void cptr_cmp_tests() {
    struct test_case {
        const char* a;
        const char* b;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    test_case cases[] = {
        { nullptr, nullptr, test_case::zero },
        { nullptr, "",      test_case::negative},
        { "",      nullptr, test_case::positive },
        { "",      "",      test_case::zero },
        { "a",     "",      test_case::positive },
        { "",      "a",     test_case::negative },
        { "a",     "a",     test_case::zero },
        { "a",     "b",     test_case::negative },
        { "b",     "a",     test_case::positive },
        { "abc",   "abc",   test_case::zero },
        { "abd",   "abc",   test_case::positive },
        { "abb",   "abc",   test_case::negative },
    };

    for (auto& c : cases) {
        switch (c.expected) {
            case 1:  Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) > 0);  break;
            case -1: Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) < 0);  break;
            case 0:  Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) == 0); break;
        }
    }
}

void cptr_cpy_test() {
    const char* src = "1234567890";
    char dst[20] = {};
    core::cptr_cpy(src, core::cptr_len(src), dst);

    Assert(core::cptr_eq(src, core::cptr_len(src), dst, core::cptr_len(dst)));
    Assert(dst[0] == '1');
    Assert(dst[1] == '2');
    Assert(dst[2] == '3');
    Assert(dst[3] == '4');
    Assert(dst[4] == '5');
    Assert(dst[5] == '6');
    Assert(dst[6] == '7');
    Assert(dst[7] == '8');
    Assert(dst[8] == '9');
    Assert(dst[9] == '0');
    Assert(dst[10] == '\0');

    core::memset(dst, 0, 20);
    core::cptr_cpy(src, 5, dst);

    Assert(core::cptr_eq(dst, core::cptr_len(dst), "12345", core::cptr_len("12345")));
    Assert(dst[0] == '1');
    Assert(dst[1] == '2');
    Assert(dst[2] == '3');
    Assert(dst[3] == '4');
    Assert(dst[4] == '5');
    Assert(dst[5] == '\0');
}

constexpr void cptr_idx_of_char_test() {
    struct test_case {
        const char* src;
        char val;
        ptr_size idx;
    };

    test_case cases[] = {
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

    for (auto& c : cases) {
        ptr_size idx = core::cptr_idx_of_char(c.src, core::cptr_len(c.src), c.val);
        Assert(idx == c.idx);
    }
}

constexpr void cptr_idx_of_test() {
    struct test_case {
        const char* src;
        const char* val;
        ptr_size idx;
    };

    test_case cases[] = {
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

    for (auto& c : cases) {
        ptr_size idx = core::cptr_idx_of(c.src, core::cptr_len(c.src), c.val, core::cptr_len(c.val));
        Assert(idx == c.idx);
    }
}

void run_cptr_ptr_tests_suite() {
    RunTest(int_to_cptr_test);
    RunTest(cptr_len_test);
    RunTest(cptr_cmp_tests);
    RunTest(cptr_cpy_test);
    RunTest(cptr_idx_of_char_test);
    RunTest(cptr_idx_of_test);
}
