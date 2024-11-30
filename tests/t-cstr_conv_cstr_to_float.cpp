#include "t-index.h"

#include "common/ryu_reference_impl/ryu.h"
#include "common/ryu_reference_impl/ryu_parse.h"

constexpr i32 verifyTestCase(const auto& v, const auto& c, const char* cErr) {
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

constexpr i32 checkWithOriginal(const auto& v, const auto& c, const char* cErr) {
    IS_NOT_CONST_EVALUATED {
        using core::ParseError;

        auto value = v.value();

        if constexpr (std::is_same_v<decltype(value), f32>) {
            f32 expected;
            auto status = ryu::s2f_n(c.input, u32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ParseError::None) {
                CT_CHECK(status == ryu::Status::SUCCESS, cErr);
                CT_CHECK(value == expected, cErr);
            }
            else {
                CT_CHECK(status != ryu::Status::SUCCESS, cErr);
            }
        }
        else if constexpr (std::is_same_v<decltype(value), f64>) {
            f64 expected;
            auto status = ryu::s2d_n(c.input, u32(core::cstrLen(c.input)), &expected);
            if (c.err ==  ParseError::None) {
                CT_CHECK(status == ryu::Status::SUCCESS, cErr);
                CT_CHECK(value == expected, cErr);
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

            // Exponent with leading zeros
            { "1e001", 10.0f, ParseError::None },
            { "1e-001", 0.1f, ParseError::None },

            // Largest representable value
            { "3.4028235e38", core::limitMax<f32>(), ParseError::None },
            { "-3.4028235e38", -core::limitMax<f32>(), ParseError::None },

            // Smallest representable value
            { "1.17549435e-38", core::limitMin<f32>(), ParseError::None },
            { "-1.17549435e-38", -core::limitMin<f32>(), ParseError::None },

            // Invalid exponent format
            { "1e1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e+1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e-1.0", 0.0f, ParseError::InputHasInvalidSymbol }, // Exponent with decimal
            { "1e1e1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple exponents
            { "1e++1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0f, ParseError::InputHasInvalidSymbol }, // Multiple '-' signs
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            CT_CHECK(verifyTestCase(v, c, cErr) == 0);
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
            { "1e++1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple '+' signs
            { "1e--1", 0.0, ParseError::InputHasInvalidSymbol }, // Multiple '-' signs
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
            CT_CHECK(checkWithOriginal(v, c, cErr) == 0);
            CT_CHECK(verifyTestCase(v, c, cErr) == 0);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

// constexpr i32 cstrToFloatTest() {
//     using core::ParseError;

//     {
//         struct TestCase {
//             const char* input;
//             f32 expected;
//             ParseError err;
//         };

//         constexpr TestCase cases[] = {
//             // Basic
//             { "0", 0.0f, ParseError::None },
//             { "0.0", 0.0f, ParseError::None },
//             { "0.0000", 0.0f, ParseError::None },
//             { "-0", -0.0f, ParseError::None },
//             { "-0.0", -0.0f, ParseError::None },

//             { "1", 1.0f, ParseError::None },
//             { "-1", -1.0f, ParseError::None },
//             { "2.0", 2.0f, ParseError::None },
//             { "4.0", 4.0f, ParseError::None },
//             { "123456789", 123456792.0f, ParseError::None },
//             { "299792458", 299792448.0f, ParseError::None },
//             { ".123", 0.123f, ParseError::None },
//             { "123", 123.f, ParseError::None },
//             { "00000000000000000000000000000", 0.f, ParseError::None },
//             { "0.000000000000000000000000000", 0.f, ParseError::None },

//             // Sign variations
//             { "+1.0", 1.0f, ParseError::None },
//             { "-123.456", -123.456f, ParseError::None },
//             { "+0.0", 0.0f, ParseError::None },
//             { "--1.0", 0.0f, ParseError::InputHasInvalidSymbol },
//             { "+-1.0", 0.0f, ParseError::InputHasInvalidSymbol },
//             { "-+1.0", 0.0f, ParseError::InputHasInvalidSymbol },

//             // Non-Representable Decimal Fractions
//             { "0.1", 0.100000001490116119384765625f, ParseError::None },
//             { "0.2", 0.20000000298023223876953125f, ParseError::None },
//             { "0.3", 0.300000011920928955078125f, ParseError::None },
//             { "0.7", 0.699999988079071044921875f, ParseError::None },

//             // Trailing zeroes
//             { "26843549.5", 26843550.0f, ParseError::None },
//             { "50000002.5", 50000004.0f, ParseError::None },
//             { "99999989.5", 99999992.0f, ParseError::None },

//             // Error cases
//             { "", 0, ParseError::InputEmpty },
//             { nullptr, 0, ParseError::InputEmpty },
//             { ".12.3", 0, ParseError::InputHasMultipleDots },

//             { "a123", 0, ParseError::InputHasInvalidSymbol },
//             { "0.a123", 0, ParseError::InputHasInvalidSymbol },
//             { ".123a", 0, ParseError::InputHasInvalidSymbol },
//             { "123a", 0, ParseError::InputHasInvalidSymbol },
//             { "12.3a", 0, ParseError::InputHasInvalidSymbol },
//             { "1 2", 0, ParseError::InputHasInvalidSymbol },

//             { "0123456789", 123456789.f, ParseError::None },
//             { "1234567890", 0.f, ParseError::InputNumberTooLarge },
//             { "123456789.0", 0.f, ParseError::InputNumberTooLarge },
//             { "12345678.00", 0.f, ParseError::InputNumberTooLarge },
//             { "1234567.000", 0.f, ParseError::InputNumberTooLarge },

//             { "9999999999", 0.f, ParseError::InputNumberTooLarge },
//             { "-1234567890", 0.f, ParseError::InputNumberTooLarge },

//             // TODO2: At some point I might want these to work, but for now just make sure they don't assert
//             { " 21.123", 0.f, ParseError::InputHasInvalidSymbol },
//             { "21.123 ", 0.f, ParseError::InputHasInvalidSymbol },
//         };

//         i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
//             auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
//             if (c.err == ParseError::None) {
//                 CT_CHECK(v.hasValue(), cErr);
//                 CT_CHECK(v.value() == c.expected, cErr);

//                 CT_CHECK(!core::isnan(v.value()), cErr);
//                 CT_CHECK(!core::isinf(v.value()), cErr);
//             }
//             else {
//                 CT_CHECK(v.hasErr(), cErr);
//                 CT_CHECK(v.err() == c.err, cErr);
//             }

//             return 0;
//         });
//         CT_CHECK(ret == 0);
//     }

//     {
//         struct TestCase {
//             const char* input;
//             f64 expected;
//             ParseError err;
//         };

//         constexpr TestCase cases[] = {
//             // Basic
//             { "0", 0.0, ParseError::None },
//             { "0.0", 0.0, ParseError::None },
//             { "0.0000", 0.0, ParseError::None },
//             { "-0", -0.0, ParseError::None },
//             { "-0.0", -0.0, ParseError::None },

//             { "1", 1.0, ParseError::None },
//             { "-1", -1.0, ParseError::None },
//             { "2.0", 2.0, ParseError::None },
//             { "4.0", 4.0, ParseError::None },
//             { "123456789", 123456789.0, ParseError::None },
//             { "299792458", 299792458.0, ParseError::None },
//             { ".123", 0.123, ParseError::None },
//             { "123", 123.0, ParseError::None },
//             { "00000000000000000000000000000", 0.0, ParseError::None },
//             { "0.000000000000000000000000000", 0.0, ParseError::None },

//             // Sign Variations
//             { "+1.0", 1.0, ParseError::None },
//             { "-123.456", -123.456, ParseError::None },
//             { "+0.0", 0.0, ParseError::None },
//             { "--1.0", 0.0, ParseError::InputHasInvalidSymbol },
//             { "+-1.0", 0.0, ParseError::InputHasInvalidSymbol },
//             { "-+1.0", 0.0, ParseError::InputHasInvalidSymbol },

//             // Non-Representable Decimal Fractions
//             { "0.1", 0.1000000000000000055511151231257827021181583404541015625, ParseError::None },
//             { "0.2", 0.200000000000000011102230246251565404236316680908203125, ParseError::None },
//             { "0.3", 0.299999999999999988897769753748434595763683319091796875, ParseError::None },
//             { "0.7", 0.6999999999999999555910790149937383830547332763671875, ParseError::None },

//             // Trailing Zeroes
//             { "26843549.5", 26843549.5, ParseError::None },
//             { "50000002.5", 50000002.5, ParseError::None },
//             { "99999989.5", 99999989.5, ParseError::None },

//             // Error Cases
//             { "", 0.0, ParseError::InputEmpty },
//             { nullptr, 0.0, ParseError::InputEmpty },
//             { ".12.3", 0.0, ParseError::InputHasMultipleDots },

//             { "a123", 0.0, ParseError::InputHasInvalidSymbol },
//             { "0.a123", 0.0, ParseError::InputHasInvalidSymbol },
//             { ".123a", 0.0, ParseError::InputHasInvalidSymbol },
//             { "123a", 0.0, ParseError::InputHasInvalidSymbol },
//             { "12.3a", 0.0, ParseError::InputHasInvalidSymbol },
//             { "1 2", 0.0, ParseError::InputHasInvalidSymbol },

//             { "012345678912345678", 012345678912345678.0, ParseError::None },
//             { "123456789123456789", 0.0, ParseError::InputNumberTooLarge },
//             { "12345678912345678.0", 0.0, ParseError::InputNumberTooLarge },
//             { "1234567891234567.00", 0.0, ParseError::InputNumberTooLarge },
//             { "123456789123456.000", 0.0, ParseError::InputNumberTooLarge },

//             { "99999999999999999", 99999999999999999.0, ParseError::None },
//             { "-99999999999999999", -99999999999999999.0, ParseError::None },

//             // TODO2: At some point I might want these to work, but for now just make sure they don't assert
//             { " 21.123", 0.0, ParseError::InputHasInvalidSymbol },
//             { "21.123 ", 0.0, ParseError::InputHasInvalidSymbol },
//         };

//         i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
//             auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
//             if (c.err == ParseError::None) {
//                 CT_CHECK(v.hasValue(), cErr);
//                 CT_CHECK(v.value() == c.expected, cErr);

//                 CT_CHECK(!core::isnan(v.value()), cErr);
//                 CT_CHECK(!core::isinf(v.value()), cErr);
//             }
//             else {
//                 CT_CHECK(v.hasErr(), cErr);
//                 CT_CHECK(v.err() == c.err, cErr);
//             }

//             return 0;
//         });
//         CT_CHECK(ret == 0);
//     }

//     return 0;
// };

// constexpr i32 cstrToFloatNanAndInfTest() {
//     using core::ParseError;

//     {
//         struct TestCase {
//             const char* input;
//             f32 expected;
//             ParseError err;
//         };

//         constexpr TestCase cases[] = {
//             // Infinity cases:
//             { "inf", core::infinity<f32>(), ParseError::None },
//             { "-inf", -core::infinity<f32>(), ParseError::None },
//             { "INF", core::infinity<f32>(), ParseError::None },
//             { "-INF", -core::infinity<f32>(), ParseError::None },

//             // None cases:
//             { "nan", core::signalingNaN<f32>(), ParseError::None },
//             { "NaN", core::signalingNaN<f32>(), ParseError::None },
//             { "NAN", core::signalingNaN<f32>(), ParseError::None },
//         };

//         i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
//             auto v = core::cstrToFloat<f32>(c.input, u32(core::cstrLen(c.input)));
//             CT_CHECK(v.hasValue(), cErr);

//             if (core::isinf(c.expected)) {
//                 CT_CHECK(v.value() == c.expected, cErr);
//                 CT_CHECK(core::isinf(v.value()), cErr);
//             }
//             else if (core::isnan(c.expected)) {
//                 CT_CHECK(core::isnan(v.value()), cErr);
//             }

//             return 0;
//         });
//         CT_CHECK(ret == 0);
//     }

//     {
//         struct TestCase {
//             const char* input;
//             f64 expected;
//             ParseError err;
//         };

//         constexpr TestCase cases[] = {
//             // Infinity cases:
//             { "inf", core::infinity<f64>(), ParseError::None },
//             { "-inf", -core::infinity<f64>(), ParseError::None },
//             { "INF", core::infinity<f64>(), ParseError::None },
//             { "-INF", -core::infinity<f64>(), ParseError::None },

//             // None cases:
//             { "nan", core::signalingNaN<f64>(), ParseError::None },
//             { "NaN", core::signalingNaN<f64>(), ParseError::None },
//             { "NAN", core::signalingNaN<f64>(), ParseError::None },
//         };

//         i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
//             auto v = core::cstrToFloat<f64>(c.input, u32(core::cstrLen(c.input)));
//             CT_CHECK(v.hasValue(), cErr);

//             if (core::isinf(c.expected)) {
//                 CT_CHECK(v.value() == c.expected, cErr);
//                 CT_CHECK(core::isinf(v.value()), cErr);
//             }
//             else if (core::isnan(c.expected)) {
//                 CT_CHECK(core::isnan(v.value()), cErr);
//             }

//             return 0;
//         });
//         CT_CHECK(ret == 0);
//     }

//     return 0;
// }

i32 runCstrConv_CstrToFloat() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(exponentNotationTest);
    if (runTest(tInfo, exponentNotationTest) != 0) { ret = -1; }
    // tInfo.name = FN_NAME_TO_CPTR(cstrToFloatNanAndInfTest);
    // if (runTest(tInfo, cstrToFloatNanAndInfTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCstrConvTestsSuite() {
    RunTestCompileTime(exponentNotationTest);
    // RunTestCompileTime(cstrToFloatNanAndInfTest);

    return 0;
}
