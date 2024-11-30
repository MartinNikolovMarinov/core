#include "t-index.h"

#include "common/ryu_reference_impl/ryu.h"
#include "common/ryu_reference_impl/ryu_parse.h"

constexpr bool eqFloats(auto a, auto b, bool eqPossible) {
    if (eqPossible) {
        return a == b;
    }

    // Compare bits:
    if constexpr (sizeof(decltype(a)) == 4) {
        u32 bitsa = core::bitCast<u32>(a);
        u32 bitsb = core::bitCast<u32>(b);
        return bitsa == bitsb;
    }
    else {
        u64 bitsa = core::bitCast<u64>(a);
        u64 bitsb = core::bitCast<u64>(b);
        return bitsa == bitsb;
    }
}

constexpr i32 verifyGenericTestCase(const auto& v, const auto& c, const char* cErr) {
    using core::ParseError;

    if (c.err == ParseError::None) {
        CT_CHECK(v.hasValue(), cErr);
        CT_CHECK(v.value() == c.expected, cErr);

        CT_CHECK(!core::isnan(v.value()), cErr);
        CT_CHECK(!core::isinf(v.value()), cErr);
    }
    else {
        CT_CHECK(v.hasErr(), cErr);
        CT_CHECK(v.err() == c.err, cErr);
    }

    return 0;
}

constexpr i32 checkWithOriginal(const auto& v, const auto& c, const char* cErr, bool eqPossible = true) {
    IS_NOT_CONST_EVALUATED {
        using core::ParseError;

        auto value = v.value();

        if constexpr (std::is_same_v<decltype(value), f32>) {
            f32 expected = 0;
            auto status = ryu::s2f_n(c.input, u32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ParseError::None) {
                CT_CHECK(status == ryu::Status::SUCCESS, cErr);
                CT_CHECK(eqFloats(value, expected, eqPossible));
            }
            else {
                CT_CHECK(status != ryu::Status::SUCCESS, cErr);
            }
        }
        else if constexpr (std::is_same_v<decltype(value), f64>) {

            f64 expected = 0;
            auto status = ryu::s2d_n(c.input, u32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ParseError::None) {
                CT_CHECK(status == ryu::Status::SUCCESS, cErr);
                CT_CHECK(eqFloats(value, expected, eqPossible));
            }
            else {
                CT_CHECK(status != ryu::Status::SUCCESS, cErr);
            }
        }
        else {
            static_assert(core::always_false<decltype(value)>, "invalid type passed");
        }
    }

    return 0;
}

constexpr i32 verifyNanAndInf(const auto& v, const auto& c, const char* cErr) {
    if (core::isinf(c.expected)) {
        CT_CHECK(v.value() == c.expected, cErr);
        CT_CHECK(core::isinf(v.value()), cErr);
    }
    else if (core::isnan(c.expected)) {
        CT_CHECK(core::isnan(v.value()), cErr);
    }

    return 0;
}

