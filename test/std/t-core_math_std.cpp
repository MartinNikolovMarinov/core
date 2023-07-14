// Running tests for more complex math functions requires a check with the standard library implementation. It is also
// required to keep one's sanity.

i32 fp_classifications_test() {
    {
        struct TestCase {
            f32 a;
            i32 expected;
        };

        constexpr f32 infinity = std::numeric_limits<f32>::infinity();
        constexpr f32 nan = std::numeric_limits<f32>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 0.0, FP_ZERO },
            { 1.5, FP_NORMAL },
            { -2.75, FP_NORMAL },
            { 1.0e-38f, FP_SUBNORMAL },
            { infinity, FP_INFINITE },
            { -infinity, FP_INFINITE },
            { 1.0, FP_NORMAL },
            { -3.0, FP_NORMAL },
            { nan, FP_NAN },
        };

        executeTestTable("fp_classifications test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::fpclassify(c.a);
            auto stdGot = std::fpclassify(c.a);
            Assert(i32(got) == c.expected, cErr);
            Assert(i32(stdGot) == i32(got), cErr);
        });
    }

    {
        struct TestCase {
            f64 a;
            i32 expected;
        };

        constexpr f64 infinity = std::numeric_limits<f64>::infinity();
        constexpr f64 nan = std::numeric_limits<f64>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 0.0, FP_ZERO },
            { 1.5, FP_NORMAL },
            { -2.75, FP_NORMAL },
            { 1.0e-308, FP_SUBNORMAL },
            { infinity, FP_INFINITE },
            { -infinity, FP_INFINITE },
            { 1.0, FP_NORMAL },
            { -3.0, FP_NORMAL },
            { nan, FP_NAN },
        };

        executeTestTable("fp_classifications test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::fpclassify(c.a);
            auto stdGot = std::fpclassify(c.a);
            Assert(i32(got) == c.expected, cErr);
            Assert(i32(stdGot) == i32(got), cErr);
        });
    }

    return 0;
}

i32 round_test() {
    {
        struct TestCase {
            f32 a;
            f32 expected;
        };

        constexpr f32 infinity = std::numeric_limits<f32>::infinity();
        constexpr f32 nan = std::numeric_limits<f32>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 3.7f, 4.0f },
            { -2.5f, -3.0f }, // Rounding to the next integer in the negative direction!
            { 0.0f, 0.0f },
            { 10.9f, 11.0f },
            { -7.2f, -7.0f },
            { 2.0f, 2.0f },
            { 6.5f, 7.0f },
            { -9.8f, -10.0f },
            { 15.3f, 15.0f },
            { -4.0f, -4.0f },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("floor test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::round(c.a);
            auto fromStd = std::round(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::round(nan)));
        Assert(std::isnan(std::round(nan)));
    }

    {
        struct TestCase {
            f64 a;
            f64 expected;
        };

        constexpr f64 infinity = std::numeric_limits<f64>::infinity();
        constexpr f64 nan = std::numeric_limits<f64>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 3.7, 4.0 },
            { -2.5, -3.0 }, // Rounding to the next integer in the negative direction!
            { 0.0, 0.0 },
            { 10.9, 11.0 },
            { -7.2, -7.0 },
            { 2.0, 2.0 },
            { 6.5, 7.0 },
            { -9.8, -10.0 },
            { 15.3, 15.0 },
            { -4.0, -4.0 },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("floor test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::round(c.a);
            auto fromStd = std::round(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::round(nan)));
        Assert(std::isnan(std::round(nan)));
    }

    return 0;
}

i32 floor_test() {
    {
        struct TestCase {
            f32 a;
            f32 expected;
        };

        constexpr f32 infinity = std::numeric_limits<f32>::infinity();
        constexpr f32 nan = std::numeric_limits<f32>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 3.7, 3.0 },
            { -2.5, -3.0 },
            { 0.0, 0.0 },
            { 10.9, 10.0 },
            { -7.2, -8.0 },
            { 2.0, 2.0 },
            { 6.5, 6.0 },
            { -9.8, -10.0 },
            { 15.3, 15.0 },
            { -4.0, -4.0 },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("floor test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::floor(c.a);
            auto fromStd = std::floor(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::floor(nan)));
        Assert(core::isnan(core::floor(nan)));
    }

    {
        struct TestCase {
            f64 a;
            f64 expected;
        };

        constexpr f64 infinity = std::numeric_limits<f64>::infinity();
        constexpr f64 nan = std::numeric_limits<f64>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 3.7, 3.0 },
            { -2.5, -3.0 },
            { 0.0, 0.0 },
            { 10.9, 10.0 },
            { -7.2, -8.0 },
            { 2.0, 2.0 },
            { 6.5, 6.0 },
            { -9.8, -10.0 },
            { 15.3, 15.0 },
            { -4.0, -4.0 },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("floor test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::floor(c.a);
            auto fromStd = std::floor(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::floor(nan)));
        Assert(core::isnan(core::floor(nan)));
    }

    return 0;
}

