#include "t-index.h"

#include "common/ryu_reference_impl/ryu.h"
#include "common/ryu_reference_impl/ryu_parse.h"

constexpr i32 checkWithOriginalNormal(auto in, const char* got, u32 gotN, const char* cErr) {
    IS_NOT_CONST_EVALUATED {
        if constexpr (std::is_same_v<decltype(in), f32>) {
            constexpr u32 BUFF_LEN = 64;
            char buf[BUFF_LEN];
            u32 n = ryu::f2s_buffered_n(in, buf);

            i32 cmpResult = core::memcmp(buf, n, got, gotN);
            CT_CHECK(cmpResult == 0, cErr);
        }
        else if constexpr (std::is_same_v<decltype(in), f64>) {
            constexpr u32 BUFF_LEN = 64;
            char buf[BUFF_LEN];
            u32 n = ryu::d2s_buffered_n(in, buf);

            i32 cmpResult = core::memcmp(buf, n, got, gotN);
            CT_CHECK(cmpResult == 0, cErr);
        }
        else {
            static_assert(core::always_false<decltype(in)>, "invalid type passed");
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
            { 0.0f, "0E0" },
            { -0.0f, "-0E0" },
            { 1.0f, "1E0" },
            { -1.0f, "-1E0" },
            { 1.2f, "1.2E0" },
            { 1.23f, "1.23E0" },
            { 1.234f, "1.234E0" },
            { 1.2345f, "1.2345E0" },
            { 1.23456f, "1.23456E0" },
            { 1.234567f, "1.234567E0" },
            { 1.2345678f, "1.2345678E0" },

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
            { 7.038531E-26f, "7.038531E-26" }, // TODO2: verify on MSVC, which might have a problem with this, and round to ASSERT_F2S("7.038531E-26", 7.0385309E-26f);
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
            u32 gotN = core::floatToCstr(c.input, gotBuf, BUFF_LEN);
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
            { 0.0, "0E0" },
            { -0.0, "-0E0" },
            { 1.0, "1E0" },
            { -1.0, "-1E0" },
            { 1.2, "1.2E0" },
            { 1.23, "1.23E0" },
            { 1.234, "1.234E0" },
            { 1.2345, "1.2345E0" },
            { 1.23456, "1.23456E0" },
            { 1.234567, "1.234567E0" },
            { 1.2345678, "1.2345678E0" },
            { 1.23456789, "1.23456789E0" },
            { 1.234567895, "1.234567895E0" },
            { 1.2345678901, "1.2345678901E0" },
            { 1.23456789012, "1.23456789012E0" },
            { 1.234567890123, "1.234567890123E0" },
            { 1.2345678901234, "1.2345678901234E0" },
            { 1.23456789012345, "1.23456789012345E0" },
            { 1.234567890123456, "1.234567890123456E0" },
            { 1.2345678901234567, "1.2345678901234567E0" },

            // Test 32-bit chunking
            { 4.294967294, "4.294967294E0" }, // 2^32 - 2
            { 4.294967295, "4.294967295E0" }, // 2^32 - 1
            { 4.294967296, "4.294967296E0" }, // 2^32
            { 4.294967297, "4.294967297E0" }, // 2^32 + 1
            { 4.294967298, "4.294967298E0" }, // 2^32 + 2

            // Trailing zeros
            { 2.98023223876953125E-8, "2.9802322387695312E-8" },

            // pow 5 corner cases
            { 5.764607523034235E39, "5.764607523034235E39" },
            { 1.152921504606847E40, "1.152921504606847E40" },
            { 2.305843009213694E40, "2.305843009213694E40" },

            // Small integer test cases
            { 9007199254740991.0, "9.007199254740991E15" }, // 2^53-1,
            { 9007199254740992.0, "9.007199254740992E15" }, // 2^53,

            { 1.0e+0, "1E0" },
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
            { 1.0e+0, "1E0" },
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
            { 8.0, "8E0" },
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
            { 1.2345678, "1.2345678E0" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::floatToCstr(c.input, gotBuf, BUFF_LEN);
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

#include <sstream>

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

            // FIXME: Fix this with the check with original
            // Overflow cases
            // { core::infinity<f32>(), "Inf" },
            // { -core::infinity<f32>(), "-Inf" },

            // Underflow to zero
            { 0.0f, "0E0" },
            { -0.0f, "-0E0" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for f32 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = core::floatToCstr(c.input, gotBuf, BUFF_LEN);
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    // {
    //     struct TestCase {
    //         f64 input;
    //         const char* expected;
    //     };

    //     constexpr TestCase cases[] = {

    //     };

    //     i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [](auto& c, const char* cErr) {
    //         constexpr u32 BUFF_LEN = 64;
    //         char gotBuf[BUFF_LEN];
    //         u32 gotN = core::floatToCstr(c.input, gotBuf, BUFF_LEN);
    //         gotBuf[gotN] = '\0';

    //         i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
    //         CT_CHECK(cmpResult == 0, cErr);

    //         CT_CHECK(checkWithOriginalNormal(c.input, gotBuf, gotN, cErr) == 0);

    //         return 0;
    //     });
    //     CT_CHECK(ret == 0);
    // }

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

    // FIXME: continue testing

    return ret;
}

constexpr i32 runCompiletimeCstrConv_FloatToCstr_TestsSuite() {
    RunTestCompileTime(toExponentNotationTest);

    return 0;
}
