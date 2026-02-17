#include "t-index.h"

#include "common/ryu_reference_impl/ryu.h"
#include "common/ryu_reference_impl/ryu_parse.h"
#include "common/ryu_reference_impl/test_tables.h"

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

constexpr i32 checkWithOriginalPrecision(f64 in, u32 precission, const char* got, u32 gotN, const char* cErr) {
    IS_NOT_CONST_EVALUATED {
        constexpr u32 BUFF_LEN = core::CORE_KILOBYTE * 2;
        char buf[BUFF_LEN];
        u32 n = u32(ryu::d2fixed_buffered_n(in, precission, buf));

        if (core::isinf(in)) {
            if (in > 0) {
                CT_CHECK(core::memcmp(buf, n, "Infinity", core::cstrLen("Infinity")) == 0, cErr);
                CT_CHECK(core::memcmp(got, gotN, "inf", core::cstrLen("inf")) == 0, cErr);
            }
            else {
                CT_CHECK(core::memcmp(buf, n, "-Infinity", core::cstrLen("-Infinity")) == 0, cErr);
                CT_CHECK(core::memcmp(got, gotN, "-inf", core::cstrLen("-inf")) == 0, cErr);
            }
        }
        else if (core::isnan(in)) {
            // This is a very strange inconsistency in the implementation, to return nan or even nan(snan) here and NaN
            // in the f2s and d2s variants. I keep it consistant NaN.
            CT_CHECK(buf[0] == 'n');
            CT_CHECK(buf[1] == 'a');
            CT_CHECK(buf[2] == 'n');
            CT_CHECK(core::memcmp(got, gotN, "NaN", core::cstrLen("NaN")) == 0, cErr);
        }
        else {
            i32 cmpResult = core::memcmp(buf, n, got, gotN);
            CT_CHECK(cmpResult == 0, cErr);
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
            u32 gotN = Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
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
            u32 gotN = Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
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

constexpr i32 toFixedNotationTest() {
    struct TestCase {
        f64 input;
        u32 precision;
        const char* expected;
    };

    TestCase cases[] = {
        // Basic
        { 0.0, 0, "0" },
        { 0.0, 1, "0.0" },
        { 0.0, 2, "0.00" },
        { 0.0, 3, "0.000" },
        { -0.0, 0, "-0" },
        { -0.0, 1, "-0.0" },
        { -0.0, 2, "-0.00" },
        { -0.0, 3, "-0.000" },
        { 1.0, 0, "1" },
        { 1.0, 1, "1.0" },
        { 1.0, 2, "1.00" },
        { 1.0, 3, "1.000" },
        { -1.0, 0, "-1" },
        { -1.0, 1, "-1.0" },
        { -1.0, 2, "-1.00" },
        { -1.0, 3, "-1.000" },
        { 0.0, 50, "0.00000000000000000000000000000000000000000000000000" },
        { -12.0, 50, "-12.00000000000000000000000000000000000000000000000000" },
        { -123.456789, 6, "-123.456789" },
        { 1.2345678901234567890, 1, "1.2" },
        { 1.2345678901234567890, 2, "1.23" },
        { 1.2345678901234567890, 3, "1.235" },
        { 1.2345678901234567890, 4, "1.2346" },
        { 1.2345678901234567890, 5, "1.23457" },
        { 1.2345678901234567890, 6, "1.234568" },
        { 1.2345678901234567890, 7, "1.2345679" },
        { 1.2345678901234567890, 8, "1.23456789" },
        { 1.2345678901234567890, 9, "1.234567890" },
        { 1.2345678901234567890, 10, "1.2345678901" },
        { 1.2345678901234567890, 11, "1.23456789012" },
        { 1.2345678901234567890, 12, "1.234567890123" },
        { 1.2345678901234567890, 13, "1.2345678901235" },
        { 1.2345678901234567890, 14, "1.23456789012346" },
        { 1.2345678901234567890, 15, "1.234567890123457" },
        { 1.2345678901234567890, 16, "1.2345678901234567" },
        { 1.2345678901234567890, 17, "1.23456789012345669" }, // cutoff point.

        // Large number with 0 precision after the dot
        { 3.2910091147154864e+63, 0, "3291009114715486435425664845573426149758869524108446525879746560" },

        // Rounding edge cases
        { 0.0009, 5, "0.00090" },
        { 0.0009, 4, "0.0009" },
        { 0.0009, 3, "0.001" },
        { 0.0099, 3, "0.010" },
        { 0.00999, 4, "0.0100" },
        { 7.018232e-82, 6, "0.000000" },

        // Min/Max
        {
            core::bitCast<f64>(core::createFloat64(1, 0, false)), 1074,
            "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000049406564584124654417656879286822137"
            "236505980261432476442558568250067550727020875186529983636163599237979656469544571773092665"
            "671035593979639877479601078187812630071319031140452784581716784898210368871863605699873072"
            "305000638740915356498438731247339727316961514003171538539807412623856559117102665855668676"
            "818703956031062493194527159149245532930545654440112748012970999954193198940908041656332452"
            "475714786901472678015935523861155013480352649347201937902681071074917033322268447533357208"
            "324319360923828934583680601060115061698097530783422773183292479049825247307763759272478746"
            "560847782037344696995336470179726777175851256605511991315048911014510378627381672509558373"
            "89733598993664809941164205702637090279242767544565229087538682506419718265533447265625"
        },
        {
            core::bitCast<f64>(core::createFloat64(0xFFFFFFFFFFFFFu, 2046, false)), 0,
            "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558"
            "632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245"
            "490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168"
            "738177180919299881250404026184124858368"
        },

        // Round to even
        { 0.125, 3, "0.125" },
        { 0.125, 2, "0.12"  },
        { 0.375, 3, "0.375" },
        { 0.375, 2, "0.38"  },

        // Round to even integer
        { 2.5, 1, "2.5" },
        { 2.5, 0, "2"   },
        { 3.5, 1, "3.5" },
        { 3.5, 0, "4"   },

        // Non round to even
        { 0.748046875, 3, "0.748"  },
        { 0.748046875, 2, "0.75"   },
        { 0.748046875, 1, "0.7"    }, // 0.75 would round to "0.8", but this is smaller
        { 0.2509765625, 3, "0.251" },
        { 0.2509765625, 2, "0.25"  },
        { 0.2509765625, 1, "0.3"   }, // 0.25 would round to "0.2", but this is larger
        { core::bitCast<f64>(core::createFloat64(1, 1021, false)), 54, "0.250000000000000055511151231257827021181583404541015625" },
        { core::bitCast<f64>(core::createFloat64(1, 1021, false)),  3, "0.250" },
        { core::bitCast<f64>(core::createFloat64(1, 1021, false)),  2, "0.25"  },
        { core::bitCast<f64>(core::createFloat64(1, 1021, false)),  1, "0.3"   }, // 0.25 would round to "0.2", but this is larger (again)

        // Exhaustive precision testing
        { 1729.142857142857, 21, "1729.142857142857110375189" },
        { 1729.142857142857, 20, "1729.14285714285711037519" },
        { 1729.142857142857, 19, "1729.1428571428571103752" },
        { 1729.142857142857, 18, "1729.142857142857110375" },
        { 1729.142857142857, 17, "1729.14285714285711038" },
        { 1729.142857142857, 16, "1729.1428571428571104" },
        { 1729.142857142857, 15, "1729.142857142857110" },
        { 1729.142857142857, 14, "1729.14285714285711" },
        { 1729.142857142857, 13, "1729.1428571428571" },
        { 1729.142857142857, 12, "1729.142857142857" },
        { 1729.142857142857, 11, "1729.14285714286" },
        { 1729.142857142857, 10, "1729.1428571429" },
        { 1729.142857142857,  9, "1729.142857143" },
        { 1729.142857142857,  8, "1729.14285714" },
        { 1729.142857142857,  7, "1729.1428571" },
        { 1729.142857142857,  6, "1729.142857" },
        { 1729.142857142857,  5, "1729.14286" },
        { 1729.142857142857,  4, "1729.1429" },
        { 1729.142857142857,  3, "1729.143" },
        { 1729.142857142857,  2, "1729.14" },
        { 1729.142857142857,  1, "1729.1" },
        { 1729.142857142857,  0, "1729" },

        // Carrying
        {   0.0009, 4,   "0.0009" },
        {   0.0009, 3,   "0.001"  },
        {   0.0029, 4,   "0.0029" },
        {   0.0029, 3,   "0.003"  },
        {   0.0099, 4,   "0.0099" },
        {   0.0099, 3,   "0.010"  },
        {   0.0299, 4,   "0.0299" },
        {   0.0299, 3,   "0.030"  },
        {   0.0999, 4,   "0.0999" },
        {   0.0999, 3,   "0.100"  },
        {   0.2999, 4,   "0.2999" },
        {   0.2999, 3,   "0.300"  },
        {   0.9999, 4,   "0.9999" },
        {   0.9999, 3,   "1.000"  },
        {   2.9999, 4,   "2.9999" },
        {   2.9999, 3,   "3.000"  },
        {   9.9999, 4,   "9.9999" },
        {   9.9999, 3,  "10.000"  },
        {  29.9999, 4,  "29.9999" },
        {  29.9999, 3,  "30.000"  },
        {  99.9999, 4,  "99.9999" },
        {  99.9999, 3, "100.000"  },
        { 299.9999, 4, "299.9999" },
        { 299.9999, 3, "300.000"  },
        { 99999.9999999, 4, "100000.0000" },

        {   0.09, 2,   "0.09" },
        {   0.09, 1,   "0.1"  },
        {   0.29, 2,   "0.29" },
        {   0.29, 1,   "0.3"  },
        {   0.99, 2,   "0.99" },
        {   0.99, 1,   "1.0"  },
        {   2.99, 2,   "2.99" },
        {   2.99, 1,   "3.0"  },
        {   9.99, 2,   "9.99" },
        {   9.99, 1,  "10.0"  },
        {  29.99, 2,  "29.99" },
        {  29.99, 1,  "30.0"  },
        {  99.99, 2,  "99.99" },
        {  99.99, 1, "100.0"  },
        { 299.99, 2, "299.99" },
        { 299.99, 1, "300.0"  },

        {   0.9, 1,   "0.9" },
        {   0.9, 0,   "1"   },
        {   2.9, 1,   "2.9" },
        {   2.9, 0,   "3"   },
        {   9.9, 1,   "9.9" },
        {   9.9, 0,  "10"   },
        {  29.9, 1,  "29.9" },
        {  29.9, 0,  "30"   },
        {  99.9, 1,  "99.9" },
        {  99.9, 0, "100"   },
        { 299.9, 1, "299.9" },
        { 299.9, 0, "300"   },

        // Rounding result to zero
        { 0.004, 3, "0.004" },
        { 0.004, 2, "0.00"  },
        { 0.4, 1, "0.4"     },
        { 0.4, 0, "0"       },
        { 0.5, 1, "0.5"     },
        { 0.5, 0, "0"       },
    };

    constexpr u32 BUFF_LEN = core::CORE_KILOBYTE * 2;
    char gotBuf[BUFF_LEN];

    i32 ret = core::testing::executeTestTable("test case failed for f64 at index: ", cases, [&](auto& c, const char* cErr) {
        core::memset(gotBuf, char(9), BUFF_LEN);

        u32 gotN = Unpack(core::floatToFixedCstr(c.input, c.precision, gotBuf, BUFF_LEN));
        gotBuf[gotN] = '\0';

        i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
        CT_CHECK(cmpResult == 0, cErr);

        CT_CHECK(checkWithOriginalPrecision(c.input, c.precision, gotBuf, gotN, cErr) == 0);

        return 0;
    });

    return ret;
}

i32 toFixedNotationAllPowersOf10Test() {
    // This can totally be a constexpr, but I have to address this limitation, which I don't want to do.
    //  error: ‘constexpr’ evaluation operation count exceeds limit of 33554432 (use ‘-fconstexpr-ops-limit=’ to increase the limit)

    constexpr u32 BUFF_LEN = core::CORE_KILOBYTE * 3;
    char gotBuf[BUFF_LEN];

    for (auto tc : g_allPowersOf10) {
        core::memset(gotBuf, char(9), BUFF_LEN);
        u32 gotN = Unpack(core::floatToFixedCstr(tc.value, tc.precision, gotBuf, BUFF_LEN));
        gotBuf[gotN] = '\0';
        i32 cmpResult = core::memcmp(gotBuf, gotN, tc.expectedStr, core::cstrLen(tc.expectedStr));
        CT_CHECK(cmpResult == 0);
    }

    return 0;
}

i32 toFixedNotationAllBinaryExponentsTest() {
    // This can totally be a constexpr, but I have to address this limitation, which I don't want to do.
    //  error: ‘constexpr’ evaluation operation count exceeds limit of 33554432 (use ‘-fconstexpr-ops-limit=’ to increase the limit)

    constexpr u32 BUFF_LEN = core::CORE_KILOBYTE * 3;
    char gotBuf[BUFF_LEN];

    for (auto tc : g_allBinaryExponents) {
        core::memset(gotBuf, char(9), BUFF_LEN);
        u32 gotN = Unpack(core::floatToFixedCstr(tc.value, tc.precision, gotBuf, BUFF_LEN));
        gotBuf[gotN] = '\0';
        i32 cmpResult = core::memcmp(gotBuf, gotN, tc.expectedStr, core::cstrLen(tc.expectedStr));
        CT_CHECK(cmpResult == 0);
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
            u32 gotN = Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
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
            u32 gotN = Unpack(core::floatToCstr(c.input, gotBuf, BUFF_LEN));
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
            u32 precision;
            const char* expected;
        };

        constexpr TestCase cases[] = {
            { core::infinity<f64>(), 0, "inf" },
            { core::infinity<f64>(), 1, "inf" },
            { core::infinity<f64>(), 2, "inf" },
            { core::infinity<f64>(), 3, "inf" },
            { core::infinity<f64>(), 4, "inf" },
            { core::infinity<f64>(), 5, "inf" },
            { core::infinity<f64>(), 6, "inf" },
            { core::infinity<f64>(), 7, "inf" },
            { core::infinity<f64>(), 8, "inf" },
            { core::infinity<f64>(), 9, "inf" },

            { -core::infinity<f64>(), 0, "-inf" },
            { -core::infinity<f64>(), 1, "-inf" },
            { -core::infinity<f64>(), 2, "-inf" },
            { -core::infinity<f64>(), 3, "-inf" },
            { -core::infinity<f64>(), 4, "-inf" },
            { -core::infinity<f64>(), 5, "-inf" },
            { -core::infinity<f64>(), 6, "-inf" },
            { -core::infinity<f64>(), 7, "-inf" },
            { -core::infinity<f64>(), 8, "-inf" },
            { -core::infinity<f64>(), 9, "-inf" },

            { core::quietNaNF64(), 0, "NaN" },
            { core::quietNaNF64(), 1, "NaN" },
            { core::quietNaNF64(), 2, "NaN" },
            { core::quietNaNF64(), 3, "NaN" },
            { core::quietNaNF64(), 4, "NaN" },
            { core::quietNaNF64(), 5, "NaN" },
            { core::quietNaNF64(), 6, "NaN" },
            { core::quietNaNF64(), 7, "NaN" },
            { core::quietNaNF64(), 8, "NaN" },
            { core::quietNaNF64(), 9, "NaN" },

            { core::signalingNaNF64(), 0, "NaN" },
            { core::signalingNaNF64(), 1, "NaN" },
            { core::signalingNaNF64(), 2, "NaN" },
            { core::signalingNaNF64(), 3, "NaN" },
            { core::signalingNaNF64(), 4, "NaN" },
            { core::signalingNaNF64(), 5, "NaN" },
            { core::signalingNaNF64(), 6, "NaN" },
            { core::signalingNaNF64(), 7, "NaN" },
            { core::signalingNaNF64(), 8, "NaN" },
            { core::signalingNaNF64(), 9, "NaN" },
        };

        i32 ret = core::testing::executeTestTable("test case failed for fixed f64 at index: ", cases, [](auto& c, const char* cErr) {
            constexpr u32 BUFF_LEN = 64;
            char gotBuf[BUFF_LEN];
            u32 gotN = Unpack(core::floatToFixedCstr(c.input, c.precision, gotBuf, BUFF_LEN));
            gotBuf[gotN] = '\0';

            i32 cmpResult = core::memcmp(gotBuf, gotN, c.expected, core::cstrLen(c.expected));
            CT_CHECK(cmpResult == 0, cErr);

            CT_CHECK(checkWithOriginalPrecision(c.input, c.precision, gotBuf, gotN, cErr) == 0);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

constexpr i32 shortBufferWritesTest() {
    using core::ConversionError;

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
            CT_CHECK(got.err() == core::ConversionError::OutputBufferTooSmall, cErr);
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
            CT_CHECK(got.err() == core::ConversionError::OutputBufferTooSmall);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            f64 input;
            u32 precision;
            u32 bufferSize;
        };

        TestCase cases[] = {
            { -0.0, 0, 0 },
            { -0.0, 0, 1 },
            { -1.0, 0, 0 },
            { 0.5, 0, 0 },
            { 2.0, 1, 1 },

            { core::infinity<double>(), 5, 0 }, // no space to write anything
            { core::infinity<double>(), 5, 2 }, // enough for 'i' but not 'inf'
            { core::quietNaNF64(), 0, 0 },
            { core::quietNaNF64(), 3, 2 }, // enough for 'N' but not the full "NaN"

            { core::signalingNaNF64(), 0, 0 },

            { 1.23456, 3, 3 },
            { 1e-300, 50, 10 },
            { 1e200, 20, 15 },
            { 1e-200, 9, 4 },
            { 1e-200, 9, 5 },
            { 1e-200, 40, 5 },
            { 1e-200, 20, 5 },
            { 4.94e-324, 10, 5 },
            { 0.0, 5, 2 },
            { -0.0, 3, 2 },
            { 2.5, 0, 1 },
            { 2.345, 2, 3 },
            { 0.9999, 3, 3 },
            { -9.9, 0, 2 },
            { 9.999999, 6, 7 },
            { 2.49, 1, 3 },
            { 9.0, 1, 2 },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            char buff[64];
            auto got = core::floatToFixedCstr(c.input, c.precision, buff, c.bufferSize);
            CT_CHECK(got.hasErr(), cErr);
            CT_CHECK(got.err() == core::ConversionError::OutputBufferTooSmall);
            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 runCstrConv_FloatToCstr_TestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo(sInfo);

    tInfo.name = FN_NAME_TO_CPTR(toExponentNotationTest);
    if (runTest(tInfo, toExponentNotationTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(toFixedNotationTest);
    if (runTest(tInfo, toFixedNotationTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(toFixedNotationAllPowersOf10Test);
    if (runTest(tInfo, toFixedNotationAllPowersOf10Test) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(toFixedNotationAllBinaryExponentsTest);
    if (runTest(tInfo, toFixedNotationAllBinaryExponentsTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(toSpecialValuesTest);
    if (runTest(tInfo, toSpecialValuesTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(shortBufferWritesTest);
    if (runTest(tInfo, shortBufferWritesTest) != 0) { return -1; }

    return 0;
}

constexpr i32 runCompiletimeCstrConv_FloatToCstr_TestsSuite() {
    RunTestCompileTime(toExponentNotationTest);
    RunTestCompileTime(toFixedNotationTest);
    RunTestCompileTime(toSpecialValuesTest);
    RunTestCompileTime(shortBufferWritesTest);

    return 0;
}
