constexpr i32 is_digit_test() {
    struct test_case {
        char in;
        bool expected;
    };

    test_case cases[] = {
        { ' ', false },
        { '-', false },
        { '/', false },
        { '0', true },
        { '1', true },
        { '2', true },
        { '3', true },
        { '4', true },
        { '5', true },
        { '6', true },
        { '7', true },
        { '8', true },
        { '9', true },
        { ':', false },
    };

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::is_digit(c.in) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 is_white_space_test() {
    struct test_case {
        char in;
        bool expected;
    };

    test_case cases[] = {
        { ' ', true },
        { '\n', true },
        { '\t', true },
        { '\r', true },
        { 'a', false },
        { '\\', false }
    };

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::is_white_space(c.in) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 cptr_len_test() {
    struct test_case {
        const char* in;
        addr_size expected;
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_size len = core::cptr_len(c.in);
        Assert(len == c.expected, cErr);
    });

    return 0;
}

constexpr i32 cptr_cmp_tests() {
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case 1:  Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) > 0, cErr);  break;
            case -1: Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) < 0, cErr);  break;
            case 0:  Assert(core::cptr_cmp(c.a, core::cptr_len(c.a), c.b, core::cptr_len(c.b)) == 0, cErr); break;
        }
    });

    return 0;
}

constexpr i32 cptr_eq_test() {
    struct test_case {
        const char* a;
        const char* b;
        addr_size len;
        bool expected;
    };

    test_case cases[] = {
        { nullptr, nullptr, 0, true },
        { nullptr, "",      0, false },
        { "",      nullptr, 0, false },
        { "",      "",      0, true },
        { "a",     "a",     1, true },
        { "a",     "b",     1, false },
        { "abc",   "abc",   3, true },
        { "abd",   "ab9",   2, true },
        { "a\0c",  "a\0c",  3, true },
    };

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::cptr_eq(c.a, c.b, c.len) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 cptr_copy_test() {
    const char* src = "1234567890";
    char dst[20] = {};
    core::cptr_copy(dst, src, core::cptr_len(src));

    Assert(core::cptr_len(src) == core::cptr_len(dst));
    Assert(core::cptr_eq(src, dst, core::cptr_len(dst)));
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

    for (u32 i = 0; i < 20; ++i) dst[i] = 0;
    core::cptr_copy(dst, src, 5);

    Assert(core::cptr_eq(dst, "12345", core::cptr_len("12345")));
    Assert(dst[0] == '1');
    Assert(dst[1] == '2');
    Assert(dst[2] == '3');
    Assert(dst[3] == '4');
    Assert(dst[4] == '5');
    Assert(dst[5] == '\0');

    return 0;
}

constexpr i32 cptr_idx_of_char_test() {
    struct test_case {
        const char* src;
        char val;
        addr_off idx;
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_off idx = core::cptr_idx_of_char(c.src, core::cptr_len(c.src), c.val);
        Assert(idx == c.idx, cErr);
    });

    return 0;
}

constexpr i32 cptr_idx_of_test() {
    struct test_case {
        const char* src;
        const char* val;
        addr_off idx;
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

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        addr_off idx = core::cptr_idx_of(c.src, core::cptr_len(c.src), c.val, core::cptr_len(c.val));
        Assert(idx == c.idx, cErr);
    });

    return 0;
}

constexpr i32 cptr_skip_white_space_test() {
    struct test_case {
        const char* src;
        const char* expected;
    };

    test_case cases[] = {
        { nullptr, nullptr },
        { "", "" },
        { " aa", "aa" },
        { "        aa", "aa" },
        { "\taa", "aa" },
        { "\taa  ", "aa  " },
        { "\naa", "aa" },
        { "\n\raa", "aa" },
    };

    executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        const char* res = core::cptr_skip_space(c.src);
        Assert(core::cptr_eq(res, c.expected, core::cptr_len(c.expected)), cErr);
    });

    return 0;
};

i32 run_cptr_ptr_tests_suite() {
    RunTest(is_digit_test);
    RunTest(is_white_space_test);
    RunTest(cptr_len_test);
    RunTest(cptr_cmp_tests);
    RunTest(cptr_eq_test);
    RunTest(cptr_copy_test);
    RunTest(cptr_idx_of_char_test);
    RunTest(cptr_idx_of_test);
    RunTest(cptr_skip_white_space_test);

    return 0;
}

constexpr i32 run_compiletime_cptr_ptr_tests_suite() {
    RunTestCompileTime(is_digit_test);
    RunTestCompileTime(is_white_space_test);
    RunTestCompileTime(cptr_len_test);
    RunTestCompileTime(cptr_cmp_tests);
    RunTestCompileTime(cptr_eq_test);
    RunTestCompileTime(cptr_copy_test);
    RunTestCompileTime(cptr_idx_of_char_test);
    RunTestCompileTime(cptr_idx_of_test);
    RunTestCompileTime(cptr_skip_white_space_test);

    return 0;
}
