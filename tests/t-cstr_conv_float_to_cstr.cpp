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

            // pow 5 corner case
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
            { 0.0, "0E0" },
            { -0.0, "-0E0" },
            { 1.0, "1E0" },
            { -1.0, "-1E0" },
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

i32 runCstrConv_FloatToCstr_TestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(toExponentNotationTest);
    if (runTest(tInfo, toExponentNotationTest) != 0) { ret = -1; }

    // FIXME: continue testing

    return ret;
}

constexpr i32 runCompiletimeCstrConv_FloatToCstr_TestsSuite() {

    return 0;
}
