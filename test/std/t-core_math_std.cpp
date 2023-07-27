i32 min_max_compare_with_std_test() {
    auto specialCompare = [](auto a, auto b) -> bool {
        if (core::isnan(a)) return core::isnan(b);
        if (core::isnan(b)) return false;
        return a == b;
    };

    {
        struct test_case {
            f32 a;
            f32 b;
            f32 expectedMax;
            f32 expectedMin;
        };

        constexpr test_case cases[] = {
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
        struct test_case {
            f64 a;
            f64 b;
            f64 expectedMax;
            f64 expectedMin;
        };

        constexpr test_case cases[] = {
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

i32 check_system_limits_with_std_test() {
    Assert(core::limit_min<i8>() == std::numeric_limits<i8>::min());
    Assert(core::limit_max<i8>() == std::numeric_limits<i8>::max());
    Assert(core::limit_min<u8>() == std::numeric_limits<u8>::min());
    Assert(core::limit_max<u8>() == std::numeric_limits<u8>::max());
    Assert(core::limit_min<i16>() == std::numeric_limits<i16>::min());
    Assert(core::limit_max<i16>() == std::numeric_limits<i16>::max());
    Assert(core::limit_min<u16>() == std::numeric_limits<u16>::min());
    Assert(core::limit_max<u16>() == std::numeric_limits<u16>::max());
    Assert(core::limit_min<i32>() == std::numeric_limits<i32>::min());
    Assert(core::limit_max<i32>() == std::numeric_limits<i32>::max());
    Assert(core::limit_min<u32>() == std::numeric_limits<u32>::min());
    Assert(core::limit_max<u32>() == std::numeric_limits<u32>::max());
    Assert(core::limit_min<i64>() == std::numeric_limits<i64>::min());
    Assert(core::limit_max<i64>() == std::numeric_limits<i64>::max());
    Assert(core::limit_min<u64>() == std::numeric_limits<u64>::min());
    Assert(core::limit_max<u64>() == std::numeric_limits<u64>::max());
    Assert(core::limit_min<f32>() == std::numeric_limits<f32>::min());
    Assert(core::limit_max<f32>() == std::numeric_limits<f32>::max());
    Assert(core::limit_min<f64>() == std::numeric_limits<f64>::min());
    Assert(core::limit_max<f64>() == std::numeric_limits<f64>::max());

    return 0;
}

i32 isinf_isnan_isnormal_cc_impl_compare_with_std_test() {
    auto inf = core::infinity_f32();
    auto nan = core::quiet_NaN_f32();
    auto signalingNan = core::signaling_NaN_f32();
    auto stdinf = std::numeric_limits<f32>::infinity();
    auto stdnan = std::numeric_limits<f32>::quiet_NaN();
    auto stdsignalingNan = std::numeric_limits<f32>::signaling_NaN();

    // Infinity tests
    Assert(std::isinf(inf));
    Assert(std::isinf(-inf));
    Assert(core::isinf(inf));
    Assert(core::isinf(-inf));
    Assert(core::isinf(stdinf));
    Assert(core::isinf(-stdinf));
    Assert(inf == stdinf);
    Assert(-inf == -stdinf);

    // NaN tests
    Assert(std::isnan(nan) == core::isnan(nan));
    Assert(std::isnan(signalingNan) == core::isnan(signalingNan));
    Assert(core::isnan(stdnan));
    Assert(core::isnan(stdsignalingNan));

    // Normal tests
    Assert(std::isnormal(1.0f) == core::isnormal(1.0f));
    Assert(std::isnormal(-1.0f) == core::isnormal(-1.0f));
    Assert(std::isnormal(0.0f) == core::isnormal(0.0f));

    Assert(std::isnormal(std::numeric_limits<f32>::min()) == core::isnormal(std::numeric_limits<f32>::min()));
    Assert(std::isnormal(core::limit_min<f32>()) == core::isnormal(core::limit_min<f32>()));
    Assert(std::isnormal(-std::numeric_limits<f32>::min()) == core::isnormal(-std::numeric_limits<f32>::min()));
    Assert(std::isnormal(-core::limit_min<f32>()) == core::isnormal(-core::limit_min<f32>()));

    Assert(std::isnormal(std::numeric_limits<f32>::max()) == core::isnormal(std::numeric_limits<f32>::max()));
    Assert(std::isnormal(core::limit_max<f32>()) == core::isnormal(core::limit_max<f32>()));
    Assert(std::isnormal(-std::numeric_limits<f32>::max()) == core::isnormal(-std::numeric_limits<f32>::max()));
    Assert(std::isnormal(-core::limit_max<f32>()) == core::isnormal(-core::limit_max<f32>()));

    return 0;
}

i32 fp_classify_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            i32 expected;
        };

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        constexpr test_case cases[] = {
            { 0.0f, FP_ZERO },
            { 1.5f, FP_NORMAL },
            { -2.75, FP_NORMAL },
            { 1.0e-38f, FP_SUBNORMAL },
            { infinity, FP_INFINITE },
            { -infinity, FP_INFINITE },
            { 1.0f, FP_NORMAL },
            { -3.0f, FP_NORMAL },
            { nan, FP_NAN },
        };

        executeTestTable("fp_classifications test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::fpclassify_compiletime_impl(c.a);
            auto stdGot = std::fpclassify(c.a);
            Assert(i32(got) == c.expected, cErr);
            Assert(i32(stdGot) == i32(got), cErr);
        });
    }

    {
        struct test_case {
            f64 a;
            i32 expected;
        };

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        constexpr test_case cases[] = {
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
            auto got = core::detail::fpclassify_compiletime_impl(c.a);
            auto stdGot = std::fpclassify(c.a);
            Assert(i32(got) == c.expected, cErr);
            Assert(i32(stdGot) == i32(got), cErr);
        });
    }

    return 0;
}

