constexpr void degrees_test() {
    Assert(core::deg_to_rad(0.0f) == 0.0f);
    Assert(core::deg_to_rad(90.0f) == core::PI / 2.0f);
    Assert(core::deg_to_rad(180.0f) == core::PI);
    Assert(core::deg_to_rad(270.0f) == core::PI * 1.5f);
    Assert(core::deg_to_rad(360.0f) == core::PI * 2.0f);

    Assert(core::rad_to_deg(0.0f) == 0.0f);
    Assert(core::rad_to_deg(core::PI / 2.0f) == 90.0f);
    Assert(core::rad_to_deg(core::PI) == 180.0f);
    Assert(core::rad_to_deg(core::PI * 1.5f) == 270.0f);
    Assert(core::rad_to_deg(core::PI * 2.0f) == 360.0f);
}

constexpr void pow_u64_test() {
    Assert(core::pow_u64(0, 0) == 0);
    Assert(core::pow_u64(0, 5) == 0);
    Assert(core::pow_u64(5, 0) == 1);
    Assert(core::pow_u64(5, 1) == 5);
    Assert(core::pow_u64(5, 2) == 25);
    Assert(core::pow_u64(5, 3) == 125);
    Assert(core::pow_u64(5, 4) == 625);
    Assert(core::pow_u64(5, 5) == 3125);
    Assert(core::pow_u64(5, 6) == 15625);
    Assert(core::pow_u64(5, 7) == 78125);
    Assert(core::pow_u64(5, 8) == 390625);
    Assert(core::pow_u64(5, 9) == 1953125);
    Assert(core::pow_u64(5, 10) == 9765625);
    Assert(core::pow_u64(5, 11) == 48828125);
    Assert(core::pow_u64(5, 12) == 244140625);
    Assert(core::pow_u64(5, 13) == 1220703125);
    Assert(core::pow_u64(5, 14) == 6103515625);
    Assert(core::pow_u64(5, 15) == 30517578125);
    Assert(core::pow_u64(5, 16) == 152587890625);
    Assert(core::pow_u64(5, 17) == 762939453125);
    Assert(core::pow_u64(5, 18) == 3814697265625);
    Assert(core::pow_u64(5, 19) == 19073486328125);
    Assert(core::pow_u64(5, 20) == 95367431640625);
    Assert(core::pow_u64(5, 21) == 476837158203125);
    Assert(core::pow_u64(5, 22) == 2384185791015625);
    Assert(core::pow_u64(5, 23) == 11920928955078125);
    Assert(core::pow_u64(5, 24) == 59604644775390625);
    Assert(core::pow_u64(5, 25) == 298023223876953125);
    Assert(core::pow_u64(5, 26) == 1490116119384765625);
    Assert(core::pow_u64(5, 27) == 7450580596923828125);
    // next one is too big for 64-bit int

    Assert(core::pow_u64(2, 0) == 1);
    Assert(core::pow_u64(2, 1) == 2);
    Assert(core::pow_u64(2, 2) == 4);
    Assert(core::pow_u64(2, 3) == 8);
    Assert(core::pow_u64(2, 4) == 16);
    Assert(core::pow_u64(2, 5) == 32);
    Assert(core::pow_u64(2, 6) == 64);
    Assert(core::pow_u64(2, 7) == 128);
    Assert(core::pow_u64(2, 8) == 256);
    Assert(core::pow_u64(2, 9) == 512);
    Assert(core::pow_u64(2, 10) == 1024);
    Assert(core::pow_u64(2, 11) == 2048);
    Assert(core::pow_u64(2, 12) == 4096);
    Assert(core::pow_u64(2, 13) == 8192);
    Assert(core::pow_u64(2, 14) == 16384);
    Assert(core::pow_u64(2, 15) == 32768);
    Assert(core::pow_u64(2, 16) == 65536);
    Assert(core::pow_u64(2, 17) == 131072);
    Assert(core::pow_u64(2, 18) == 262144);
    Assert(core::pow_u64(2, 19) == 524288);
    Assert(core::pow_u64(2, 20) == 1048576);
    Assert(core::pow_u64(2, 21) == 2097152);
    Assert(core::pow_u64(2, 22) == 4194304);
    Assert(core::pow_u64(2, 23) == 8388608);
    Assert(core::pow_u64(2, 24) == 16777216);
    Assert(core::pow_u64(2, 25) == 33554432);
    Assert(core::pow_u64(2, 26) == 67108864);
    Assert(core::pow_u64(2, 27) == 134217728);
    Assert(core::pow_u64(2, 28) == 268435456);
    Assert(core::pow_u64(2, 29) == 536870912);
    Assert(core::pow_u64(2, 30) == 1073741824);
    Assert(core::pow_u64(2, 31) == 2147483648);
    Assert(core::pow_u64(2, 32) == 4294967296);
    Assert(core::pow_u64(2, 33) == 8589934592);
    Assert(core::pow_u64(2, 34) == 17179869184);
    Assert(core::pow_u64(2, 35) == 34359738368);
    Assert(core::pow_u64(2, 36) == 68719476736);
    Assert(core::pow_u64(2, 37) == 137438953472);
    Assert(core::pow_u64(2, 38) == 274877906944);
    Assert(core::pow_u64(2, 39) == 549755813888);
    Assert(core::pow_u64(2, 40) == 1099511627776);
    Assert(core::pow_u64(2, 41) == 2199023255552);
    Assert(core::pow_u64(2, 42) == 4398046511104);
    Assert(core::pow_u64(2, 43) == 8796093022208);
    Assert(core::pow_u64(2, 44) == 17592186044416);
    Assert(core::pow_u64(2, 45) == 35184372088832);
    Assert(core::pow_u64(2, 46) == 70368744177664);
    Assert(core::pow_u64(2, 47) == 140737488355328);
    Assert(core::pow_u64(2, 48) == 281474976710656);
    Assert(core::pow_u64(2, 49) == 562949953421312);
    Assert(core::pow_u64(2, 50) == 1125899906842624);
    Assert(core::pow_u64(2, 51) == 2251799813685248);
    Assert(core::pow_u64(2, 52) == 4503599627370496);
    Assert(core::pow_u64(2, 53) == 9007199254740992);
    Assert(core::pow_u64(2, 54) == 18014398509481984);
    Assert(core::pow_u64(2, 55) == 36028797018963968);
    Assert(core::pow_u64(2, 56) == 72057594037927936);
    Assert(core::pow_u64(2, 57) == 144115188075855872);
    Assert(core::pow_u64(2, 58) == 288230376151711744);
    Assert(core::pow_u64(2, 59) == 576460752303423488);
    Assert(core::pow_u64(2, 60) == 1152921504606846976);
    Assert(core::pow_u64(2, 61) == 2305843009213693952);
    Assert(core::pow_u64(2, 62) == 4611686018427387904);
    // next one is too big for 64-bit int
}

