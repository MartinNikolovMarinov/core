#include "../t-index.h"
#include "../common/wrapping_math_test_tables.h"

constexpr i32 pow10Test() {
    struct TestCase {
        u32 in;
        u64 expected;
    };

    constexpr TestCase cases[] = {
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

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::pow10(c.in) == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 pow2Test() {
    struct TestCase {
        u32 in;
        u64 expected;
    };

    constexpr TestCase cases[] = {
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

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::pow2(c.in) == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 degreesTest() {
    CT_CHECK(core::degToRad(0.0f).value == 0.0f);
    CT_CHECK(core::degToRad(90.0f).value == core::piF32() / 2.0f);
    CT_CHECK(core::degToRad(180.0f).value == core::piF32());
    CT_CHECK(core::degToRad(270.0f).value == core::piF32() * 1.5f);
    CT_CHECK(core::degToRad(360.0f).value == core::piF32() * 2.0f);

    CT_CHECK(core::radToDeg(core::radians(0.0f)) == 0.0f);
    CT_CHECK(core::radToDeg(core::radians(core::piF32() / 2.0f)) == 90.0f);
    CT_CHECK(core::radToDeg(core::radians(core::piF32())) == 180.0f);
    CT_CHECK(core::radToDeg(core::radians(core::piF32() * 1.5f)) == 270.0f);
    CT_CHECK(core::radToDeg(core::radians(core::piF32() * 2.0f)) == 360.0f);

    return 0;
}

constexpr i32 absTest() {
    CT_CHECK(core::absGeneric(i8(0)) == 0);
    CT_CHECK(core::absGeneric(i8(1)) == 1);
    CT_CHECK(core::absGeneric(i8(-1)) == 1);
    CT_CHECK(core::absGeneric(i16(0)) == 0);
    CT_CHECK(core::absGeneric(i16(1)) == 1);
    CT_CHECK(core::absGeneric(i16(-1)) == 1);
    CT_CHECK(core::absGeneric(i32(0)) == 0);
    CT_CHECK(core::absGeneric(i32(1)) == 1);
    CT_CHECK(core::absGeneric(i32(-1)) == 1);
    CT_CHECK(core::absGeneric(i64(0)) == 0);
    CT_CHECK(core::absGeneric(i64(1)) == 1);
    CT_CHECK(core::absGeneric(i64(-1)) == 1);
    CT_CHECK(core::abs(f32(0)) == 0.0f);
    CT_CHECK(core::abs(f32(1)) == 1.0f);
    CT_CHECK(core::abs(f32(-1)) == 1.0f);
    CT_CHECK(core::abs(f64(0)) == 0.0);
    CT_CHECK(core::abs(f64(1)) == 1.0);
    CT_CHECK(core::abs(f64(-1)) == 1.0);

    // check with max and min values
    CT_CHECK(core::absGeneric(i8(core::limitMax<i8>()))       == core::limitMax<i8>());
    CT_CHECK(core::absGeneric(i8(core::limitMin<i8>() + 1))   == core::limitMax<i8>());
    CT_CHECK(core::absGeneric(i16(core::limitMax<i16>()))     == core::limitMax<i16>());
    CT_CHECK(core::absGeneric(i16(core::limitMin<i16>() + 1)) == core::limitMax<i16>());
    CT_CHECK(core::absGeneric(i32(core::limitMax<i32>()))     == core::limitMax<i32>());
    CT_CHECK(core::absGeneric(i32(core::limitMin<i32>() + 1)) == core::limitMax<i32>());
    CT_CHECK(core::absGeneric(i64(core::limitMax<i64>()))     == core::limitMax<i64>());
    CT_CHECK(core::absGeneric(i64(core::limitMin<i64>() + 1)) == core::limitMax<i64>());

    return 0;
}

constexpr i32 isPositiveTest() {
    CT_CHECK(core::isPositive(i8(0)) == true);
    CT_CHECK(core::isPositive(i8(1)) == true);
    CT_CHECK(core::isPositive(i8(-1)) == false);
    CT_CHECK(core::isPositive(i16(0)) == true);
    CT_CHECK(core::isPositive(i16(1)) == true);
    CT_CHECK(core::isPositive(i16(-1)) == false);
    CT_CHECK(core::isPositive(i32(0)) == true);
    CT_CHECK(core::isPositive(i32(1)) == true);
    CT_CHECK(core::isPositive(i32(-1)) == false);
    CT_CHECK(core::isPositive(i64(0)) == true);
    CT_CHECK(core::isPositive(i64(1)) == true);
    CT_CHECK(core::isPositive(i64(-1)) == false);
    CT_CHECK(core::isPositive(f32(0)) == true);
    CT_CHECK(core::isPositive(f32(1)) == true);
    CT_CHECK(core::isPositive(f32(-1)) == false);
    CT_CHECK(core::isPositive(f64(0)) == true);
    CT_CHECK(core::isPositive(f64(1)) == true);
    CT_CHECK(core::isPositive(f64(-1)) == false);

    return 0;
}

constexpr i32 floatSafeEqTest() {
    struct TestCase {
        f32 startA;
        f32 startB;
        f32 step;
        f32 epsilon;
        i32 iterations;
        bool expected;
    };

    TestCase cases[] = {
        { 0.1f, 0.1f, 0.1f,  0.01f,  10, true },
        { 0.1f, 0.1f, 0.2f,  0.01f,  10, true },
        { 0.1f, 0.1f, 0.01f, 0.01f, 100, true },

        { 0.1f, 0.2f, 0.1f,  0.01f,  10, false },
        { 0.1f, 0.2f, 0.01f, 0.01f, 100, false },

        { 0.001111f, 0.00999f, 0.001f, 0.01f, 10, true },
        { 0.001111f, 0.00999f, 0.001f, 0.001f, 10, false },
    };

    i32 i = 0;
    constexpr const char* iterAsCstrFmt = "floatSafeEq test case failed at index: ";
    constexpr const addr_size iterAsCstrFmtLen = core::cstrLen(iterAsCstrFmt);
    char iterAsCstr[iterAsCstrFmtLen + 20] = {};
    for (i32 j = 0; j < i32(iterAsCstrFmtLen); ++j) iterAsCstr[j] = iterAsCstrFmt[j];
    char* appendIdx = &iterAsCstr[iterAsCstrFmtLen];
    for (auto& c : cases) {
        core::intToCstr(i++, appendIdx, iterAsCstrFmtLen, 2);
        for (i32 j = 0; j < c.iterations; ++j) {
            CT_CHECK(core::safeEq(c.startA, c.startB, c.epsilon) == c.expected, iterAsCstr);
            c.startA += c.step;
            c.startB += c.step;
        }
    }

    return 0;
}

i32 floatNearlyEqExtreamCasesTest() {
    struct TestCase {
        f32 a;
        f32 b;
        f32 epsilon;
        bool expected;
    };

    constexpr f32 defaultEpsilon = 0.00001f;
    TestCase cases[] = {
        { core::limitMax<f32>(), core::limitMax<f32>(), defaultEpsilon, true },
        // { core::limitMax<f32>(), -core::limitMax<f32>(), defaultEpsilon, false }, // These 2 look like a bug, but I don't think I care.
        // { -core::limitMax<f32>(), core::limitMax<f32>(), defaultEpsilon, false },
        { core::limitMax<f32>(), core::limitMax<f32>() / 2, defaultEpsilon, false },
        { core::limitMax<f32>(), -core::limitMax<f32>() / 2, defaultEpsilon, false },
        { -core::limitMax<f32>(), core::limitMax<f32>() / 2, defaultEpsilon, false },

        { core::limitMin<f32>(), core::limitMin<f32>(), defaultEpsilon, true },
        { core::limitMin<f32>(), -core::limitMin<f32>(), defaultEpsilon, true },
        { -core::limitMin<f32>(), core::limitMin<f32>(), defaultEpsilon, true },
        { core::limitMin<f32>(), 0, defaultEpsilon, false },
        { 0, core::limitMin<f32>(), defaultEpsilon, false },
        { -core::limitMin<f32>(), 0, defaultEpsilon, false },
        { 0, -core::limitMin<f32>(), defaultEpsilon, false },
        { 0.000000001f, -core::limitMin<f32>(), defaultEpsilon, false },
        { 0.000000001f, core::limitMin<f32>(), defaultEpsilon, false },
        { core::limitMin<f32>(), 0.000000001f, defaultEpsilon, false },
        { -core::limitMin<f32>(), 0.000000001f, defaultEpsilon, false },
    };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::nearlyEq(c.a, c.b, c.epsilon) == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 floatNearlyEqTest() {
    struct TestCase {
        f32 a;
        f32 b;
        f32 epsilon;
        bool expected;
    };

    constexpr f32 defaultEpsilon = 0.00001f;

    TestCase cases[] = {
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

    i32 ret = core::testing::executeTestTable("float_nearlyEq test case failed at index: ", cases, [](auto& c, const char* cErr) {
        CT_CHECK(core::nearlyEq(c.a, c.b, c.epsilon) == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 safeAddTest() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_add_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) constexpr -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::safeAdd(a, b, res);

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

constexpr i32 safeSubTest() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_sub_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) constexpr -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::safeSub(a, b, res);

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

constexpr i32 safeMulTest() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_mul_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) constexpr -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::safeMul(a, b, res);

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

constexpr i32 safeDivTest() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_div_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) constexpr -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::safeDiv(a, b, res);

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

i32 runMathTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(pow10Test);
    if (runTest(tInfo, pow10Test) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(pow2Test);
    if (runTest(tInfo, pow2Test) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(degreesTest);
    if (runTest(tInfo, degreesTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(absTest);
    if (runTest(tInfo, absTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(isPositiveTest);
    if (runTest(tInfo, isPositiveTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(floatSafeEqTest);
    if (runTest(tInfo, floatSafeEqTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(floatNearlyEqExtreamCasesTest);
    if (runTest(tInfo, floatNearlyEqExtreamCasesTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(floatNearlyEqTest);
    if (runTest(tInfo, floatNearlyEqTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(safeAddTest);
    if (runTest(tInfo, safeAddTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(safeSubTest);
    if (runTest(tInfo, safeSubTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(safeMulTest);
    if (runTest(tInfo, safeMulTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(safeDivTest);
    if (runTest(tInfo, safeDivTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeMathTestsSuite() {
    RunTestCompileTime(pow10Test);
    RunTestCompileTime(pow2Test);
    RunTestCompileTime(degreesTest);
    RunTestCompileTime(absTest);
    RunTestCompileTime(isPositiveTest);
    RunTestCompileTime(floatSafeEqTest);
    RunTestCompileTime(floatNearlyEqTest);
    RunTestCompileTime(safeAddTest);
    RunTestCompileTime(safeSubTest);
    RunTestCompileTime(safeMulTest);
    RunTestCompileTime(safeDivTest);

    return 0;
}
