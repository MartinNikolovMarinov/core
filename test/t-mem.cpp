constexpr i32 align_test() {
    struct test_case {
        ptr_size in;
        ptr_size expected;
    };

    constexpr test_case cases[] = {
        { 0, 0 },
        { 1, 8 },
        { 2, 8 },
        { 3, 8 },
        { 4, 8 },
        { 5, 8 },
        { 6, 8 },
        { 7, 8 },
        { 8, 8 },
        { 9, 16 },
        { 10, 16 },
        { 17, 24 },
        { 25, 32 }
    };

    executeTestTable("align_test failed at index: ", cases, [](auto& c, const char* cErr) {
        auto got = core::align(c.in);
        Assert(got == c.expected, cErr);
    });

    return 0;
}

i32 swap_bytes_test() {
    auto run_test_case = [](auto& a, auto& b, ptr_size N) {
        for (i32 i = 0; i < N; ++i) {
            a[i] = i;
            b[i] = i + N;
        }

        core::swap_bytes(a, b, N);

        for (i32 i = 0; i < N; ++i) {
            Assert(a[i] == i + N);
            Assert(b[i] == i);
        }
    };

    {
        // small
        u8 a[1] = {};
        u8 b[1] = {};
        run_test_case(a, b, 1);
    }
    {
        // odd
        u8 a[5] = {};
        u8 b[5] = {};
        run_test_case(a, b, 5);
    }
    {
        // even
        u8 a[6] = {};
        u8 b[6] = {};
        run_test_case(a, b, 6);
    }
    {
        struct A {
            i32 a;
            u64 b;
            u8 c;
        };

        A a1 = { core::MAX_I32, core::MAX_U64, core::MAX_U8 };
        A a2 = { core::MIN_I32, 0, 0 };
        core::swap_bytes(&a1, &a2, sizeof(A));
        Assert(a1.a == core::MIN_I32);
        Assert(a1.b == 0);
        Assert(a1.c == 0);
        Assert(a2.a == core::MAX_I32);
        Assert(a2.b == core::MAX_U64);
        Assert(a2.c == core::MAX_U8);
    }
    {
        struct A {
            i32 a;
            u8 b[3];
            i32 c;
        };

        A a1 = { 1, {2, 3, 4}, 5 };
        A a2 = { 5, {6, 7, 8}, 9 };
        core::swap_bytes(&a1, &a2, sizeof(A));
        Assert(a1.a == 5);
        Assert(a1.b[0] == 6);
        Assert(a1.b[1] == 7);
        Assert(a1.b[2] == 8);
        Assert(a1.c == 9);
        Assert(a2.a == 1);
        Assert(a2.b[0] == 2);
        Assert(a2.b[1] == 3);
        Assert(a2.b[2] == 4);
        Assert(a2.c == 5);
    }

    return 0;
}

i32 memcopy_tests() {
    constexpr i32 N = 20;
    u8 sequence[N] = {};

    // Set the sequence from 1..N
    for (i32 i = 0; i < N; i++) {
        sequence[i] = i;
    }

    for (i32 i = 0; i < N; i++) {
        u8 buf[N] = {};
        core::memcopy(buf, sequence, i); // copy the sequence into the buffer to i
        for (i32 j = 0; j < i; j++) {
            // Assert that the first i bytes are the same as the sequence
            Assert(buf[j] == j);
        }
        for (i32 j = i; j < N; j++) {
            // Assert that the rest are all zeroes
            Assert(buf[j] == 0);
        }
    }

    return 0;
}

i32 memset_tests() {
    constexpr i32 N = 20;
    for (i32 i = 0; i < N; i++) {
        u8 buf[N] = {};
        core::memset(buf, 7, i); // set the first i bytes to 7
        for (i32 j = 0; j < i; j++) {
            // Assert that the first i bytes are 7
            Assert(buf[j] == 7);
        }
        for (i32 j = i; j < N; j++) {
            // Assert that the rest are all zeroes
            Assert(buf[j] == 0);
        }
    }

    return 0;
}

i32 memcmp_tests() {
     struct test_case {
        const char* a;
        const char* b;
        ptr_size n;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    constexpr test_case cases[] = {
        { "", "", 0, test_case::zero },
        { "asdzxcasd", "", 0, test_case::zero },
        { "abc", "abc", 3, test_case::zero },
        { "abc", "abd", 3, test_case::negative },
        { "abd", "abc", 3, test_case::positive },
        { "abc123", "abc000", 3, test_case::zero },
        { "abc123", "abc000", 4, test_case::positive },
    };

    executeTestTable("memcmp_tests failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case 1:  Assert(core::memcmp(c.a, c.b, c.n) > 0, cErr);  break;
            case -1: Assert(core::memcmp(c.a, c.b, c.n) < 0, cErr);  break;
            case 0:  Assert(core::memcmp(c.a, c.b, c.n) == 0, cErr); break;
        }
    });

    return 0;
}

i32 run_mem_tests_suite() {
    RunTest(align_test);
    RunTest(swap_bytes_test);
    RunTest(memcopy_tests);
    RunTest(memset_tests);
    RunTest(memcmp_tests);

    return 0;
}

constexpr i32 run_compiletime_mem_tests_suite() {
    RunTestCompileTime(align_test);

    return 0;
}
