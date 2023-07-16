i32 digit_count_test() {
    {
        struct test_case {
            i32 value;
            u32 expected;
        };

        test_case cases[] = {
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

        executeTestTable("test case failed for i32 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::digit_count(c.value) == c.expected, cErr);
        });
    }
    {
        struct test_case {
            u32 value;
            u32 expected;
        };

        test_case cases[] = {
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

        executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::digit_count(c.value) == c.expected, cErr);
        });
    }
    {
        struct test_case {
            i64 value;
            u32 expected;
        };

        test_case cases[] = {
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

        executeTestTable("test case failed for i64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::digit_count(c.value) == c.expected, cErr);
        });
    }
    {
        struct test_case {
            u64 value;
            u32 expected;
        };

        test_case cases[] = {
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

        executeTestTable("test case failed for u64 at index: ", cases, [](auto& c, const char* cErr) {
            Assert(core::digit_count(c.value) == c.expected, cErr);
        });
    }

    return 0;
}

i32 run_ints_tests_suite() {
    RunTest(digit_count_test);

    return 0;
}
