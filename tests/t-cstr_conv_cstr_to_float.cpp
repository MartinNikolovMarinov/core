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
    using core::ConversionError;

    if (c.err == ConversionError::NONE) {
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
        using core::ConversionError;

        auto value = v.value();

        if constexpr (std::is_same_v<decltype(value), f32>) {
            f32 expected = 0;
            auto status = ryu::s2f_n(c.input, i32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ConversionError::NONE) {
                CT_CHECK(status == ryu::Status::SUCCESS, cErr);
                CT_CHECK(eqFloats(value, expected, eqPossible));
            }
            else {
                CT_CHECK(status != ryu::Status::SUCCESS, cErr);
            }
        }
        else if constexpr (std::is_same_v<decltype(value), f64>) {

            f64 expected = 0;
            auto status = ryu::s2d_n(c.input, i32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ConversionError::NONE) {
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Simple exponentials
            { "1e1", 10.0f, ConversionError::NONE },
            { "1e+1", 10.0f, ConversionError::NONE },
            { "1E1", 10.0f, ConversionError::NONE },
            { "1e10", 1e10f, ConversionError::NONE },
            { "1e+10", 1e10f, ConversionError::NONE },
            { "2.5e3", 2500.0f, ConversionError::NONE },
            { "3.1415e2", 314.15f, ConversionError::NONE },
            { "3.1415E2", 314.15f, ConversionError::NONE },
            { "6.022e23", 6.022e23f, ConversionError::NONE }, // Avogadro's number (approximate)
            { "123456e-3", 123.456f, ConversionError::NONE },
            { "1234.56e-1", 123.456f, ConversionError::NONE },

            // Negative Exponents
            { "1e-1", 0.1f, ConversionError::NONE },
            { "1E-1", 0.1f, ConversionError::NONE },
            { "5e-3", 0.005f, ConversionError::NONE },
            { "2.71828e-5", 2.71828e-5f, ConversionError::NONE },
            { "1.234e-10", 1.234e-10f, ConversionError::NONE },

            // Exponent zero
            { "1e0", 1.0f, ConversionError::NONE },
            { "0e0", 0.0f, ConversionError::NONE },
            { "1e", 1.0f, ConversionError::NONE },
            { "1e+", 1.0f, ConversionError::NONE },
            { "1e-", 1.0f, ConversionError::NONE },

            // Only exponent
            { ".e10", 0.0f, ConversionError::NONE },
            { "e10", 0.0f, ConversionError::NONE },

            // Exponent with leading zeros
            { "1e001", 10.0f, ConversionError::NONE },
            { "1e-001", 0.1f, ConversionError::NONE },

            // Invalid exponent format
            { "1e1.0", 0.0f, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e+1.0", 0.0f, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e-1.0", 0.0f, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e1e1", 0.0f, ConversionError::InputHasInvalidSymbol }, // Multiple exponents
            { "1ee1", 0.0f, ConversionError::InputHasInvalidSymbol }, // Multiple exponents version 2
            { "1e++1", 0.0f, ConversionError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0f, ConversionError::InputHasInvalidSymbol }, // Multiple '-' signs
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
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Simple exponentials
            { "1e1", 10.0, ConversionError::NONE },
            { "1e+1", 10.0, ConversionError::NONE },
            { "1E1", 10.0, ConversionError::NONE },
            { "1e10", 1e10, ConversionError::NONE },
            { "1e+10", 1e10, ConversionError::NONE },
            { "2.5e3", 2500.0, ConversionError::NONE },
            { "3.1415e2", 314.15, ConversionError::NONE },
            { "3.1415E2", 314.15, ConversionError::NONE },
            { "6.022e23", 6.022e23, ConversionError::NONE }, // Avogadro's number (approximate)
            { "123456e-3", 123.456, ConversionError::NONE },
            { "1234.56e-1", 123.456, ConversionError::NONE },

            // Negative Exponents
            { "1e-1", 0.1, ConversionError::NONE },
            { "1E-1", 0.1, ConversionError::NONE },
            { "5e-3", 0.005, ConversionError::NONE },
            { "2.71828e-5", 2.71828e-5, ConversionError::NONE },
            { "1.234e-10", 1.234e-10, ConversionError::NONE },

            // Exponent zero
            { "1e0", 1.0, ConversionError::NONE },
            { "0e0", 0.0, ConversionError::NONE },
            { "1e", 1.0, ConversionError::NONE },
            { "1e+", 1.0, ConversionError::NONE },
            { "1e-", 1.0, ConversionError::NONE },

            // Only exponent
            { "e10", 0.0, ConversionError::NONE },
            { ".e10", 0.0, ConversionError::NONE },

            // Exponent with leading zeros
            { "1e001", 10.0, ConversionError::NONE },
            { "1e-001", 0.1, ConversionError::NONE },

            // Large exponents
            { "1e38", 1e38, ConversionError::NONE }, // Near float max
            { "1e-45", 1e-45, ConversionError::NONE }, // Near float min (denormal)

            // Largest representable value
            { "1.7976931348623157e308", core::limitMax<f64>(), ConversionError::NONE },
            { "-1.7976931348623157e308", -core::limitMax<f64>(), ConversionError::NONE },

            // Smallest representable value
            { "2.2250738585072014e-308", core::limitMin<f64>(), ConversionError::NONE },
            { "-2.2250738585072014e-308", -core::limitMin<f64>(), ConversionError::NONE },

            // Invalid exponent format
            { "1e1.0", 0.0, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e+1.0", 0.0, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e-1.0", 0.0, ConversionError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e1e1", 0.0, ConversionError::InputHasInvalidSymbol }, // Multiple exponents
            { "1ee1", 0.0, ConversionError::InputHasInvalidSymbol }, // Multiple exponents version 2
            { "1e++1", 0.0, ConversionError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0, ConversionError::InputHasInvalidSymbol }, // Multiple '-' signs

            // Edge cases
            { "1.2999999999999999E+154", 1.2999999999999999E+154, ConversionError::NONE },
            { "2.2250738585072012e-308", 2.2250738585072012e-308, ConversionError::NONE }, // Denormal boundary
            { "2.2250738585072013e-308", 2.2250738585072013e-308, ConversionError::NONE }, // Denormal boundary
            { "2.2250738585072014e-308", 2.2250738585072014e-308, ConversionError::NONE }, // Denormal boundary
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
            bool dontCheckOriginal;
        };

        constexpr TestCase cases[] = {
            // Exact Values and Representations
            { "1.5", 1.5f, ConversionError::NONE, false },
            { "0.125", 0.125f, ConversionError::NONE, false }, // Exact binary fraction
            { "0.25", 0.25f, ConversionError::NONE, false },
            { "0.75", 0.75f, ConversionError::NONE, false },

            // Variations of zero
            { "0", 0.0f, ConversionError::NONE, false },
            { "0.0", 0.0f, ConversionError::NONE, false },
            { "0.0000", 0.0f, ConversionError::NONE, false },
            { "-0", -0.0f, ConversionError::NONE, false },
            { "-0.0", -0.0f, ConversionError::NONE, false },
            { "00000000000000000000000000000", 0.f, ConversionError::NONE, false },
            { "0.000000000000000000000000000", 0.f, ConversionError::NONE, false },

            // Arbitrary basic cases
            { "1", 1.0f, ConversionError::NONE, false },
            { "-1", -1.0f, ConversionError::NONE, false },
            { "2.0", 2.0f, ConversionError::NONE, false },
            { "4.0", 4.0f, ConversionError::NONE, false },
            { ".123", 0.123f, ConversionError::NONE, false },
            { "123", 123.f, ConversionError::NONE, false },

            // Sign variations
            { "+1.0", 1.0f, ConversionError::NONE, true },
            { "+0.0", 0.0f, ConversionError::NONE, true },
            { "-123.456", -123.456f, ConversionError::NONE, false },

            // Leading zeroes
            { "000123.456", 123.456f, ConversionError::NONE, false },
            { "0.00000123", 0.00000123f, ConversionError::NONE, false },
            { "0000.0000", 0.0f, ConversionError::NONE, false },

            // Non-Representable Decimal Fractions
            { "0.1", 0.100000001490116119384765625f, ConversionError::NONE, false },
            { "0.2", 0.20000000298023223876953125f, ConversionError::NONE, false },
            { "0.3", 0.300000011920928955078125f, ConversionError::NONE, false },
            { "0.7", 0.699999988079071044921875f, ConversionError::NONE, false },

            // Trailing zeroes
            { "26843549.5", 26843550.0f, ConversionError::NONE, false },
            { "50000002.5", 50000004.0f, ConversionError::NONE, false },
            { "99999989.5", 99999992.0f, ConversionError::NONE, false },
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
            ConversionError err;
            bool dontCheckOriginal;
        };

        constexpr TestCase cases[] = {
            // Exact Values and Representations
            { "1.5", 1.5, ConversionError::NONE, false },
            { "0.125", 0.125, ConversionError::NONE, false }, // Exact binary fraction
            { "0.25", 0.25, ConversionError::NONE, false },
            { "0.75", 0.75, ConversionError::NONE, false },

            // Variations of zero
            { "0", 0.0, ConversionError::NONE, false },
            { "0.0", 0.0, ConversionError::NONE, false },
            { "0.0000", 0.0, ConversionError::NONE, false },
            { "-0", -0.0, ConversionError::NONE, false },
            { "-0.0", -0.0, ConversionError::NONE, false },
            { "00000000000000000000000000000", 0., ConversionError::NONE, false },
            { "0.000000000000000000000000000", 0., ConversionError::NONE, false },

            // Arbitrary basic cases
            { "1", 1.0, ConversionError::NONE, false },
            { "-1", -1.0, ConversionError::NONE, false },
            { "2.0", 2.0, ConversionError::NONE, false },
            { "4.0", 4.0, ConversionError::NONE, false },
            { ".123", 0.123, ConversionError::NONE, false },
            { "123", 123., ConversionError::NONE, false },

            // Sign variations
            { "+1.0", 1.0, ConversionError::NONE, true },
            { "+0.0", 0.0, ConversionError::NONE, true },
            { "-123.456", -123.456, ConversionError::NONE, false },

            // Leading zeroes
            { "000123.456", 123.456, ConversionError::NONE, false },
            { "0.00000123", 0.00000123, ConversionError::NONE, false },
            { "0000.0000", 0.0, ConversionError::NONE, false },

            // Non-Representable Decimal Fractions
            { "0.1", 0.10000000000000001, ConversionError::NONE, false },
            { "0.2", 0.20000000000000001, ConversionError::NONE, false },
            { "0.3", 0.29999999999999999, ConversionError::NONE, false },
            { "0.7", 0.69999999999999996, ConversionError::NONE, false },
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            bool eqPossible;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { "1e-45", 1.401298e-45f, true, ConversionError::NONE }, // Smallest positive subnormal float
            { "-1e-45", -1.401298e-45f, true, ConversionError::NONE },
            { "1.4e-45", 1.4e-45f, true, ConversionError::NONE },

            // Largest representable value
            { "3.4028235e38", core::limitMax<f32>(), true, ConversionError::NONE },
            { "-3.4028235e38", -core::limitMax<f32>(), true, ConversionError::NONE },

            // Smallest representable value
            { "1.17549435e-38", core::limitMin<f32>(), true, ConversionError::NONE },
            { "-1.17549435e-38", -core::limitMin<f32>(), true, ConversionError::NONE },

            // Overflow cases
            { "1e39", core::infinity<f32>(), false, ConversionError::NONE },
            { "-1e39", -core::infinity<f32>(), false, ConversionError::NONE },

            // Underflow to zero
            { "1e-50", 0.0f, true, ConversionError::NONE },
            { "-1e-50", -0.0f, true, ConversionError::NONE },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr, c.eqPossible) == 0);

            if (c.err == ConversionError::NONE) {
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
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { "5e-324", 5e-324, true, ConversionError::NONE }, // Smallest positive subnormal f64
            { "-5e-324", -5e-324, true, ConversionError::NONE },
            { "5.3e-324", 5.3e-324, true, ConversionError::NONE },

            // Largest representable value
            { "1.7976931348623157e308", core::limitMax<f64>(), true, ConversionError::NONE },
            { "-1.7976931348623157e308", -core::limitMax<f64>(), true, ConversionError::NONE },

            // Smallest representable value
            { "2.2250738585072014e-308", core::limitMin<f64>(), true, ConversionError::NONE },
            { "-2.2250738585072014e-308", -core::limitMin<f64>(), true, ConversionError::NONE },

            // Overflow cases
            { "1e309", core::infinity<f64>(), false, ConversionError::NONE },
            { "-1e309", -core::infinity<f64>(), false, ConversionError::NONE },

            // Underflow to zero
            { "1e-325", 0.0, true, ConversionError::NONE },
            { "-1e-325", -0.0, true, ConversionError::NONE },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr, c.eqPossible) == 0);

            if (c.err == ConversionError::NONE) {
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f32>(), ConversionError::NONE },
            { "-inf", -core::infinity<f32>(), ConversionError::NONE },
            { "INF", core::infinity<f32>(), ConversionError::NONE },
            { "-INF", -core::infinity<f32>(), ConversionError::NONE },

            // None cases:
            { "nan", core::signalingNaN<f32>(), ConversionError::NONE },
            { "NaN", core::signalingNaN<f32>(), ConversionError::NONE },
            { "NAN", core::signalingNaN<f32>(), ConversionError::NONE },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ConversionError::NONE) CT_CHECK(v.hasValue(), cErr); // verify error is expected
            CT_CHECK(verifyNanAndInf(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* input;
            f64 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Infinity cases:
            { "inf", core::infinity<f64>(), ConversionError::NONE },
            { "-inf", -core::infinity<f64>(), ConversionError::NONE },
            { "INF", core::infinity<f64>(), ConversionError::NONE },
            { "-INF", -core::infinity<f64>(), ConversionError::NONE },

            // None cases:
            { "nan", core::signalingNaN<f64>(), ConversionError::NONE },
            { "NaN", core::signalingNaN<f64>(), ConversionError::NONE },
            { "NAN", core::signalingNaN<f64>(), ConversionError::NONE },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            if (c.err == ConversionError::NONE) CT_CHECK(v.hasValue(), cErr); // verify error is expected
            CT_CHECK(verifyNanAndInf(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 whitespaceTest() {
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            { " 1.0", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "1.0 ", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "\t1.0", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "1.0\n", 0.0f, ConversionError::InputHasInvalidSymbol },

            { "0.1 0", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "0. 10", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "0 .10", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "0 1", 0.0f, ConversionError::InputHasInvalidSymbol },

            { "", 0.0f, ConversionError::InputEmpty },
            { nullptr, 0.0f, ConversionError::InputEmpty },
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
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            { " 1.0", 0.0, ConversionError::InputHasInvalidSymbol },
            { "1.0 ", 0.0, ConversionError::InputHasInvalidSymbol },
            { "\t1.0", 0.0, ConversionError::InputHasInvalidSymbol },
            { "1.0\n", 0.0, ConversionError::InputHasInvalidSymbol },

            { "0.1 0", 0.0, ConversionError::InputHasInvalidSymbol },
            { "0. 10", 0.0, ConversionError::InputHasInvalidSymbol },
            { "0 .10", 0.0, ConversionError::InputHasInvalidSymbol },
            { "0 1", 0.0, ConversionError::InputHasInvalidSymbol },

            { "", 0.0, ConversionError::InputEmpty },
            { nullptr, 0.0, ConversionError::InputEmpty },
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            { "1.0f", 0.0f, ConversionError::InputHasInvalidSymbol }, // Suffix 'f' not expected
            { "1.0d", 0.0f, ConversionError::InputHasInvalidSymbol }, // Suffix 'd' not expected
            { "123_456", 0.0f, ConversionError::InputHasInvalidSymbol },

            // Non-ASCII and Unicode Characters are not supported
            { "１.０", 0.0f, ConversionError::InputHasInvalidSymbol }, // Full-width digits
            { "1.0€", 0.0f, ConversionError::InputHasInvalidSymbol }, // Trailing Unicode character

            // Multiple decimal points and signs
            { "1..0", 0.0f, ConversionError::InputHasMultipleDots },
            { "1.0.0", 0.0f, ConversionError::InputHasMultipleDots },
            { "++1.0", 0.0f, ConversionError::InputHasInvalidSymbol },
            { "--1.0", 0.0f, ConversionError::InputHasInvalidSymbol },
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
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            { "1.0f", 0.0, ConversionError::InputHasInvalidSymbol }, // Suffix 'f' not expected
            { "1.0d", 0.0, ConversionError::InputHasInvalidSymbol }, // Suffix 'd' not expected
            { "123_456", 0.0, ConversionError::InputHasInvalidSymbol },

            // Non-ASCII and Unicode Characters are not supported
            { "１.０", 0.0, ConversionError::InputHasInvalidSymbol }, // Full-width digits
            { "1.0€", 0.0, ConversionError::InputHasInvalidSymbol }, // Trailing Unicode character

            // Multiple decimal points and signs
            { "1..0", 0.0, ConversionError::InputHasMultipleDots },
            { "1.0.0", 0.0, ConversionError::InputHasMultipleDots },
            { "++1.0", 0.0, ConversionError::InputHasInvalidSymbol },
            { "--1.0", 0.0, ConversionError::InputHasInvalidSymbol },
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
    using core::ConversionError;

    {
        struct TestCase {
            const char* input;
            f32 expected;
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Mantissa too large
            { "9999999999", 0.f, ConversionError::InputNumberTooLarge },
            { "-1234567890", 0.f, ConversionError::InputNumberTooLarge },

            { "0123456789", 123456789.f, ConversionError::NONE },
            { "-0123456789", -123456789.f, ConversionError::NONE },
            { "1234567890", 0.f, ConversionError::InputNumberTooLarge },
            { "123456789.0", 0.f, ConversionError::InputNumberTooLarge },
            { "12345678.00", 0.f, ConversionError::InputNumberTooLarge },
            { "1234567.000", 0.f, ConversionError::InputNumberTooLarge },

            // Mantissa rounding overflow
            { "0.999999999", 1.0f, ConversionError::NONE },

            // Exponent too large
            { "1e12345", 0.0f, ConversionError::InputNumberTooLarge },
            { "1e-12345", 0.0f, ConversionError::InputNumberTooLarge },
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
            ConversionError err;
        };

        constexpr TestCase cases[] = {
            // Mantissa too large
            { "999999999999999999", 0.0, ConversionError::InputNumberTooLarge },
            { "-123456789012345678", 0.0, ConversionError::InputNumberTooLarge },

            { "012345678912345678", 012345678912345678.0, ConversionError::NONE },
            { "-012345678912345678", -012345678912345678.0, ConversionError::NONE },
            { "123456789123456789", 0.0, ConversionError::InputNumberTooLarge },
            { "12345678912345678.0", 0.0, ConversionError::InputNumberTooLarge },
            { "1234567891234567.00", 0.0, ConversionError::InputNumberTooLarge },
            { "123456789123456.000", 0.0, ConversionError::InputNumberTooLarge },

            // Mantissa rounding overflow
            { "0.99999999999999999", 1.0, ConversionError::NONE },

            // Exponent too large
            { "1e12345", 0.0, ConversionError::InputNumberTooLarge },
            { "1e-12345", 0.0, ConversionError::InputNumberTooLarge },
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

i32 runCstrConv_CstrToFloat_TestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo(sInfo);

    tInfo.name = FN_NAME_TO_CPTR(exponentNotationTest);
    if (runTest(tInfo, exponentNotationTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(normalNotationTest);
    if (runTest(tInfo, normalNotationTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(specialValuesTest);
    if (runTest(tInfo, specialValuesTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(nanAndInfTest);
    if (runTest(tInfo, nanAndInfTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(whitespaceTest);
    if (runTest(tInfo, whitespaceTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(invalidInputTest);
    if (runTest(tInfo, invalidInputTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(inputCloseToLimitTest);
    if (runTest(tInfo, inputCloseToLimitTest) != 0) { return -1; }

    return 0;
}

constexpr i32 runCompiletimeCstrConv_CstrToFloat_TestsSuite() {
    RunTestCompileTime(exponentNotationTest);
    RunTestCompileTime(normalNotationTest);
    RunTestCompileTime(specialValuesTest);
    RunTestCompileTime(nanAndInfTest);
    RunTestCompileTime(whitespaceTest);
    RunTestCompileTime(invalidInputTest);
    RunTestCompileTime(inputCloseToLimitTest);

    return 0;
}