i32 floor_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            f32 expected;
        };

        constexpr test_case cases[] = {
            { 3.7, 3.0 },
            { 3.999, 3.0 },
            { -2.5, -3.0 },
            { -2.00001, -3.0 },
            { 0.0, 0.0 },
            { 0.999, 0.0 },
            { -0.999, -1.0 },
            { 10.9, 10.0 },
            { -7.2, -8.0 },
            { 2.0, 2.0 },
            { 6.5, 6.0 },
            { -9.9999, -10.0 },
            { 15.3, 15.0 },
            { -4.0, -4.0 },
        };

        executeTestTable("floor test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::floor_compiletime_impl(c.a);
            Assert(got == c.expected, cErr);
            auto gotstd = std::floor(c.a);
            Assert(gotstd == c.expected, cErr);
        });

        // Check for NaN and infinity

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        Assert(core::isnan(core::detail::floor_compiletime_impl(nan)));
        Assert(core::isinf(core::detail::floor_compiletime_impl(infinity)));
        Assert(core::isinf(core::detail::floor_compiletime_impl(-infinity)));
    }

    {
        struct test_case {
            f64 a;
            f64 expected;
        };

        constexpr test_case cases[] = {
            { 3.7, 3.0 },
            { 3.999999, 3.0 },
            { -2.5, -3.0 },
            { -2.0000001, -3.0 },
            { 0.0, 0.0 },
            { 0.999999, 0.0 },
            { -0.999999, -1.0 },
            { 10.9, 10.0 },
            { -7.2, -8.0 },
            { 2.0, 2.0 },
            { 6.5, 6.0 },
            { -9.999999, -10.0 },
            { 15.3, 15.0 },
            { -4.0, -4.0 },
        };

        executeTestTable("floor test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::floor_compiletime_impl(c.a);
            Assert(got == c.expected, cErr);
            auto gotstd = std::floor(c.a);
            Assert(gotstd == c.expected, cErr);
        });

        // Check for NaN and infinity

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        Assert(core::isnan(core::detail::floor_compiletime_impl(nan)));
        Assert(core::detail::floor_compiletime_impl(infinity) == infinity);
        Assert(core::detail::floor_compiletime_impl(-infinity) == -infinity);
    }

    return 0;
}

