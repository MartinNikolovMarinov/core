#include "t-index.h"

#include "common/ryu_reference_impl/ryu.h"
#include "common/ryu_reference_impl/ryu_parse.h"

constexpr i32 checkWithOriginalNormal(auto in, const char* got, u32 gotN, const char* cErr) {
    IS_NOT_CONST_EVALUATED {
        constexpr u32 BUFF_LEN = 64;
        char buf[BUFF_LEN];
        u32 n;

        if constexpr (std::is_same_v<decltype(in), f32>)
            n = u32(ryu::f2s_buffered_n(in, buf));
        else if constexpr (std::is_same_v<decltype(in), f64>)
            n = u32(ryu::d2s_buffered_n(in, buf));
        else
            static_assert(core::always_false<decltype(in)>, "invalid type passed");

        // Handle difference for cases that end with zero exponent = E0
        if (n > 2) {
            if (buf[n - 2] == 'E' && buf[n - 1] == '0') {
                n -= 2;
            }
        }

        if (!core::isinf(in)) {
            i32 cmpResult = core::memcmp(buf, n, got, gotN);
            CT_CHECK(cmpResult == 0, cErr);
        }
        else {
            if (in > 0) {
                CT_CHECK(core::memcmp(buf, n, "Infinity", core::cstrLen("Infinity")) == 0, cErr);
                CT_CHECK(core::memcmp(got, gotN, "inf", core::cstrLen("inf")) == 0, cErr);
            }
            else {
                CT_CHECK(core::memcmp(buf, n, "-Infinity", core::cstrLen("-Infinity")) == 0, cErr);
                CT_CHECK(core::memcmp(got, gotN, "-inf", core::cstrLen("-inf")) == 0, cErr);
            }
        }
    }

    return 0;
}

