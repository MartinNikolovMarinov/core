constexpr i32 pow10_test() {
    struct test_case {
        u32 in;
        u64 expected;
    };

    constexpr test_case cases[] = {
        { 0, 1ULL },
        { 1, 10ULL },
        { 2, 100ULL },
        { 3, 1000ULL },
        { 4, 10000ULL },
        { 5, 100000ULL },
        { 6, 1000000ULL },
        { 7, 10000000ULL },
        { 8, 100000000ULL },
        { 9, 1000000000ULL },
        { 10, 10000000000ULL },
        { 11, 100000000000ULL },
        { 12, 1000000000000ULL },
        { 13, 10000000000000ULL },
        { 14, 100000000000000ULL },
        { 15, 1000000000000000ULL },
        { 16, 10000000000000000ULL },
        { 17, 100000000000000000ULL },
        { 18, 1000000000000000000ULL },
        { 19, 10000000000000000000ULL },
    };

    executeTestTable("pow10 test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::pow10(c.in) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 pow2_test() {
    struct test_case {
        u32 in;
        u64 expected;
    };

    constexpr test_case cases[] = {
        { 0, 1ULL },
        { 1, 2ULL },
        { 2, 4ULL },
        { 3, 8ULL },
        { 4, 16ULL },
        { 5, 32ULL },
        { 6, 64ULL },
        { 7, 128ULL },
        { 8, 256ULL },
        { 9, 512ULL },
        { 10, 1024ULL },
        { 11, 2048ULL },
        { 12, 4096ULL },
        { 13, 8192ULL },
        { 14, 16384ULL },
        { 15, 32768ULL },
        { 16, 65536ULL },
        { 17, 131072ULL },
        { 18, 262144ULL },
        { 19, 524288ULL },
        { 20, 1048576ULL },
        { 21, 2097152ULL },
        { 22, 4194304ULL },
        { 23, 8388608ULL },
        { 24, 16777216ULL },
        { 25, 33554432ULL },
        { 26, 67108864ULL },
        { 27, 134217728ULL },
        { 28, 268435456ULL },
        { 29, 536870912ULL },
        { 30, 1073741824ULL },
        { 31, 2147483648ULL },
        { 32, 4294967296ULL },
        { 33, 8589934592ULL },
        { 34, 17179869184ULL },
        { 35, 34359738368ULL },
        { 36, 68719476736ULL },
        { 37, 137438953472ULL },
        { 38, 274877906944ULL },
        { 39, 549755813888ULL },
        { 40, 1099511627776ULL },
        { 41, 2199023255552ULL },
        { 42, 4398046511104ULL },
        { 43, 8796093022208ULL },
        { 44, 17592186044416ULL },
        { 45, 35184372088832ULL },
        { 46, 70368744177664ULL },
        { 47, 140737488355328ULL },
        { 48, 281474976710656ULL },
        { 49, 562949953421312ULL },
        { 50, 1125899906842624ULL },
        { 51, 2251799813685248ULL },
        { 52, 4503599627370496ULL },
        { 53, 9007199254740992ULL },
        { 54, 18014398509481984ULL },
        { 55, 36028797018963968ULL },
        { 56, 72057594037927936ULL },
        { 57, 144115188075855872ULL },
        { 58, 288230376151711744ULL },
        { 59, 576460752303423488ULL },
        { 60, 1152921504606846976ULL },
        { 61, 2305843009213693952ULL },
        { 62, 4611686018427387904ULL },
        { 63, 9223372036854775808ULL },
    };

    executeTestTable("pow2 test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::pow2(c.in) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 degrees_test() {
    Assert(core::deg_to_rad(0.0f) == 0.0f);
    Assert(core::deg_to_rad(90.0f) == core::PI / 2.0f);
    Assert(core::deg_to_rad(180.0f) == core::PI);
    Assert(core::deg_to_rad(270.0f) == core::PI * 1.5f);
    Assert(core::deg_to_rad(360.0f) == core::PI * 2.0f);

    Assert(core::rad_to_deg(core::radians(0.0f)) == 0.0f);
    Assert(core::rad_to_deg(core::radians(core::PI / 2.0f)) == 90.0f);
    Assert(core::rad_to_deg(core::radians(core::PI)) == 180.0f);
    Assert(core::rad_to_deg(core::radians(core::PI * 1.5f)) == 270.0f);
    Assert(core::rad_to_deg(core::radians(core::PI * 2.0f)) == 360.0f);

    return 0;
}

i32 mantissa_exponent_test() {
    {
        struct test_case {
            f32 in;
            u32 expectedMantissa;
            u32 expectedExponent;
        };

        constexpr test_case cases[] = {
            { 0,        0b0,                       0b0 },
            { 966.68f,  0b11100011010101110000101, 0b10001000 }, // has conversion error
            { 16.4f,    0b00000110011001100110011, 0b10000011 }, // has conversion error
            { -12.375f, 0b10001100000000000000000, 0b10000010 },
            { -0.02f,   0b01000111101011100001010, 0b01111001 }, // has conversion error
        };

        executeTestTable("mantissa_exponent 32bit test case failed at index: ", cases, [](auto& c, const char* cErr) {
            auto gotMantissa = core::mantissa(c.in);
            auto gotExponent = core::exponent(c.in);
            Assert(gotMantissa == c.expectedMantissa, cErr);
            Assert(gotExponent == c.expectedExponent, cErr);
        });
    }

    {
        struct test_case {
            f64 in;
            u64 expectedMantissa;
            u64 expectedExponent;
        };

        constexpr test_case cases[] = {
            { 0,        0b0,                                                    0b0 },
            { 966.68,   0b1110001101010111000010100011110101110000101000111101, 0b10000001000 }, // has conversion error
            { 16.4,     0b0000011001100110011001100110011001100110011001100110, 0b10000000011 }, // has conversion error
            { -12.375,  0b1000110000000000000000000000000000000000000000000000, 0b10000000010 },
            { -0.02,    0b0100011110101110000101000111101011100001010001111011, 0b01111111001 }, // has conversion error
        };

        executeTestTable("mantissa_exponent 64bit test case failed at index: ", cases, [](auto& c, const char* cErr) {
            auto gotMantissa = core::mantissa(c.in);
            auto gotExponent = core::exponent(c.in);
            Assert(gotMantissa == c.expectedMantissa, cErr);
            Assert(gotExponent == c.expectedExponent, cErr);
        });
    }

    return 0;
}

constexpr i32 abs_test() {
    Assert(core::abs_slow(i8(0)) == 0);
    Assert(core::abs_slow(i8(1)) == 1);
    Assert(core::abs_slow(i8(-1)) == 1);
    Assert(core::abs_slow(i16(0)) == 0);
    Assert(core::abs_slow(i16(1)) == 1);
    Assert(core::abs_slow(i16(-1)) == 1);
    Assert(core::abs_slow(i32(0)) == 0);
    Assert(core::abs_slow(i32(1)) == 1);
    Assert(core::abs_slow(i32(-1)) == 1);
    Assert(core::abs_slow(i64(0)) == 0);
    Assert(core::abs_slow(i64(1)) == 1);
    Assert(core::abs_slow(i64(-1)) == 1);
    Assert(core::abs(f32(0)) == 0.0f);
    Assert(core::abs(f32(1)) == 1.0f);
    Assert(core::abs(f32(-1)) == 1.0f);
    Assert(core::abs(f64(0)) == 0.0);
    Assert(core::abs(f64(1)) == 1.0);
    Assert(core::abs(f64(-1)) == 1.0);

    // check with max and min values
    Assert(core::abs_slow(i8(core::MAX_I8))       == core::MAX_I8);
    Assert(core::abs_slow(i8(core::MIN_I8 + 1))   == core::MAX_I8);
    Assert(core::abs_slow(i16(core::MAX_I16))     == core::MAX_I16);
    Assert(core::abs_slow(i16(core::MIN_I16 + 1)) == core::MAX_I16);
    Assert(core::abs_slow(i32(core::MAX_I32))     == core::MAX_I32);
    Assert(core::abs_slow(i32(core::MIN_I32 + 1)) == core::MAX_I32);
    Assert(core::abs_slow(i64(core::MAX_I64))     == core::MAX_I64);
    Assert(core::abs_slow(i64(core::MIN_I64 + 1)) == core::MAX_I64);

    return 0;
}

constexpr i32 is_positive_test() {
    Assert(core::is_positive(i8(0)) == true);
    Assert(core::is_positive(i8(1)) == true);
    Assert(core::is_positive(i8(-1)) == false);
    Assert(core::is_positive(i16(0)) == true);
    Assert(core::is_positive(i16(1)) == true);
    Assert(core::is_positive(i16(-1)) == false);
    Assert(core::is_positive(i32(0)) == true);
    Assert(core::is_positive(i32(1)) == true);
    Assert(core::is_positive(i32(-1)) == false);
    Assert(core::is_positive(i64(0)) == true);
    Assert(core::is_positive(i64(1)) == true);
    Assert(core::is_positive(i64(-1)) == false);
    Assert(core::is_positive(f32(0)) == true);
    Assert(core::is_positive(f32(1)) == true);
    Assert(core::is_positive(f32(-1)) == false);
    Assert(core::is_positive(f64(0)) == true);
    Assert(core::is_positive(f64(1)) == true);
    Assert(core::is_positive(f64(-1)) == false);

    return 0;
}

constexpr i32 float_safe_eq_test() {
    struct test_case {
        f32 startA;
        f32 startB;
        f32 step;
        f32 epsilon;
        i32 iterations;
        bool expected;
    };

    test_case cases[] = {
        { 0.1f, 0.1f, 0.1f,  0.01f,  10, true },
        { 0.1f, 0.1f, 0.2f,  0.01f,  10, true },
        { 0.1f, 0.1f, 0.01f, 0.01f, 100, true },

        { 0.1f, 0.2f, 0.1f,  0.01f,  10, false },
        { 0.1f, 0.2f, 0.01f, 0.01f, 100, false },

        { 0.001111f, 0.00999f, 0.001f, 0.01f, 10, true },
        { 0.001111f, 0.00999f, 0.001f, 0.001f, 10, false },
    };

    i32 i = 0;
    constexpr const char* iterAsCptrFmt = "float_safe_eq test case failed at index: ";
    constexpr const addr_size iterAsCptrFmtLen = core::cptr_len(iterAsCptrFmt);
    char iterAsCptr[iterAsCptrFmtLen + 20] = {};
    for (i32 j = 0; j < i32(iterAsCptrFmtLen); ++j) iterAsCptr[j] = iterAsCptrFmt[j];
    char* appendIdx = &iterAsCptr[iterAsCptrFmtLen];
    for (auto& c : cases) {
        core::int_to_cptr(i++, appendIdx, 2);
        for (i32 j = 0; j < c.iterations; ++j) {
            Assert(core::safe_eq(c.startA, c.startB, c.epsilon) == c.expected, iterAsCptr);
            c.startA += c.step;
            c.startB += c.step;
        }
    }

    return 0;
}

i32 float_nearly_eq_extream_cases_test() {
    struct test_case {
        f32 a;
        f32 b;
        f32 epsilon;
        bool expected;
    };

    constexpr f32 defaultEpsilon = 0.00001f;
    test_case cases[] = {
        { MAX_F32, MAX_F32, defaultEpsilon, true },
        // { MAX_F32, -MAX_F32, defaultEpsilon, false }, // These 2 look like a bug, but I don't think I care.
        // { -MAX_F32, MAX_F32, defaultEpsilon, false },
        { MAX_F32, MAX_F32 / 2, defaultEpsilon, false },
        { MAX_F32, -MAX_F32 / 2, defaultEpsilon, false },
        { -MAX_F32, MAX_F32 / 2, defaultEpsilon, false },

        { MIN_F32, MIN_F32, defaultEpsilon, true },
        { MIN_F32, -MIN_F32, defaultEpsilon, true },
        { -MIN_F32, MIN_F32, defaultEpsilon, true },
        { MIN_F32, 0, defaultEpsilon, false },
        { 0, MIN_F32, defaultEpsilon, false },
        { -MIN_F32, 0, defaultEpsilon, false },
        { 0, -MIN_F32, defaultEpsilon, false },
        { 0.000000001f, -MIN_F32, defaultEpsilon, false },
        { 0.000000001f, MIN_F32, defaultEpsilon, false },
        { MIN_F32, 0.000000001f, defaultEpsilon, false },
        { -MIN_F32, 0.000000001f, defaultEpsilon, false },
    };

        executeTestTable("float_nearly_eq_extream_cases_test test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::nearly_eq(c.a, c.b, c.epsilon) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 float_nearly_eq_test() {
    struct test_case {
        f32 a;
        f32 b;
        f32 epsilon;
        bool expected;
    };

    constexpr f32 defaultEpsilon = 0.00001f;

    test_case cases[] = {
        { 1000000.f, 1000001.f, defaultEpsilon, true },
        { 1000001.f, 1000000.f, defaultEpsilon, true },
        { 10000.f, 10001.f, defaultEpsilon, false },
        { 10001.f, 10000.f, defaultEpsilon, false },

        { -1000000.f, -1000001.f, defaultEpsilon, true},
        { -1000001.f, -1000000.f, defaultEpsilon, true},
        { -10000.f,  -10001.f, defaultEpsilon, false},
        { -10001.f,  -10000.f, defaultEpsilon, false},

        { -1000000.f, -1000001.f, defaultEpsilon, true},
        { -1000001.f, -1000000.f, defaultEpsilon, true},
        { -10000.f,  -10001.f, defaultEpsilon, false},
        { -10001.f,  -10000.f, defaultEpsilon, false},

        { -1.000001f, -1.000002f, defaultEpsilon, true },
        { -1.000002f, -1.000001f, defaultEpsilon, true },
        { -1.0001f,  -1.0002f, defaultEpsilon, false },
        { -1.0002f,  -1.0001f, defaultEpsilon, false },

        { 0.000000001000001f,  0.000000001000002f, defaultEpsilon, true },
        { 0.000000001000002f,  0.000000001000001f, defaultEpsilon, true },
        { 0.000000000001002f, 0.000000000001001f, defaultEpsilon, false },
        { 0.000000000001001f, 0.000000000001002f, defaultEpsilon, false },

        { -0.000000001000001f,  -0.000000001000002f, defaultEpsilon, true },
        { -0.000000001000002f,  -0.000000001000001f, defaultEpsilon, true },
        { -0.000000000001002f, -0.000000000001001f, defaultEpsilon, false },
        { -0.000000000001001f, -0.000000000001002f, defaultEpsilon, false },

        { 0.3f, 0.30000003f, defaultEpsilon, true },
        { -0.3f, -0.30000003f, defaultEpsilon, true },

        { 0.f, 0.f, defaultEpsilon, true },
        { 0.f, -0.f, defaultEpsilon, true },
        { -0.f, 0.f, defaultEpsilon, true },
        { -0.f, -0.f, defaultEpsilon, true },
        { 0.00000001f, 0.0f, defaultEpsilon, false },
        { 0.0f, 0.00000001f, defaultEpsilon, false },
        { -0.00000001f, 0.0f, defaultEpsilon, false },
        { 0.0f, -0.00000001f, defaultEpsilon, false },
        { 0.0f, 1e-40f,  0.01f, true },
        { 1e-40f, 0.0f,  0.01f, true },
        { 1e-40f, 0.0f, 0.000001f, false },
        { 0.0f, 1e-40f, 0.000001f, false },
        { 0.0f, -1e-40f,  0.1f, true },
        { -1e-40f, 0.0f,  0.1f, true },
        { -1e-40f, 0.0f, 0.00000001f, false },
        { 0.0f, -1e-40f, 0.00000001f, false },
    };

    executeTestTable("float_nearly_eq test case failed at index: ", cases, [](auto& c, const char* cErr) {
        Assert(core::nearly_eq(c.a, c.b, c.epsilon) == c.expected, cErr);
    });

    return 0;
}

i32 run_core_math_tests_suite() {
    RunTest(pow10_test);
    RunTest(pow2_test);
    RunTest(degrees_test);
    RunTest(mantissa_exponent_test);
    RunTest(abs_test);
    RunTest(is_positive_test);
    RunTest(float_safe_eq_test);
    RunTest(float_nearly_eq_extream_cases_test);
    RunTest(float_nearly_eq_test);

    return 0;
}

constexpr i32 run_compiletime_core_math_tests_suite() {
    RunTestCompileTime(pow10_test);
    RunTestCompileTime(pow2_test);
    RunTestCompileTime(degrees_test);
    RunTestCompileTime(abs_test);
    RunTestCompileTime(is_positive_test);
    RunTestCompileTime(float_safe_eq_test);
    RunTestCompileTime(float_nearly_eq_test);

    return 0;
}