i32 ceil_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            f32 expected;
        };

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        constexpr test_case cases[] = {
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

        executeTestTable("ceil test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::ceil_compiletime_impl(c.a);
            auto fromStd = std::ceil(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::detail::ceil_compiletime_impl(nan)));
        Assert(core::isnan(core::detail::ceil_compiletime_impl(nan)));
    }

    {
        struct test_case {
            f64 a;
            f64 expected;
        };

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        constexpr test_case cases[] = {
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
            auto got = core::detail::ceil_compiletime_impl(c.a);
            auto fromStd = std::ceil(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(std::isnan(core::detail::ceil_compiletime_impl(nan)));
        Assert(core::isnan(core::detail::ceil_compiletime_impl(nan)));
    }

    return 0;
}

i32 sqrt_cc_impl_compare_with_std_test() {
    // Differnet implementations of sqrt are not guaranteed to be exact, even across different standard libraries, so I
    // just check that they are close enough.

    for (f32 i = -100.0f; i < 100.0f; i += 0.1234f) {
        auto got = core::detail::sqrt_compiletime_impl(i);
        auto fromStd = std::sqrt(i);
        if (i < 0) {
            Assert(core::isnan(got));
            Assert(std::isnan(fromStd));
        } else {
            Assert(core::safe_eq(got, fromStd, 0.000001f));
        }
    }

    for (f64 i = -1000.0; i < 1000.0; i += 0.1234) {
        auto got = core::detail::sqrt_compiletime_impl(i);
        auto fromStd = std::sqrt(i);
        if (i < 0) {
            Assert(core::isnan(got));
            Assert(std::isnan(fromStd));
        } else {
            Assert(core::safe_eq(got, fromStd, 0.0000000001));
        }
    }

    return 0;
}

i32 round_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            f32 expected;
        };

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        constexpr test_case cases[] = {
            { 0.f, 0.f },

            { 0.00001, 0.f },
            { 0.49999, 0.f },
            { 0.5, 1.f },

            { 7.00001, 7.f },
            { 7.49999, 7.f },
            { 7.5, 8.f },

            { -0.000009, 0.f },
            { -0.4999999, 0.f },
            { -0.5, -1.f },

            { -7.00001, -7.f },
            { -7.49999, -7.f },
            { -7.5, -8.f },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("round test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::round_compiletime_impl(c.a);
            auto fromStd = std::round(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::round(nan)));
        Assert(std::isnan(std::round(nan)));
    }

    {
        struct test_case {
            f64 a;
            f64 expected;
        };

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        constexpr test_case cases[] = {
            { 0, 0 },

            { 0.00000001, 0 },
            { 0.49999999, 0 },
            { 0.5, 1 },

            { 7.00000001, 7 },
            { 7.49999999, 7 },
            { 7.5, 8 },

            { -0.00000001, 0 },
            { -0.49999999, 0 },
            { -0.5, -1 },

            { -7.00000001, -7 },
            { -7.49999999, -7 },
            { -7.5, -8 },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("round test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::round_compiletime_impl(c.a);
            auto fromStd = std::round(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::round(nan)));
        Assert(std::isnan(std::round(nan)));
    }

    return 0;
}

i32 trunc_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            f32 expected;
        };

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        constexpr test_case cases[] = {
            { 0.f, 0.f },

            { 0.00001, 0.f },
            { 0.49999, 0.f },
            { 0.5, 0.f },

            { 7.00001, 7.f },
            { 7.49999, 7.f },
            { 7.5, 7.f },

            { -0.000009, 0.f },
            { -0.4999999, 0.f },
            { -0.5, 0.f },

            { -7.00001, -7.f },
            { -7.49999, -7.f },
            { -7.5, -7.f },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("trunc test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::trunc_compiletime_impl(c.a);
            auto fromStd = std::trunc(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::trunc(nan)));
        Assert(std::isnan(std::trunc(nan)));
    }

    {
        struct test_case {
            f64 a;
            f64 expected;
        };

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        constexpr test_case cases[] = {
            { 0, 0 },

            { 0.00000001, 0 },
            { 0.49999999, 0 },
            { 0.5, 0 },

            { 7.00000001, 7 },
            { 7.49999999, 7 },
            { 7.5, 7 },

            { -0.00000001, 0 },
            { -0.49999999, 0 },
            { -0.5, 0 },

            { -7.00000001, -7 },
            { -7.49999999, -7 },
            { -7.5, -7 },

            { infinity, infinity },
            { -infinity, -infinity },
        };

        executeTestTable("trunc test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            auto got = core::detail::trunc_compiletime_impl(c.a);
            auto fromStd = std::trunc(c.a);
            Assert(got == fromStd, cErr);
            Assert(got == c.expected, cErr);
        });

        Assert(core::isnan(core::trunc(nan)));
        Assert(std::isnan(std::trunc(nan)));
    }

    return 0;
}