i32 ceil_test() {
    {
        struct TestCase {
            f32 a;
            f32 expected;
        };

        constexpr f32 infinity = std::numeric_limits<f32>::infinity();
        constexpr f32 nan = std::numeric_limits<f32>::quiet_NaN();

        constexpr TestCase cases[] = {
            { 3.7f, 4.0f },
            { -2.5f, -2.0f },
            { 0.0f, 0.0f },
            { 10.9f, 11.0f },
            { -7.2f, -7.0f },
            { 2.0f, 2.0f },
            { 6.5f, 7.0f },
            { -9.8f, -9.0f },
            { 15.3f, 16.0f },
            { -4.0f, -4.0f },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("floor test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::ceil(c.a);
            auto fromStd = std::ceil(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::ceil(nan)));
        Assert(core::isnan(core::ceil(nan)));
    }

    {
        struct TestCase {
            f64 a;
            f64 expected;
        };

        constexpr f64 infinity = std::numeric_limits<f64>::infinity();
        constexpr f64 nan = std::numeric_limits<f64>::quiet_NaN();

        constexpr TestCase cases[] = {
           { 3.7f, 4.0f },
           { -2.5f, -2.0f },
           { 0.0f, 0.0f },
           { 10.9f, 11.0f },
           { -7.2f, -7.0f },
           { 2.0f, 2.0f },
           { 6.5f, 7.0f },
           { -9.8f, -9.0f },
           { 15.3f, 16.0f },
           { -4.0f, -4.0f },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("ceil test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::ceil(c.a);
            auto fromStd = std::ceil(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::ceil(nan)));
        Assert(core::isnan(core::ceil(nan)));
    }

    return 0;
}

i32 min_max_test() {
    auto specialCompare = [](auto a, auto b) -> bool {
        if (core::isnan(a)) return core::isnan(b);
        if (core::isnan(b)) return false;
        return a == b;
    };

    {
        struct TestCase {
            f32 a;
            f32 b;
            f32 expectedMax;
            f32 expectedMin;
        };

        constexpr TestCase cases[] = {
            { 3.14, 2.71, 3.14, 2.71 },
            { -5.0, -10.0, -5.0, -10.0 },
            { 0.0, 0.0, 0.0, 0.0 },
            { 0.0, -0.0, 0.0, 0.0 },
            { -0.0, 0.0, 0.0, 0.0 },
        };

        executeTestTable("min max clamp test case for f32 failed at index: ", cases, [&](auto& c, const char* cErr) {
            {
                auto got = core::min(c.a, c.b);
                auto fromStd = std::min(c.a, c.b);
                Assert(specialCompare(got, fromStd), cErr);
                Assert(specialCompare(got, c.expectedMin), cErr);
            }
            {
                auto got = core::max(c.a, c.b);
                auto fromStd = std::max(c.a, c.b);
                Assert(specialCompare(got, fromStd), cErr);
                Assert(specialCompare(got, c.expectedMax), cErr);
            }
            {
                auto [gotMin, gotMax] = core::minmax(c.a, c.b);
                Assert(specialCompare(gotMin, c.expectedMin), cErr);
                Assert(specialCompare(gotMax, c.expectedMax), cErr);
            }
        });
    }

    {
        struct TestCase {
            f64 a;
            f64 b;
            f64 expectedMax;
            f64 expectedMin;
        };

        constexpr TestCase cases[] = {
            { 3.14, 2.71, 3.14, 2.71 },
            { -5.0, -10.0, -5.0, -10.0 },
            { 0.0, 0.0, 0.0, 0.0 },
            { 0.0, -0.0, 0.0, 0.0 },
            { -0.0, 0.0, 0.0, 0.0 },
        };

        executeTestTable("min max clamp test case for f64 failed at index: ", cases, [&](auto& c, const char* cErr) {
            {
                auto got = core::min(c.a, c.b);
                auto fromStd = std::min(c.a, c.b);
                Assert(specialCompare(got, fromStd), cErr);
                Assert(specialCompare(got, c.expectedMin), cErr);
            }
            {
                auto got = core::max(c.a, c.b);
                auto fromStd = std::max(c.a, c.b);
                Assert(specialCompare(got, fromStd), cErr);
                Assert(specialCompare(got, c.expectedMax), cErr);
            }
            {
                auto [gotMin, gotMax] = core::minmax(c.a, c.b);
                Assert(specialCompare(gotMin, c.expectedMin), cErr);
                Assert(specialCompare(gotMax, c.expectedMax), cErr);
            }
        });
    }

    return 0;
}

i32 run_core_math_tests_suite_std() {
    RunTest(fp_classifications_test);
    RunTest(round_test);
    RunTest(floor_test);
    RunTest(ceil_test);
    RunTest(min_max_test);

    return 0;
}