constexpr void abs_test() {
    Assert(core::abs(i8(0)) == 0);
    Assert(core::abs(i8(1)) == 1);
    Assert(core::abs(i8(-1)) == 1);
    Assert(core::abs(i16(0)) == 0);
    Assert(core::abs(i16(1)) == 1);
    Assert(core::abs(i16(-1)) == 1);
    Assert(core::abs(i32(0)) == 0);
    Assert(core::abs(i32(1)) == 1);
    Assert(core::abs(i32(-1)) == 1);
    Assert(core::abs(i64(0)) == 0);
    Assert(core::abs(i64(1)) == 1);
    Assert(core::abs(i64(-1)) == 1);
    Assert(core::abs(f32(0)) == 0.0f);
    Assert(core::abs(f32(1)) == 1.0f);
    Assert(core::abs(f32(-1)) == 1.0f);
    Assert(core::abs(f64(0)) == 0.0);
    Assert(core::abs(f64(1)) == 1.0);
    Assert(core::abs(f64(-1)) == 1.0);

    // check with max and min values
    Assert(core::abs(i8(core::MAX_I8))       == core::MAX_I8);
    Assert(core::abs(i8(core::MIN_I8 + 1))   == core::MAX_I8);
    Assert(core::abs(i16(core::MAX_I16))     == core::MAX_I16);
    Assert(core::abs(i16(core::MIN_I16 + 1)) == core::MAX_I16);
    Assert(core::abs(i32(core::MAX_I32))     == core::MAX_I32);
    Assert(core::abs(i32(core::MIN_I32 + 1)) == core::MAX_I32);
    Assert(core::abs(i64(core::MAX_I64))     == core::MAX_I64);
    Assert(core::abs(i64(core::MIN_I64 + 1)) == core::MAX_I64);
}

constexpr void is_positive_test() {
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
}

constexpr void float_safe_eq() {
    struct test_case {
        f32 startA;
        f32 startB;
        f32 step;
        f32 epsilon;
        i32 iterations;
        bool expected;
    };

    test_case cases[] = {
        { 0.1, 0.1, 0.1,  0.01,  10, true },
        { 0.1, 0.1, 0.2,  0.01,  10, true },
        { 0.1, 0.1, 0.01, 0.01, 100, true },

        { 0.1, 0.2, 0.1,  0.01,  10, false },
        { 0.1, 0.2, 0.01, 0.01, 100, false },

        { 0.001111, 0.00999, 0.001, 0.01, 10, true },
        { 0.001111, 0.00999, 0.001, 0.001, 10, false },
    };

    i32 i = 0;
    constexpr const char* iterAsCptrFmt = "float_safe_eq test case failed at index: ";
    constexpr const ptr_size iterAsCptrFmtLen = core::cptr_len(iterAsCptrFmt);
    char iterAsCptr[iterAsCptrFmtLen + 20] = {};
    core::memcopy(iterAsCptr, iterAsCptrFmt, iterAsCptrFmtLen);
    char* appendIdx = &iterAsCptr[iterAsCptrFmtLen];
    for (auto& c : cases) {
        core::int_to_cptr(i++, appendIdx, 2);
        for (i32 j = 0; j < c.iterations; ++j) {
            Assert(core::safe_eq(c.startA, c.startB, c.epsilon) == c.expected, iterAsCptr);
            c.startA += c.step;
            c.startB += c.step;
        }
    }
}

void run_core_math_tests_suite() {
    RunTest(degrees_test);
    RunTest(pow_u64_test);
    RunTest(abs_test);
    RunTest(is_positive_test);
    RunTest(float_safe_eq);
}