i32 modf_cc_impl_compare_with_std_test() {
    {
        struct test_case {
            f32 a;
            f32 expectedInt;
            f32 expectedFrac;
        };

        constexpr f32 nan = core::quiet_NaN_f32();
        constexpr f32 infinity = core::infinity_f32();

        constexpr test_case cases[] = {
            { 0.f, 0.f, 0.f },

            { 0.00001, 0.f, 0.00001f },
            { 0.49999, 0.f, 0.49999f },
            { 0.5, 0.f, 0.5f },

            { 7.00001, 7.f, 0.00001f },
            { 7.49999, 7.f, 0.49999f },
            { 7.5, 7.f, 0.5f },

            { -0.000009, 0.f, -0.000009f },
            { -0.4999999, 0.f, -0.4999999f },
            { -0.5, 0.f, -0.5f },

            { -7.00001, -7.f, -0.00001f },
            { -7.49999, -7.f, -0.49999f },
            { -7.5, -7.f, -0.5f },

            { infinity, infinity, 0.f },
            { -infinity, -infinity, 0.f },
        };

        executeTestTable("modf test case for f32 failed at index: ", cases, [](auto& c, const char* cErr) {
            f32 intPart = 0.f;
            f32 got = core::detail::modf_compiletime_impl(c.a, &intPart);
            f32 fromStdIntPart = 0.f;
            f32 fromStd = std::modf(c.a, &fromStdIntPart);
            Assert(core::safe_eq(got, fromStd, 0.00001f), cErr);
            Assert(core::safe_eq(got, c.expectedFrac, 0.00001f), cErr);
            if (!core::isinf(intPart)) {
                Assert(core::safe_eq(intPart, fromStdIntPart, 0.00001f), cErr);
                Assert(core::safe_eq(intPart, c.expectedInt, 0.00001f), cErr);
            }
            else {
                Assert(core::isinf(fromStdIntPart), cErr);
                Assert(core::isinf(c.expectedInt), cErr);
            }
        });

        f32 intPart = 0.f;
        Assert(core::isnan(core::modf(nan, &intPart)));
        Assert(std::isnan(std::modf(nan, &intPart)));
    }

    {
        struct test_case {
            f64 a;
            f64 expectedInt;
            f64 expectedFrac;
        };

        constexpr f64 nan = core::quiet_NaN_f64();
        constexpr f64 infinity = core::infinity_f64();

        constexpr test_case cases[] = {
            { 0, 0, 0 },

            { 0.00000001, 0, 0.00000001 },
            { 0.49999999, 0, 0.49999999 },
            { 0.5, 0, 0.5 },

            { 7.00000001, 7, 0.00000001 },
            { 7.49999999, 7, 0.49999999 },
            { 7.5, 7, 0.5 },

            { -0.00000001, 0, -0.00000001 },
            { -0.49999999, 0, -0.49999999 },
            { -0.5, 0, -0.5 },

            { -7.00000001, -7, -0.00000001 },
            { -7.49999999, -7, -0.49999999 },
            { -7.5, -7, -0.5 },

            { infinity, infinity, 0 },
            { -infinity, -infinity, 0 },
        };

        executeTestTable("modf test case for f64 failed at index: ", cases, [](auto& c, const char* cErr) {
            f64 intPart = 0;
            f64 got = core::detail::modf_compiletime_impl(c.a, &intPart);
            f64 fromStdIntPart = 0;
            f64 fromStd = std::modf(c.a, &fromStdIntPart);
            Assert(core::safe_eq(got, fromStd, 0.00000001), cErr);
            Assert(core::safe_eq(got, c.expectedFrac, 0.00000001), cErr);
            if (!core::isinf(intPart)) {
                Assert(core::safe_eq(intPart, fromStdIntPart, 0.00000001), cErr);
                Assert(core::safe_eq(intPart, c.expectedInt, 0.00000001), cErr);
            }
            else {
                Assert(core::isinf(fromStdIntPart), cErr);
                Assert(core::isinf(c.expectedInt), cErr);
            }
        });

        f64 intPart = 0;
        Assert(core::isnan(core::modf(nan, &intPart)));
        Assert(std::isnan(std::modf(nan, &intPart)));
    }

    return 0;
}

i32 run_core_math_tests_suite_std() {
    RunTest(min_max_compare_with_std_test);
    RunTest(check_system_limits_with_std_test);

    RunTest(isinf_isnan_isnormal_cc_impl_compare_with_std_test);
    RunTest(fp_classify_cc_impl_compare_with_std_test);
    RunTest(floor_cc_impl_compare_with_std_test);
    RunTest(ceil_cc_impl_compare_with_std_test);
    RunTest(sqrt_cc_impl_compare_with_std_test);
    RunTest(round_cc_impl_compare_with_std_test);
    RunTest(trunc_cc_impl_compare_with_std_test);
    RunTest(modf_cc_impl_compare_with_std_test);

    return 0;
}