constexpr i32 exponentNotationTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Simple exponentials
            { "1e1", 10.0f, ParseError::None },
            { "1e+1", 10.0f, ParseError::None },
            { "1E1", 10.0f, ParseError::None },
            { "1e10", 1e10f, ParseError::None },
            { "1e+10", 1e10f, ParseError::None },
            { "2.5e3", 2500.0f, ParseError::None },
            { "3.1415e2", 314.15f, ParseError::None },
            { "3.1415E2", 314.15f, ParseError::None },
            { "6.022e23", 6.022e23f, ParseError::None }, // Avogadro's number (approximate)
            { "123456e-3", 123.456f, ParseError::None },
            { "1234.56e-1", 123.456f, ParseError::None },

            // Negative Exponents
            { "1e-1", 0.1f, ParseError::None },
            { "1E-1", 0.1f, ParseError::None },
            { "5e-3", 0.005f, ParseError::None },
            { "2.71828e-5", 2.71828e-5f, ParseError::None },
            { "1.234e-10", 1.234e-10f, ParseError::None },

            // Exponent zero
            { "1e0", 1.0f, ParseError::None },
            { "0e0", 0.0f, ParseError::None },
            { "1e", 1.0f, ParseError::None },
            { "1e+", 1.0f, ParseError::None },
            { "1e-", 1.0f, ParseError::None },

            // Only exponent
            { ".e10", 0.0f, ParseError::None },
            { "e10", 0.0f, ParseError::None },

            // Exponent with leading zeros
            { "1e001", 10.0f, ParseError::None },
            { "1e-001", 0.1f, ParseError::None },

            // Invalid exponent format
            { "1e1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e+1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e-1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e1e1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple exponents
            { "1ee1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple exponents version 2
            { "1e++1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple '-' signs
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Simple exponentials
            { "1e1", 10.0, ParseError::None },
            { "1e+1", 10.0, ParseError::None },
            { "1E1", 10.0, ParseError::None },
            { "1e10", 1e10, ParseError::None },
            { "1e+10", 1e10, ParseError::None },
            { "2.5e3", 2500.0, ParseError::None },
            { "3.1415e2", 314.15, ParseError::None },
            { "3.1415E2", 314.15, ParseError::None },
            { "6.022e23", 6.022e23, ParseError::None }, // Avogadro's number (approximate)
            { "123456e-3", 123.456, ParseError::None },
            { "1234.56e-1", 123.456, ParseError::None },

            // Negative Exponents
            { "1e-1", 0.1, ParseError::None },
            { "1E-1", 0.1, ParseError::None },
            { "5e-3", 0.005, ParseError::None },
            { "2.71828e-5", 2.71828e-5, ParseError::None },
            { "1.234e-10", 1.234e-10, ParseError::None },

            // Exponent zero
            { "1e0", 1.0, ParseError::None },
            { "0e0", 0.0, ParseError::None },
            { "1e", 1.0, ParseError::None },
            { "1e+", 1.0, ParseError::None },
            { "1e-", 1.0, ParseError::None },

            // Only exponent
            { "e10", 0.0, ParseError::None },
            { ".e10", 0.0, ParseError::None },

            // Exponent with leading zeros
            { "1e001", 10.0, ParseError::None },
            { "1e-001", 0.1, ParseError::None },

            // Large exponents
            { "1e38", 1e38, ParseError::None }, // Near float max
            { "1e-45", 1e-45, ParseError::None }, // Near float min (denormal)

            // Largest representable value
            { "1.7976931348623157e308", core::limitMax<f64>(), ParseError::None },
            { "-1.7976931348623157e308", -core::limitMax<f64>(), ParseError::None },

            // Smallest representable value
            { "2.2250738585072014e-308", core::limitMin<f64>(), ParseError::None },
            { "-2.2250738585072014e-308", -core::limitMin<f64>(), ParseError::None },

            // Invalid exponent format
            { "1e1.0", 0.0, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e+1.0", 0.0, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e-1.0", 0.0, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e1e1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple exponents
            { "1ee1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple exponents version 2
            { "1e++1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple '-' signs

            // Edge cases
            { "1.2999999999999999E+154", 1.2999999999999999E+154, ParseError::None },
            { "2.2250738585072012e-308", 2.2250738585072012e-308, ParseError::None }, // Denormal boundary
            { "2.2250738585072013e-308", 2.2250738585072013e-308, ParseError::None }, // Denormal boundary
            { "2.2250738585072014e-308", 2.2250738585072014e-308, ParseError::None }, // Denormal boundary
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 normalNotationTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
            bool dontCheckOriginal;
        };

        constexpr TestCase cases[] = {
            // Exact Values and Representations
            { "1.5", 1.5f, ParseError::None, false },
            { "0.125", 0.125f, ParseError::None, false }, // Exact binary fraction
            { "0.25", 0.25f, ParseError::None, false },
            { "0.75", 0.75f, ParseError::None, false },

            // Variations of zero
            { "0", 0.0f, ParseError::None, false },
            { "0.0", 0.0f, ParseError::None, false },
            { "0.0000", 0.0f, ParseError::None, false },
            { "-0", -0.0f, ParseError::None, false },
            { "-0.0", -0.0f, ParseError::None, false },
            { "00000000000000000000000000000", 0.f, ParseError::None, false },
            { "0.000000000000000000000000000", 0.f, ParseError::None, false },

            // Arbitrary basic cases
            { "1", 1.0f, ParseError::None, false },
            { "-1", -1.0f, ParseError::None, false },
            { "2.0", 2.0f, ParseError::None, false },
            { "4.0", 4.0f, ParseError::None, false },
            { ".123", 0.123f, ParseError::None, false },
            { "123", 123.f, ParseError::None, false },

            // Sign variations
            { "+1.0", 1.0f, ParseError::None, true },
            { "+0.0", 0.0f, ParseError::None, true },
            { "-123.456", -123.456f, ParseError::None, false },

            // Leading zeroes
            { "000123.456", 123.456f, ParseError::None, false },
            { "0.00000123", 0.00000123f, ParseError::None, false },
            { "0000.0000", 0.0f, ParseError::None, false },

            // Non-Representable Decimal Fractions
            { "0.1", 0.100000001490116119384765625f, ParseError::None, false },
            { "0.2", 0.20000000298023223876953125f, ParseError::None, false },
            { "0.3", 0.300000011920928955078125f, ParseError::None, false },
            { "0.7", 0.699999988079071044921875f, ParseError::None, false },

            // Trailing zeroes
            { "26843549.5", 26843550.0f, ParseError::None, false },
            { "50000002.5", 50000004.0f, ParseError::None, false },
            { "99999989.5", 99999992.0f, ParseError::None, false },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            if (!c.dontCheckOriginal) CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
            bool dontCheckOriginal;
        };

        constexpr TestCase cases[] = {
            // Exact Values and Representations
            { "1.5", 1.5, ParseError::None, false },
            { "0.125", 0.125, ParseError::None, false }, // Exact binary fraction
            { "0.25", 0.25, ParseError::None, false },
            { "0.75", 0.75, ParseError::None, false },

            // Variations of zero
            { "0", 0.0, ParseError::None, false },
            { "0.0", 0.0, ParseError::None, false },
            { "0.0000", 0.0, ParseError::None, false },
            { "-0", -0.0, ParseError::None, false },
            { "-0.0", -0.0, ParseError::None, false },
            { "00000000000000000000000000000", 0., ParseError::None, false },
            { "0.000000000000000000000000000", 0., ParseError::None, false },

            // Arbitrary basic cases
            { "1", 1.0, ParseError::None, false },
            { "-1", -1.0, ParseError::None, false },
            { "2.0", 2.0, ParseError::None, false },
            { "4.0", 4.0, ParseError::None, false },
            { ".123", 0.123, ParseError::None, false },
            { "123", 123., ParseError::None, false },

            // Sign variations
            { "+1.0", 1.0, ParseError::None, true },
            { "+0.0", 0.0, ParseError::None, true },
            { "-123.456", -123.456, ParseError::None, false },

            // Leading zeroes
            { "000123.456", 123.456, ParseError::None, false },
            { "0.00000123", 0.00000123, ParseError::None, false },
            { "0000.0000", 0.0, ParseError::None, false },

            // Non-Representable Decimal Fractions
            { "0.1", 0.10000000000000001, ParseError::None, false },
            { "0.2", 0.20000000000000001, ParseError::None, false },
            { "0.3", 0.29999999999999999, ParseError::None, false },
            { "0.7", 0.69999999999999996, ParseError::None, false },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            if (!c.dontCheckOriginal) CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 specialValuesTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            bool eqPossible;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { "1e-45", 1.401298e-45f, true, ParseError::None }, // Smallest positive subnormal float
            { "-1e-45", -1.401298e-45f, true, ParseError::None },
            { "1.4e-45", 1.4e-45f, true, ParseError::None },

            // Largest representable value
            { "3.4028235e38", core::limitMax<f32>(), true, ParseError::None },
            { "-3.4028235e38", -core::limitMax<f32>(), true, ParseError::None },

            // Smallest representable value
            { "1.17549435e-38", core::limitMin<f32>(), true, ParseError::None },
            { "-1.17549435e-38", -core::limitMin<f32>(), true, ParseError::None },

            // Overflow cases
            { "1e39", core::infinity<f32>(), false, ParseError::None },
            { "-1e39", -core::infinity<f32>(), false, ParseError::None },

            // Underflow to zero
            { "1e-50", 0.0f, true, ParseError::None },
            { "-1e-50", -0.0f, true, ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr, c.eqPossible) == 0);

            if (c.err == ParseError::None) {
                CT_CHECK(v.hasValue(), cErr);
                CT_CHECK(eqFloats(v.value(), c.expected, c.eqPossible) , cErr);
            }
            else {
                CT_CHECK(v.hasErr(), cErr);
                CT_CHECK(v.err() == c.err, cErr);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            bool eqPossible;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { "5e-324", 5e-324, true, ParseError::None }, // Smallest positive subnormal f64
            { "-5e-324", -5e-324, true, ParseError::None },
            { "5.3e-324", 5.3e-324, true, ParseError::None },

            // Largest representable value
            { "1.7976931348623157e308", core::limitMax<f64>(), true, ParseError::None },
            { "-1.7976931348623157e308", -core::limitMax<f64>(), true, ParseError::None },

            // Smallest representable value
            { "2.2250738585072014e-308", core::limitMin<f64>(), true, ParseError::None },
            { "-2.2250738585072014e-308", -core::limitMin<f64>(), true, ParseError::None },

            // Overflow cases
            { "1e309", core::infinity<f64>(), false, ParseError::None },
            { "-1e309", -core::infinity<f64>(), false, ParseError::None },

            // Underflow to zero
            { "1e-325", 0.0, true, ParseError::None },
            { "-1e-325", -0.0, true, ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr, c.eqPossible) == 0);

            if (c.err == ParseError::None) {
                CT_CHECK(v.hasValue(), cErr);
                CT_CHECK(eqFloats(v.value(), c.expected, c.eqPossible) , cErr);
            }
            else {
                CT_CHECK(v.hasErr(), cErr);
                CT_CHECK(v.err() == c.err, cErr);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 nanAndInfTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f32>(), ParseError::None },
            { "-inf", -core::infinity<f32>(), ParseError::None },
            { "INF", core::infinity<f32>(), ParseError::None },
            { "-INF", -core::infinity<f32>(), ParseError::None },

            // None cases:
            { "nan", core::signalingNaN<f32>(), ParseError::None },
            { "NaN", core::signalingNaN<f32>(), ParseError::None },
            { "NAN", core::signalingNaN<f32>(), ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ParseError::None) CT_CHECK(v.hasValue(), cErr); // verify error is expected
            CT_CHECK(verifyNanAndInf(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f64>(), ParseError::None },
            { "-inf", -core::infinity<f64>(), ParseError::None },
            { "INF", core::infinity<f64>(), ParseError::None },
            { "-INF", -core::infinity<f64>(), ParseError::None },

            // None cases:
            { "nan", core::signalingNaN<f64>(), ParseError::None },
            { "NaN", core::signalingNaN<f64>(), ParseError::None },
            { "NAN", core::signalingNaN<f64>(), ParseError::None },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ParseError::None) CT_CHECK(v.hasValue(), cErr); // verify error is expected
            CT_CHECK(verifyNanAndInf(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 whitespaceTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            { " 1.0", 0.0f, ParseError::InputHasInvalidSymbol },
            { "1.0 ", 0.0f, ParseError::InputHasInvalidSymbol },
            { "\t1.0", 0.0f, ParseError::InputHasInvalidSymbol },
            { "1.0\n", 0.0f, ParseError::InputHasInvalidSymbol },

            { "0.1 0", 0.0f, ParseError::InputHasInvalidSymbol },
            { "0. 10", 0.0f, ParseError::InputHasInvalidSymbol },
            { "0 .10", 0.0f, ParseError::InputHasInvalidSymbol },
            { "0 1", 0.0f, ParseError::InputHasInvalidSymbol },

            { "", 0.0f, ParseError::InputEmpty },
            { nullptr, 0.0f, ParseError::InputEmpty },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            { " 1.0", 0.0, ParseError::InputHasInvalidSymbol },
            { "1.0 ", 0.0, ParseError::InputHasInvalidSymbol },
            { "\t1.0", 0.0, ParseError::InputHasInvalidSymbol },
            { "1.0\n", 0.0, ParseError::InputHasInvalidSymbol },

            { "0.1 0", 0.0, ParseError::InputHasInvalidSymbol },
            { "0. 10", 0.0, ParseError::InputHasInvalidSymbol },
            { "0 .10", 0.0, ParseError::InputHasInvalidSymbol },
            { "0 1", 0.0, ParseError::InputHasInvalidSymbol },

            { "", 0.0, ParseError::InputEmpty },
            { nullptr, 0.0, ParseError::InputEmpty },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 invalidInputTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            { "1.0f", 0.0f, ParseError::InputHasInvalidSymbol }, // Suffix 'f' not expected
            { "1.0d", 0.0f, ParseError::InputHasInvalidSymbol }, // Suffix 'd' not expected
            { "123_456", 0.0f, ParseError::InputHasInvalidSymbol },

            // Non-ASCII and Unicode Characters are not supported
            { "１.０", 0.0f, ParseError::InputHasInvalidSymbol }, // Full-width digits
            { "1.0€", 0.0f, ParseError::InputHasInvalidSymbol }, // Trailing Unicode character

            // Multiple decimal points and signs
            { "1..0", 0.0f, ParseError::InputHasMultipleDots },
            { "1.0.0", 0.0f, ParseError::InputHasMultipleDots },
            { "++1.0", 0.0f, ParseError::InputHasInvalidSymbol },
            { "--1.0", 0.0f, ParseError::InputHasInvalidSymbol },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            { "1.0f", 0.0, ParseError::InputHasInvalidSymbol }, // Suffix 'f' not expected
            { "1.0d", 0.0, ParseError::InputHasInvalidSymbol }, // Suffix 'd' not expected
            { "123_456", 0.0, ParseError::InputHasInvalidSymbol },

            // Non-ASCII and Unicode Characters are not supported
            { "１.０", 0.0, ParseError::InputHasInvalidSymbol }, // Full-width digits
            { "1.0€", 0.0, ParseError::InputHasInvalidSymbol }, // Trailing Unicode character

            // Multiple decimal points and signs
            { "1..0", 0.0, ParseError::InputHasMultipleDots },
            { "1.0.0", 0.0, ParseError::InputHasMultipleDots },
            { "++1.0", 0.0, ParseError::InputHasInvalidSymbol },
            { "--1.0", 0.0, ParseError::InputHasInvalidSymbol },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 inputCloseToLimitTest() {
    using core::ParseError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Mantissa too large
            { "9999999999", 0.f, ParseError::InputNumberTooLarge },
            { "-1234567890", 0.f, ParseError::InputNumberTooLarge },

            { "0123456789", 123456789.f, ParseError::None },
            { "-0123456789", -123456789.f, ParseError::None },
            { "1234567890", 0.f, ParseError::InputNumberTooLarge },
            { "123456789.0", 0.f, ParseError::InputNumberTooLarge },
            { "12345678.00", 0.f, ParseError::InputNumberTooLarge },
            { "1234567.000", 0.f, ParseError::InputNumberTooLarge },

            // Mantissa rounding overflow
            { "0.999999999", 1.0f, ParseError::None },

            // Exponent too large
            { "1e12345", 0.0f, ParseError::InputNumberTooLarge },
            { "1e-12345", 0.0f, ParseError::InputNumberTooLarge },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ParseError err;
        };

        constexpr TestCase cases[] = {
            // Mantissa too large
            { "999999999999999999", 0.0, ParseError::InputNumberTooLarge },
            { "-123456789012345678", 0.0, ParseError::InputNumberTooLarge },

            { "012345678912345678", 012345678912345678.0, ParseError::None },
            { "-012345678912345678", -012345678912345678.0, ParseError::None },
            { "123456789123456789", 0.0, ParseError::InputNumberTooLarge },
            { "12345678912345678.0", 0.0, ParseError::InputNumberTooLarge },
            { "1234567891234567.00", 0.0, ParseError::InputNumberTooLarge },
            { "123456789123456.000", 0.0, ParseError::InputNumberTooLarge },

            // Mantissa rounding overflow
            { "0.99999999999999999", 1.0, ParseError::None },

            // Exponent too large
            { "1e12345", 0.0, ParseError::InputNumberTooLarge },
            { "1e-12345", 0.0, ParseError::InputNumberTooLarge },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(verifyGenericTestCase(v, c, cErr) == 0);
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 runCstrConv_CstrToFloat() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(exponentNotationTest);
    if (runTest(tInfo, exponentNotationTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(normalNotationTest);
    if (runTest(tInfo, normalNotationTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(specialValuesTest);
    if (runTest(tInfo, specialValuesTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(nanAndInfTest);
    if (runTest(tInfo, nanAndInfTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(whitespaceTest);
    if (runTest(tInfo, whitespaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(invalidInputTest);
    if (runTest(tInfo, invalidInputTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(inputCloseToLimitTest);
    if (runTest(tInfo, inputCloseToLimitTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCstrConvTestsSuite() {
    RunTestCompileTime(exponentNotationTest);
    RunTestCompileTime(normalNotationTest);
    RunTestCompileTime(specialValuesTest);
    RunTestCompileTime(nanAndInfTest);
    RunTestCompileTime(whitespaceTest);
    RunTestCompileTime(invalidInputTest);
    RunTestCompileTime(inputCloseToLimitTest);

    return 0;
}