constexpr i32 toExponentNotationTest() {
    {
        struct TestCase {
            f32 input;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            // Basic
            { 0.0f, "0" },
            { -0.0f, "-0" },
            { 1.0f, "1" },
            { -1.0f, "-1" },
            { 1.2f, "1.2" },
            { 1.23f, "1.23" },
            { 1.234f, "1.234" },
            { 1.2345f, "1.2345" },
            { 1.23456f, "1.23456" },
            { 1.234567f, "1.234567" },
            { 1.2345678f, "1.2345678" },

            // Exact value round even
            { 3.0540412E5f, "3.0540412E5" },
            { 8.0990312E3f, "8.0990312E3" },

            // Lots of trailing zeroes
            { 2.4414062E-4f, "2.4414062E-4" },
            { 2.4414062E-3f, "2.4414062E-3" },
            { 4.3945312E-3f, "4.3945312E-3" },
            { 6.3476562E-3f, "6.3476562E-3" },

            // pow 5 corner cases
            { 6.7108864E17f, "6.7108864E17" },
            { 1.3421773E18f, "1.3421773E18" },
            { 2.6843546E18f, "2.6843546E18" },

            // Hard cases
            { 4.7223665E21f, "4.7223665E21" },
            { 8388608.0f, "8.388608E6" },
            { 1.6777216E7f, "1.6777216E7" },
            { 3.3554436E7f, "3.3554436E7" },
            { 6.7131496E7f, "6.7131496E7" },
            { 1.9310392E-38f, "1.9310392E-38" },
            { -2.47E-43f, "-2.47E-43" },
            { 1.993244E-38f, "1.993244E-38" },
            { 4103.9003f, "4.1039004E3" },
            { 5.3399997E9f, "5.3399997E9" },
            { 6.0898E-39f, "6.0898E-39" },
            { 0.0010310042f, "1.0310042E-3" },
            { 2.8823261E17f, "2.882326E17" },
            { 7.038531E-26f, "7.038531E-26" },
            { 9.2234038E17f, "9.223404E17" },
            { 6.7108872E7f, "6.710887E7" },
            { 1.0E-44f, "1E-44" },
            { 2.816025E14f, "2.816025E14" },
            { 9.223372E18f, "9.223372E18" },
            { 1.5846085E29f, "1.5846086E29" },
            { 1.1811161E19f, "1.1811161E19" },
            { 5.368709E18f, "5.368709E18" },
            { 4.6143165E18f, "4.6143166E18" },
            { 0.007812537f, "7.812537E-3" },
            { 1.4E-45f, "1E-45" },
            { 1.18697724E20f, "1.18697725E20" },
            { 1.00014165E-36f, "1.00014165E-36" },
            { 200.0f, "2E2" },
            { 3.3554432E7f, "3.3554432E7" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            f64 input;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            // Basic
            { 0.0, "0" },
            { -0.0, "-0" },
            { 1.0, "1" },
            { -1.0, "-1" },
            { 1.2, "1.2" },
            { 1.23, "1.23" },
            { 1.234, "1.234" },
            { 1.2345, "1.2345" },
            { 1.23456, "1.23456" },
            { 1.234567, "1.234567" },
            { 1.2345678, "1.2345678" },
            { 1.23456789, "1.23456789" },
            { 1.234567895, "1.234567895" },
            { 1.2345678901, "1.2345678901" },
            { 1.23456789012, "1.23456789012" },
            { 1.234567890123, "1.234567890123" },
            { 1.2345678901234, "1.2345678901234" },
            { 1.23456789012345, "1.23456789012345" },
            { 1.234567890123456, "1.234567890123456" },
            { 1.2345678901234567, "1.2345678901234567" },

            // Test 32-bit chunking
            { 4.294967294, "4.294967294" }, // 2^32 - 2
            { 4.294967295, "4.294967295" }, // 2^32 - 1
            { 4.294967296, "4.294967296" }, // 2^32
            { 4.294967297, "4.294967297" }, // 2^32 + 1
            { 4.294967298, "4.294967298" }, // 2^32 + 2

            // Trailing zeros
            { 2.98023223876953125E-8, "2.9802322387695312E-8" },

            // pow 5 corner cases
            { 5.764607523034235E39, "5.764607523034235E39" },
            { 1.152921504606847E40, "1.152921504606847E40" },
            { 2.305843009213694E40, "2.305843009213694E40" },

            // Small integer test cases
            { 9007199254740991.0, "9.007199254740991E15" }, // 2^53-1,
            { 9007199254740992.0, "9.007199254740992E15" }, // 2^53,

            { 1.0e+0, "1" },
            { 1.2e+1, "1.2E1" },
            { 1.23e+2, "1.23E2" },
            { 1.234e+3, "1.234E3" },
            { 1.2345e+4, "1.2345E4" },
            { 1.23456e+5, "1.23456E5" },
            { 1.234567e+6, "1.234567E6" },
            { 1.2345678e+7, "1.2345678E7" },
            { 1.23456789e+8, "1.23456789E8" },
            { 1.23456789e+9, "1.23456789E9" },
            { 1.234567895e+9, "1.234567895E9" },
            { 1.2345678901e+10, "1.2345678901E10" },
            { 1.23456789012e+11, "1.23456789012E11" },
            { 1.234567890123e+12, "1.234567890123E12" },
            { 1.2345678901234e+13, "1.2345678901234E13" },
            { 1.23456789012345e+14, "1.23456789012345E14" },
            { 1.234567890123456e+15, "1.234567890123456E15" },

            // 10^i
            { 1.0e+0, "1" },
            { 1.0e+1, "1E1" },
            { 1.0e+2, "1E2" },
            { 1.0e+3, "1E3" },
            { 1.0e+4, "1E4" },
            { 1.0e+5, "1E5" },
            { 1.0e+6, "1E6" },
            { 1.0e+7, "1E7" },
            { 1.0e+8, "1E8" },
            { 1.0e+9, "1E9" },
            { 1.0e+10, "1E10" },
            { 1.0e+11, "1E11" },
            { 1.0e+12, "1E12" },
            { 1.0e+13, "1E13" },
            { 1.0e+14, "1E14" },
            { 1.0e+15, "1E15" },

            // 10^15 + 10^i
            { 1.0e+15 + 1.0e+0, "1.000000000000001E15" },
            { 1.0e+15 + 1.0e+1, "1.00000000000001E15" },
            { 1.0e+15 + 1.0e+2, "1.0000000000001E15" },
            { 1.0e+15 + 1.0e+3, "1.000000000001E15" },
            { 1.0e+15 + 1.0e+4, "1.00000000001E15" },
            { 1.0e+15 + 1.0e+5, "1.0000000001E15" },
            { 1.0e+15 + 1.0e+6, "1.000000001E15" },
            { 1.0e+15 + 1.0e+7, "1.00000001E15" },
            { 1.0e+15 + 1.0e+8, "1.0000001E15" },
            { 1.0e+15 + 1.0e+9, "1.000001E15" },
            { 1.0e+15 + 1.0e+10, "1.00001E15" },
            { 1.0e+15 + 1.0e+11, "1.0001E15" },
            { 1.0e+15 + 1.0e+12, "1.001E15" },
            { 1.0e+15 + 1.0e+13, "1.01E15" },
            { 1.0e+15 + 1.0e+14, "1.1E15" },

            // Largest power of 2 <= 10^(i+1)
            { 8.0, "8" },
            { 64.0, "6.4E1" },
            { 512.0, "5.12E2" },
            { 8192.0, "8.192E3" },
            { 65536.0, "6.5536E4" },
            { 524288.0, "5.24288E5" },
            { 8388608.0, "8.388608E6" },
            { 67108864.0, "6.7108864E7" },
            { 536870912.0, "5.36870912E8" },
            { 8589934592.0, "8.589934592E9" },
            { 68719476736.0, "6.8719476736E10" },
            { 549755813888.0, "5.49755813888E11" },
            { 8796093022208.0, "8.796093022208E12" },
            { 70368744177664.0, "7.0368744177664E13" },
            { 562949953421312.0, "5.62949953421312E14" },
            { 9007199254740992.0, "9.007199254740992E15" },

            // 1000 * (Largest power of 2 <= 10^(i+1))
            { 8.0e+3, "8E3" },
            { 64.0e+3, "6.4E4" },
            { 512.0e+3, "5.12E5" },
            { 8192.0e+3, "8.192E6" },
            { 65536.0e+3, "6.5536E7" },
            { 524288.0e+3, "5.24288E8" },
            { 8388608.0e+3, "8.388608E9" },
            { 67108864.0e+3, "6.7108864E10" },
            { 536870912.0e+3, "5.36870912E11" },
            { 8589934592.0e+3, "8.589934592E12" },
            { 68719476736.0e+3, "6.8719476736E13" },
            { 549755813888.0e+3, "5.49755813888E14" },
            { 8796093022208.0e+3, "8.796093022208E15" },

            // Hard cases
            { -2.109808898695963E16, "-2.109808898695963E16" },
            { 4.940656E-318, "4.940656E-318" },
            { 1.18575755E-316, "1.18575755E-316" },
            { 2.989102097996E-312, "2.989102097996E-312" },
            { 9.0608011534336E15, "9.0608011534336E15" },
            { 4.708356024711512E18, "4.708356024711512E18" },
            { 9.409340012568248E18, "9.409340012568248E18" },
            { 1.2345678, "1.2345678" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 toSpecialValuesTest() {
    {
        struct TestCase {
            f32 input;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { 1.401298e-45f, "1E-45" }, // Smallest positive subnormal float
            { -1.401298e-45f, "-1E-45" },
            { 1.4e-45f, "1E-45" },

            // Largest representable value
            { core::limitMax<f32>(), "3.4028235E38" },
            { -core::limitMax<f32>(), "-3.4028235E38" },

            // Smallest representable value
            { core::limitMin<f32>(), "1.1754944E-38" },
            { -core::limitMin<f32>(), "-1.1754944E-38" },

            { core::infinity<f32>(), "inf" },
            { -core::infinity<f32>(), "-inf" },

            { core::quietNaNF32(), "NaN" },
            { core::signalingNaNF32(), "NaN" },

            // Underflow to zero
            { 0.0f, "0" },
            { -0.0f, "-0" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            f64 input;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            // Subnormal/Denormalized Numbers
            { 4.9406564584124654e-324, "5E-324" }, // Smallest positive subnormal double
            { -4.9406564584124654e-324, "-5E-324" },
            { 4.9e-324, "5E-324" },

            // Largest representable value
            { core::limitMax<f64>(), "1.7976931348623157E308" }, // 1.7976931348623157E308 ok
            { -core::limitMax<f64>(), "-1.7976931348623157E308" }, // -1.7976931348623157E308

            // Smallest representable normalized value
            { core::limitMin<f64>(), "2.2250738585072014E-308" },
            { -core::limitMin<f64>(), "-2.2250738585072014E-308" },

            { core::infinity<f64>(), "inf" },
            { -core::infinity<f64>(), "-inf" },

            { core::quietNaNF64(), "NaN" },
            { core::signalingNaNF64(), "NaN" },

            // Underflow to zero
            { 0.0, "0" },
            { -0.0, "-0" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 shortBufferWritesTest() {
    using core::ParseError;

    {
        struct TestCase {
            f32 input;
            u32 bufferSize;
        };

        constexpr TestCase cases[] = {
            { 0.0f, 0 },
            { 0.0f, 1 },
            { -1.23456f, 0 },
            { 3.4028235e38f, 5 },
            { 9.9f, 2 },
            { 1.2e2f, 3 },
            { 1.2e2f, 4 },
            { 98.0f, 3 },
            { 1.2e1f, 4 },
            { 3.4028235e38f, 1 },
            { 3.4028235e38f, 2 },
            { 3.4028235e38f, 3 },
            { 3.4028235e38f, 4 },
            { 123.456f, 1 },
            { 123.456f, 2 },
            { 123.456f, 3 },
            { 123.456f, 4 },
            { 123.456f, 5 },
            { 12.34f, 1 },
            { 12.34f, 2 },
            { 5.0f, 0 },
            { 1.23456f, 1 },
            { 1.23456f, 2 },
            { 9.0f, 0 },
            { 1.2e1f, 4 },
            { 1.23e-10f, 1 },
            { 1.23e20f, 1 },
            { 1.23e20f, 2 },
            { 1.23e1f, 1 },
            { 1e-20f, 1 },
            { 1e-20f, 2 },
            { 1e-20f, 3 },
            { 1e20f, 1 },
            { 1e20f, 2 },
            { 1e20f, 3 },
            { 3.4e38f, 1 },
            { 3.4e38f, 2 },
            { 3.4e38f, 3 },
            { 3.4e38f, 4 },
            { 3.4e38f, 5 },
            { -9.9f, 1 },
            { -9.9f, 2 },
            { -9.9f, 3 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 short writes at index: ", cases, [](auto& c, const char* cErr) {
            char buff[64];
            auto got = core::floatToCstr(c.input, buff, c.bufferSize);
            CT_CHECK(got.hasErr(), cErr);
            CT_CHECK(got.err() == core::ParseError::OutputBufferTooSmall, cErr);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            f64 input;
            u32 bufferSize;
        };

        constexpr TestCase cases[] = {
            // Zero case
            { 0.0, 0 },
            { 0.0, 1 },

            // Sign fails immediately
            { -1.23456, 0 },

            // Trigger (mantissa >> 32) != 0 and fail at first writeAt
            { core::limitMax<f64>(), 1 },
            // Same big number, slightly bigger buffers to fail at subsequent writeAt calls
            { core::limitMax<f64>(), 2 },
            { core::limitMax<f64>(), 3 },
            { core::limitMax<f64>(), 4 },

            // While(mantissa2 >= 10000) block fail
            // Use a large number to ensure multiple divisions and writeAt calls
            { 1e20, 1 },
            { 1e20, 2 },
            { 1e20, 3 },
            { 1e20, 4 },

            // If (mantissa2 >= 100) fail
            { 123.456, 1 },
            { 123.456, 2 },
            { 123.456, 3 },

            // If (mantissa2 >= 10) fail (two writes)
            { 12.34, 1 },
            { 12.34, 2 },

            // Single digit mantissa fail
            { 5.0, 0 },

            // Fail writing decimal point
            { 1.23456, 1 }, // can’t write full "1."
            { 1.23456, 2 }, // can’t write "1.2"

            // Fail advance(1) case (mantissaLen == 1)
            { 9.0, 0 },

            // Fail writing 'E'
            { 1.23e10, 1 },

            // Fail writing '-' for negative exponent
            { 1.23e-10, 1 },

            // exp >= 100 fail
            { 1.23e200, 1 },
            { 1.23e200, 2 },
            { 1.23e200, 3 },

            // exp >= 10 fail
            { 1.23e20, 1 },
            { 1.23e20, 2 },

            // exp < 10 fail
            { 1.23e1, 1 },

            // Extreme small exponent
            { 1.23e-100, 1 },
            { 1.23e-100, 2 },
            { 1.23e-100, 3 },

            // Extreme large exponent
            { 1.23e300, 1 },
            { 1.23e300, 2 },

            // Very small numbers
            { 1e-200, 1 },
            { 1e-200, 2 },

            // Boundary cases for digit handling
            { 9.9, 1 },
            { 9.9, 2 },
            { 9.9, 3 },

            // More scenarios for exp >= 10
            { 1.23e15, 1 },
            { 1.23e15, 2 },
            { 1.23e15, 3 },

            // Fail with different buffer sizes.
            { core::limitMin<f64>(), 0 },
            { core::limitMin<f64>(), 1 },
            { core::limitMin<f64>(), 2 },
            { core::limitMin<f64>(), 3 },
            { core::limitMin<f64>(), 4 },
            { core::limitMin<f64>(), 5 },
            { core::limitMin<f64>(), 6 },
            { core::limitMin<f64>(), 7 },
            { core::limitMin<f64>(), 8 },
            { core::limitMin<f64>(), 9 },
            { core::limitMin<f64>(), 10 },
            { core::limitMin<f64>(), 11 },
            { core::limitMin<f64>(), 12 },
            { core::limitMin<f64>(), 13 },
            { core::limitMin<f64>(), 14 },
            { core::limitMin<f64>(), 15 },
            { core::limitMin<f64>(), 15 },
            { core::limitMin<f64>(), 16 },
            { core::limitMin<f64>(), 17 },
            { core::limitMin<f64>(), 18 },
            { core::limitMin<f64>(), 19 },
            { core::limitMin<f64>(), 20 },
            { core::limitMin<f64>(), 21 },
            { core::limitMin<f64>(), 22 },
        };

        i32 ret = core::testing::executeTestTable("test case failed for overflow at index: ", cases, [](auto& c, const char* cErr) {
            char buff[64];
            auto got = core::floatToCstr(c.input, buff, c.bufferSize);
            CT_CHECK(got.hasErr(), cErr);
            CT_CHECK(got.err() == core::ParseError::OutputBufferTooSmall);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 runCstrConv_FloatToCstr_TestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(toExponentNotationTest);
    if (runTest(tInfo, toExponentNotationTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(toSpecialValuesTest);
    if (runTest(tInfo, toSpecialValuesTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(shortBufferWritesTest);
    if (runTest(tInfo, shortBufferWritesTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeCstrConv_FloatToCstr_TestsSuite() {
    RunTestCompileTime(toExponentNotationTest);
    RunTestCompileTime(toSpecialValuesTest);
    RunTestCompileTime(shortBufferWritesTest);

    return 0;
}
