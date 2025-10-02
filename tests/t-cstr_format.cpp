#include "core_cstr.h"
#include "core_cstr_format.h"
#include "t-index.h"

namespace {

template <typename T>
struct CommonTestCase {
    T input;
    core::Memory<char> out;
    const char* fmt;
    core::Memory<const char> wanted;

    bool expectErr = false;
    core::FormatError wantedErr = core::FormatError::SENTINEL;
};

template <typename T>
constexpr i32 checkCommonTestCase(CommonTestCase<T>& c) {
    auto res = core::format(c.out.data(), i32(c.out.len()), c.fmt, c.input);
    CT_CHECK(res.hasErr() == c.expectErr);

    if (c.expectErr) {
        CT_CHECK(res.err() == c.wantedErr);
    }
    else {
        CT_CHECK(core::memcmp(c.out.data(), c.wanted.data(), c.wanted.len()) == 0);
    }

    return 0;
}

constexpr i32 basicFormatTest() {
    constexpr addr_size BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    auto bufferMem = core::Memory<char> { buffer, BUFFER_SIZE };

    core::memset(buffer, char(77), BUFFER_SIZE);
    {
        auto res = core::format(buffer, BUFFER_SIZE, "No arguments test");
        CT_CHECK(res.hasValue());
        CT_CHECK(addr_size(res.value()) == core::cstrLen("No arguments test"));
        buffer[res.value()] = '\0';
        CT_CHECK(core::memcmp(buffer, core::cstrLen(buffer),
                              "No arguments test", core::cstrLen("No arguments test")) == 0);
    }

    core::memset(buffer, char(77), BUFFER_SIZE);
    {
        f32 a = 78.456113f;
        i8 b = -41;
        u64 c = 21512351232245;
        const char* d = "some text for testing";
        char p = 'P';
        auto res = core::format(buffer, BUFFER_SIZE,
                                "f32 a = {}, i8 b = {}, u64 c = {}, char* d = {}, char p = {}",
                                a, b, c, d, p);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 100);

        constexpr auto wanted = "f32 a = 7.8456116E1, i8 b = -41, u64 c = 21512351232245, char* d = some text for testing, char p = P"_sv;
        CT_CHECK(core::memcmp(buffer, core::cstrLen(buffer), wanted.data(), wanted.len()) == 0);
    }

    auto verifyAllTestCases = [&](auto& testCases) {
        core::memset(buffer, char(77), BUFFER_SIZE);
        for (auto& tc : testCases) {
            CT_CHECK(checkCommonTestCase(tc) == 0);
        }
        return 0;
    };

    {
        CommonTestCase<i8> testCases[] = {
            { i8(8), bufferMem, "i8 = {}", "i8 = 8"_sv },
            { i8(core::limitMin<i8>()), bufferMem, "i8 = {}", "i8 = -128"_sv },
            { i8(core::limitMax<i8>()), bufferMem, "i8 = {}", "i8 = 127"_sv },
        };
        CT_CHECK(verifyAllTestCases(testCases) == 0);
    }
    {
        CommonTestCase<i16> testCases[] = {
            { i16(16), bufferMem, "i16 = {}", "i16 = 16"_sv },
            { i16(core::limitMin<i16>()), bufferMem, "i16 = {}", "i16 = -32768"_sv },
            { i16(core::limitMax<i16>()), bufferMem, "i16 = {}", "i16 = 32767"_sv },
        };
        CT_CHECK(verifyAllTestCases(testCases) == 0);
    }
    {
        CommonTestCase<i32> testCases[] = {
            { i32(32), bufferMem, "i32 = {}", "i32 = 32"_sv },
            { i32(core::limitMin<i32>()), bufferMem, "i32 = {}", "i32 = -2147483648"_sv },
            { i32(core::limitMax<i32>()), bufferMem, "i32 = {}", "i32 = 2147483647"_sv },
        };
        CT_CHECK(verifyAllTestCases(testCases) == 0);
    }
    {
        CommonTestCase<i64> testCases[] = {
            { i64(64), bufferMem, "i64 = {}", "i64 = 64"_sv },
            { i64(core::limitMin<i64>()), bufferMem, "i64 = {}", "i64 = -9223372036854775808"_sv },
            { i64(core::limitMax<i64>()), bufferMem, "i64 = {}", "i64 = 9223372036854775807"_sv },
        };
        CT_CHECK(verifyAllTestCases(testCases) == 0);
    }

    {
        CommonTestCase<u8> testCases[] = {
            { u8(8), bufferMem, "u8 = {}", "u8 = 8"_sv },
            { u8(core::limitMin<u8>()), bufferMem, "u8 = {}", "u8 = 0"_sv },
            { u8(core::limitMax<u8>()), bufferMem, "u8 = {}", "u8 = 255"_sv },
        };
        for (auto& tc : testCases) {
            CT_CHECK(checkCommonTestCase(tc) == 0);
        }
    }
    {
        CommonTestCase<u16> testCases[] = {
            { u16(16), bufferMem, "u16 = {}", "u16 = 16"_sv },
            { u16(core::limitMin<u16>()), bufferMem, "u16 = {}", "u16 = 0"_sv },
            { u16(core::limitMax<u16>()), bufferMem, "u16 = {}", "u16 = 65535"_sv },
        };
        for (auto& tc : testCases) {
            CT_CHECK(checkCommonTestCase(tc) == 0);
        }
    }
    {
        CommonTestCase<u32> testCases[] = {
            { u32(32), bufferMem, "u32 = {}", "u32 = 32"_sv },
            { u32(core::limitMin<u32>()), bufferMem, "u32 = {}", "u32 = 0"_sv },
            { u32(core::limitMax<u32>()), bufferMem, "u32 = {}", "u32 = 4294967295"_sv },
        };
        for (auto& tc : testCases) {
            CT_CHECK(checkCommonTestCase(tc) == 0);
        }
    }
    {
        CommonTestCase<u64> testCases[] = {
            { u64(64), bufferMem, "u64 = {}", "u64 = 64"_sv },
            { u64(core::limitMin<u64>()), bufferMem, "u64 = {}", "u64 = 0"_sv },
            { u64(core::limitMax<u64>()), bufferMem, "u64 = {}", "u64 = 18446744073709551615"_sv },
        };
        for (auto& tc : testCases) {
            CT_CHECK(checkCommonTestCase(tc) == 0);
        }
    }

    return 0;
}

constexpr i32 booleanFormattingTest() {
    // Hex
    {
        constexpr const char* expected = "1 0 1 0";
        char buff[core::cstrLen(expected) + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:H} {:H} {:h} {:h}", true, false, true, false);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen(expected));
        CT_CHECK(core::sv(expected).eq(buff));
    }

    // Boolean
    {
        constexpr const char* expected = "1 0";
        char buff[core::cstrLen(expected) + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:b} {:b}", true, false);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen(expected));
        CT_CHECK(core::sv(expected).eq(buff));
    }

    // Empty
    {
        constexpr const char* expected = "true false";
        char buff[core::cstrLen(expected) + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} {}", true, false);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen(expected));
        CT_CHECK(core::sv(expected).eq(buff));
    }

    // Padding
    {
        {
            constexpr const char* expected = "?1 ???0 ?1 0 - 1 0 - ?1 0";
            char buff[core::cstrLen(expected) + 1] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff),
                "{?2:H} {?4:H} {?2:h} {?1:h} - {:b} {:b} - {?2:b} {?1:b}",
                true, false, true, false, true, false, true, false);
            CT_CHECK(res.hasValue());
            CT_CHECK(res.value() == core::cstrLen(expected));
            CT_CHECK(core::sv(expected).eq(buff));
        }
        {
            constexpr const char* expected = "1 0 1 0 - ?1 ???0 ??1 ??0 - 1 0 - ?1 0 - true false - ?true ??false";
            char buff[core::cstrLen(expected) + 1] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff),
                "{:H} {:H} {:h} {:h} - {?2:H} {?4:H} {?3:h} {?3:h} - {:b} {:b} - {?2:b} {?1:b} - {} {} - {?5:} {?7:}",
                true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
            CT_CHECK(res.hasValue());
            CT_CHECK(res.value() == core::cstrLen(expected));
            CT_CHECK(core::sv(expected).eq(buff));
        }
    }

    // Invalid
    {
        {
            char buff[32];
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:f.2}", true);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER,
                    "Fixed floating point placeholder is not allowed for boolean values");
        }
        {
            char buff[32];
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:lmijzxc}", true);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
        }
    }

    return 0;
}

constexpr i32 strFormattingTest() {
    struct TestCase {
        const char* fmt;
        core::StrView arg1;
        i32 bufferSize;
        const char* expected;
        core::FormatError err = core::FormatError::SENTINEL;
    };

    TestCase cases[] = {
        { "{}", "A"_sv, 0, "A", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{}", "A"_sv, 1, "A", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{}", "A"_sv, 2, "A"},
        { "{}", "abcdef"_sv, i32(core::cstrLen("abcdef")), "abcdef", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{}", "abcdef"_sv, i32(core::cstrLen("abcdef")) + 1, "abcdef" },

        { "{:H}", core::sv("\0", 1), 2, "00", core::FormatError::OUT_BUFFER_OVERFLOW},
        { "{:H}", core::sv("\0", 1), 3, "00"},
        { "{:H}", "z"_sv, 3, "7A"},
        { "{:h}", "z"_sv, 3, "7a"},
        { "{?0:h}", "z"_sv, 3, "7a"}, // This is technically invalid but the padding is equal to 0, I am letting it slide..
        { "{t3:h}", "z"_sv, 100, "", core::FormatError::INVALID_PLACEHOLDER }, // padding is not allowed
        { "{y2:H}", "z"_sv, 100, "", core::FormatError::INVALID_PLACEHOLDER }, // padding is not allowed

        { "{:H}", "Dui"_sv, i32(core::cstrLen("44 75 69")), "44 75 69", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{:H}", "Dui"_sv, i32(core::cstrLen("44 75 69")) + 1, "44 75 69" },
        { "{:H}", "GzkM"_sv, i32(core::cstrLen("47 7A 6B 4D")), "47 7A 6B 4D", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{:H}", "GzkM"_sv, i32(core::cstrLen("47 7A 6B 4D")) + 1, "47 7A 6B 4D"},
        { "{:h}", "GzkM"_sv, i32(core::cstrLen("47 7a 6b 4d")), "47 7a 6b 4d", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{:h}", "GzkM"_sv, i32(core::cstrLen("47 7a 6b 4d")) + 1, "47 7a 6b 4d"},

        { "{:b}", core::sv("\0", 1), 8, "00000000", core::FormatError::OUT_BUFFER_OVERFLOW},
        { "{:b}", core::sv("\0", 1), 9, "00000000"},
        { "{:b}", "z"_sv, 9, "01111010"},
        { "{?0:b}", "z"_sv, 9, "01111010"}, // This is technically invalid but the padding is equal to 0, I am letting it slide..
        { "{u3:b}", "z"_sv, 100, "", core::FormatError::INVALID_PLACEHOLDER }, // padding is not allowed
        { "{I2:b}", "z"_sv, 100, "", core::FormatError::INVALID_PLACEHOLDER }, // padding is not allowed

        { "{:b}", "ABc"_sv, 3 * core::BYTE_SIZE + 2, "01000001 01000010 01100011", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{:b}", "ABc"_sv, 3 * core::BYTE_SIZE + 3, "01000001 01000010 01100011"},
        { "{:b}", "ZZZZ"_sv, 4 * core::BYTE_SIZE + 3, "01011010 01011010 01011010 01011010", core::FormatError::OUT_BUFFER_OVERFLOW },
        { "{:b}", "ZZZZ"_sv, 4 * core::BYTE_SIZE + 4, "01011010 01011010 01011010 01011010"},

        { "{:f.2}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
        { "{h}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
        { "{H}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
        { "{H:}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
        { "{h:}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
        { "{b:}", ""_sv, 4, "", core::FormatError::INVALID_PLACEHOLDER },
    };

    constexpr i32 BUFFER_LEN = core::CORE_KILOBYTE * 2;
    char buff[BUFFER_LEN];

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [&buff](auto& c, const char* cErr) {
        core::memset(buff, char(9), BUFFER_LEN);
        auto fmtRes = core::format(buff, c.bufferSize, c.fmt, c.arg1);

        if (c.err == core::FormatError::SENTINEL) {
            CT_CHECK(fmtRes.hasValue(), cErr);
            CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(c.expected), cErr);
            buff[fmtRes.value()] = '\0';
            CT_CHECK(core::sv(c.expected).eq(buff), cErr);
        }
        else {
            CT_CHECK(fmtRes.hasErr(), cErr);
            CT_CHECK(fmtRes.err() == c.err, cErr);
        }

        return 0;
    });

    return ret;
}

i32 pointerFormattingTest() {
    {
        constexpr const char* expected = "null";
        char buff[core::cstrLen(expected) + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", nullptr);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen(expected));
        CT_CHECK(core::sv(expected).eq(buff));
    }

    {
        struct Test { i32 a; f64 b; };
        char buff[10] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", Test {1, 2});
        CT_CHECK(res.hasErr(), "Should fail to print random structs passed as rvalues");
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }

    {
        struct Test { i32 a; f64 b; };
        Test val = {1, 2};
        char buff[10] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", val);
        CT_CHECK(res.hasErr(), "Should fail to print random structs passed by lvalues");
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }

    {
        struct Test { i32 a; f64 b; };
        Test val = {1, 2};

        // Can't check the value, but addresses should be successfully formatted
        {
            char buff[32] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", &val);
            CT_CHECK(res.hasValue());
        }
        {
            char buff[32] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:h}", &val);
            CT_CHECK(res.hasValue());
        }
        {
            char buff[32] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:H}", &val);
            CT_CHECK(res.hasValue());
        }
        {
            char buff[8 * core::BYTE_SIZE + 1] = {};
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:b}", &val);
            CT_CHECK(res.hasValue());
        }
    }

    // Invalid
    {
        struct Test { i32 a; f64 b; };
        Test val = {1, 2};

        {
            char buff[32];
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:f.2}", &val);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
        }
        {
            char buff[32];
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{:lmijzxc}", &val);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
        }
        {
            char buff[32];
            auto res = core::format(buff, CORE_C_ARRLEN(buff), "{b2:}", &val);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
        }
    }

    return 0;
}

constexpr i32 intFormattingTest() {
    struct TestCase {
        const char* fmt;
        struct {
            i8 arg1;
            i16 arg2;
            i32 arg3;
            i64 arg4;
            u8 arg5;
            u16 arg6;
            u32 arg7;
            u64 arg8;
        } args;
        i32 bufferSize;
        const char* expected;
        core::FormatError err = core::FormatError::SENTINEL;
    };

    // constexpr i32 BUFF_LEN = core::CORE_KILOBYTE * 2;
    char buff[core::CORE_KILOBYTE * 2];

    auto runTestCases = [&](auto& cases) -> i32 {
        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [&](auto& c, const char* cErr) {
            core::memset(buff, char(9), core::CORE_KILOBYTE * 2);

            auto fmtRes = core::format(buff, c.bufferSize, c.fmt,
                                    c.args.arg1, c.args.arg2, c.args.arg3, c.args.arg4,
                                    c.args.arg5, c.args.arg6, c.args.arg7, c.args.arg8);

            if (c.err == core::FormatError::SENTINEL) {
                CT_CHECK(fmtRes.hasValue(), cErr);
                CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(c.expected), cErr);
                buff[fmtRes.value()] = '\0';
                CT_CHECK(core::sv(c.expected).eq(buff), cErr);
            }
            else {
                CT_CHECK(fmtRes.hasErr(), cErr);
                CT_CHECK(fmtRes.err() == c.err, cErr);
            }

            return 0;
        });

        return ret;
    };

    // Empty Place Holder Tests
    {
        TestCase cases[] = {
            {
                "{} {} {} {} {} {} {} {}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0")),
                "0 0 0 0 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {} {} {} {} {} {} {}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0"
            },
            {
                "{} {} {} {} {} {} {} {}",
                { i8(-5), i16(-213), i32(-51235), i64(-1266123), u8(5), u16(213), u32(51235), u64(1266123) },
                i32(core::cstrLen("-5 -213 -51235 -1266123 5 213 51235 1266123")),
                "-5 -213 -51235 -1266123 5 213 51235 1266123",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {} {} {} {} {} {} {}",
                { i8(-5), i16(-213), i32(-51235), i64(-1266123), u8(5), u16(213), u32(51235), u64(1266123) },
                i32(core::cstrLen("-5 -213 -51235 -1266123 5 213 51235 1266123") + 1),
                "-5 -213 -51235 -1266123 5 213 51235 1266123"
            },
            {
                "{} {} {} {} {} {} {} {}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("127 32767 2147483647 9223372036854775807 255 65535 4294967295 18446744073709551615")),
                "127 32767 2147483647 9223372036854775807 255 65535 4294967295 18446744073709551615",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {} {} {} {} {} {} {}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("127 32767 2147483647 9223372036854775807 255 65535 4294967295 18446744073709551615") + 1),
                "127 32767 2147483647 9223372036854775807 255 65535 4294967295 18446744073709551615"
            },
            {
                "{} {} {} {} {} {} {} {}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("-128 -32768 -2147483648 -9223372036854775808 0 0 0 0")),
                "-128 -32768 -2147483648 -9223372036854775808 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {} {} {} {} {} {} {}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("-128 -32768 -2147483648 -9223372036854775808 0 0 0 0") + 1),
                "-128 -32768 -2147483648 -9223372036854775808 0 0 0 0"
            },
        };

        CT_CHECK(runTestCases(cases) == 0);
    }

    // Padding Only Place Holder Tests
    {
        TestCase cases[] = {
            {
                "{00:} {t0:} {c0:} {v0:} {s0:} {a0:} {10:} {90:}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0")),
                "0 0 0 0 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{00:} {t0:} {c0:} {v0:} {s0:} {a0:} {10:} {90:}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0"
            },
            {
                "{t4:} {t4:} {t4:} {t4:} {t4:} {t4:} {t4:} {t4:}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0")),
                "ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{t4:} {t4:} {t4:} {t4:} {t4:} {t4:} {t4:} {t4:}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0") + 1),
                "ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0 ttt0"
            },
            {
                "{j4:} {j6:} {j11:} {j20:} {j4:} {j6:} {j11:} {j21:}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("j127 j32767 j2147483647 j9223372036854775807 j255 j65535 j4294967295 j18446744073709551615")),
                "j127 j32767 j2147483647 j9223372036854775807 j255 j65535 j4294967295 j18446744073709551615",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{j4:} {j6:} {j11:} {j20:} {j4:} {j6:} {j11:} {j21:}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("j127 j32767 j2147483647 j9223372036854775807 j255 j65535 j4294967295 j18446744073709551615") + 1),
                "j127 j32767 j2147483647 j9223372036854775807 j255 j65535 j4294967295 j18446744073709551615",
            },
        };

        CT_CHECK(runTestCases(cases) == 0);
    }

    // Hex
    {
        TestCase cases[] = {
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0")),
                "0 0 0 0 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0"
            },
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { i8(0x5E), i16(0x1FE), i32(0xFA41FE), i64(0xF324BFA), u8(0x5E), u16(0x1FE), u32(0xFA41FE), u64(0xF324BFA) },
                i32(core::cstrLen("5e 1fe fa41fe f324bfa 5e 1fe fa41fe f324bfa")),
                "5e 1fe fa41fe f324bfa 5e 1fe fa41fe f324bfa",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { i8(0x5E), i16(0x1FE), i32(0xFA41FE), i64(0xF324BFA), u8(0x5E), u16(0x1FE), u32(0xFA41FE), u64(0xF324BFA) },
                i32(core::cstrLen("5e 1fe fa41fe f324bfa 5e 1fe fa41fe f324bfa") + 1),
                "5e 1fe fa41fe f324bfa 5e 1fe fa41fe f324bfa"
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { i8(0x5E), i16(0x1FE), i32(0xFA41FE), i64(0xF324BFA), u8(0x5E), u16(0x1FE), u32(0xFA41FE), u64(0xF324BFA) },
                i32(core::cstrLen("5E 1FE FA41FE F324BFA 5E 1FE FA41FE F324BFA")),
                "5E 1FE FA41FE F324BFA 5E 1FE FA41FE F324BFA",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { i8(0x5E), i16(0x1FE), i32(0xFA41FE), i64(0xF324BFA), u8(0x5E), u16(0x1FE), u32(0xFA41FE), u64(0xF324BFA) },
                i32(core::cstrLen("5E 1FE FA41FE F324BFA 5E 1FE FA41FE F324BFA") + 1),
                "5E 1FE FA41FE F324BFA 5E 1FE FA41FE F324BFA"
            },
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("7f 7fff 7fffffff 7fffffffffffffff ff ffff ffffffff ffffffffffffffff")),
                "7f 7fff 7fffffff 7fffffffffffffff ff ffff ffffffff ffffffffffffffff",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:h} {:h} {:h} {:h} {:h} {:h} {:h} {:h}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("7f 7fff 7fffffff 7fffffffffffffff ff ffff ffffffff ffffffffffffffff") + 1),
                "7f 7fff 7fffffff 7fffffffffffffff ff ffff ffffffff ffffffffffffffff"
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("7F 7FFF 7FFFFFFF 7FFFFFFFFFFFFFFF FF FFFF FFFFFFFF FFFFFFFFFFFFFFFF")),
                "7F 7FFF 7FFFFFFF 7FFFFFFFFFFFFFFF FF FFFF FFFFFFFF FFFFFFFFFFFFFFFF",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("7F 7FFF 7FFFFFFF 7FFFFFFFFFFFFFFF FF FFFF FFFFFFFF FFFFFFFFFFFFFFFF") + 1),
                "7F 7FFF 7FFFFFFF 7FFFFFFFFFFFFFFF FF FFFF FFFFFFFF FFFFFFFFFFFFFFFF"
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("80 8000 80000000 8000000000000000 0 0 0 0")),
                "80 8000 80000000 8000000000000000 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:H} {:H} {:H} {:H} {:H} {:H} {:H} {:H}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("80 8000 80000000 8000000000000000 0 0 0 0") + 1),
                "80 8000 80000000 8000000000000000 0 0 0 0"
            },
        };

        CT_CHECK(runTestCases(cases) == 0);
    }

    // Binary
    {
        TestCase cases[] = {
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0")),
                "0 0 0 0 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0"
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { i8(0b101), i16(0b10101), i32(0b1010101010), i64(0b10101010101010101010), u8(0b101), u16(0b10101), u32(0b1010101010), u64(0b10101010101010101010) },
                i32(core::cstrLen("101 10101 1010101010 10101010101010101010 101 10101 1010101010 10101010101010101010")),
                "101 10101 1010101010 10101010101010101010 101 10101 1010101010 10101010101010101010",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { i8(0b101), i16(0b10101), i32(0b1010101010), i64(0b10101010101010101010), u8(0b101), u16(0b10101), u32(0b1010101010), u64(0b10101010101010101010) },
                i32(core::cstrLen("101 10101 1010101010 10101010101010101010 101 10101 1010101010 10101010101010101010") + 1),
                "101 10101 1010101010 10101010101010101010 101 10101 1010101010 10101010101010101010"
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("1111111 111111111111111 1111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111111 11111111 1111111111111111 11111111111111111111111111111111 1111111111111111111111111111111111111111111111111111111111111111")),
                "1111111 111111111111111 1111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111111 11111111 1111111111111111 11111111111111111111111111111111 1111111111111111111111111111111111111111111111111111111111111111",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { core::limitMax<i8>(), core::limitMax<i16>(), core::limitMax<i32>(), core::limitMax<i64>(), core::limitMax<u8>(), core::limitMax<u16>(), core::limitMax<u32>(), core::limitMax<u64>() },
                i32(core::cstrLen("1111111 111111111111111 1111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111111 11111111 1111111111111111 11111111111111111111111111111111 1111111111111111111111111111111111111111111111111111111111111111") + 1),
                "1111111 111111111111111 1111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111111 11111111 1111111111111111 11111111111111111111111111111111 1111111111111111111111111111111111111111111111111111111111111111"
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("10000000 1000000000000000 10000000000000000000000000000000 1000000000000000000000000000000000000000000000000000000000000000 0 0 0 0")),
                "10000000 1000000000000000 10000000000000000000000000000000 1000000000000000000000000000000000000000000000000000000000000000 0 0 0 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}",
                { core::limitMin<i8>(), core::limitMin<i16>(), core::limitMin<i32>(), core::limitMin<i64>(), core::limitMin<u8>(), core::limitMin<u16>(), core::limitMin<u32>(), core::limitMin<u64>() },
                i32(core::cstrLen("10000000 1000000000000000 10000000000000000000000000000000 1000000000000000000000000000000000000000000000000000000000000000 0 0 0 0") + 1),
                "10000000 1000000000000000 10000000000000000000000000000000 1000000000000000000000000000000000000000000000000000000000000000 0 0 0 0"
            },
            {
                "{t1:b} {t2:b} {t3:b} {t4:b} {t5:b} {t4:b} {t3:b} {t2:b}",
                { i8(0b1), i16(0b1), i32(0b1), i64(0b1), u8(0b1), u16(0b1), u32(0b11), u64(0b11) },
                i32(core::cstrLen("1 t1 tt1 ttt1 tttt1 tt11 tt1 11")),
                "1 t1 tt1 ttt1 tttt1 tt11 tt1 11",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{t1:b} {t2:b} {t3:b} {t4:b} {t5:b} {t4:b} {t3:b} {t2:b}",
                { i8(0b1), i16(0b1), i32(0b1), i64(0b1), u8(0b1), u16(0b11), u32(0b1), u64(0b11) },
                i32(core::cstrLen("1 t1 tt1 ttt1 tttt1 tt11 tt1 11") + 1),
                "1 t1 tt1 ttt1 tttt1 tt11 tt1 11"
            },
            {
                "{08:b} {08:b} {08:b} {08:b} {08:b} {08:b} {08:b} {08:b}",
                { i8(0b00000000), i16(0b00000001), i32(0b00000011), i64(0b00001111), u8(0b00011111), u16(0b00111111), u32(0b01111111), u64(0b11111111) },
                i32(core::cstrLen("00000000 00000001 00000011 00001111 00011111 00111111 01111111 11111111") ),
                "00000000 00000001 00000011 00001111 00011111 00111111 01111111 11111111",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{08:b} {08:b} {08:b} {08:b} {08:b} {08:b} {08:b} {08:b}",
                { i8(0b00000000), i16(0b00000001), i32(0b00000011), i64(0b00001111), u8(0b00011111), u16(0b00111111), u32(0b01111111), u64(0b11111111) },
                i32(core::cstrLen("00000000 00000001 00000011 00001111 00011111 00111111 01111111 11111111") + 1),
                "00000000 00000001 00000011 00001111 00011111 00111111 01111111 11111111"
            },
        };

        CT_CHECK(runTestCases(cases) == 0);
    }

    // Invalid
    {
        TestCase cases[] = {
            {
                "{} {} {} {} {} {} {} {:f.2}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0",
                core::FormatError::INVALID_PLACEHOLDER
            },
            {
                "{} {} {} {} {} {h:} {} {}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0",
                core::FormatError::INVALID_PLACEHOLDER
            },
            {
                "{} {} {} {} {} {} {H:} {}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0",
                core::FormatError::INVALID_PLACEHOLDER
            },
            {
                "{} {} {} {asd123} {} {} {} {}",
                { i8(0), i16(0), i32(0), i64(0), u8(0), u16(0), u32(0), u64(0) },
                i32(core::cstrLen("0 0 0 0 0 0 0 0") + 1),
                "0 0 0 0 0 0 0 0",
                core::FormatError::INVALID_PLACEHOLDER
            },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    return 0;
}

constexpr i32 floatFormattingTest() {
    constexpr i32 BUFFER_LEN = core::CORE_KILOBYTE * 2;
    char buff[BUFFER_LEN];

    auto runTestCases = [&buff](auto& cases) -> i32 {
        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [&buff](auto& c, const char* cErr) {
            core::memset(buff, char(9), BUFFER_LEN);

            auto fmtRes = core::format(buff, c.bufferSize, c.fmt, c.args.arg1, c.args.arg2);

            if (c.err == core::FormatError::SENTINEL) {
                CT_CHECK(fmtRes.hasValue(), cErr);
                CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(c.expected), cErr);
                buff[fmtRes.value()] = '\0';
                CT_CHECK(core::sv(c.expected).eq(buff), cErr);
            }
            else {
                CT_CHECK(fmtRes.hasErr(), cErr);
                CT_CHECK(fmtRes.err() == c.err, cErr);
            }

            return 0;
        });

        return ret;
    };

    struct TestCase {
        const char* fmt;
        struct {
            f32 arg1;
            f64 arg2;
        } args;
        i32 bufferSize;
        const char* expected;
        core::FormatError err = core::FormatError::SENTINEL;
    };

    // Empty Place Holder
    {
        TestCase cases[] = {
            {
                "{} {}",
                { 1.0e+8f, 1.234567 },
                i32(core::cstrLen("1E8 1.234567")),
                "1E8 1.234567",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {}",
                { 1.0e+8f, 1.234567 },
                i32(core::cstrLen("1E8 1.234567") + 1),
                "1E8 1.234567",
            },
            {
                "{} {}",
                { -2.47E-43f, 1.0e+15 + 1.0e+10 },
                i32(core::cstrLen("-2.47E-43 1.00001E15")),
                "-2.47E-43 1.00001E15",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {}",
                { -2.47E-43f, 1.0e+15 + 1.0e+10 },
                i32(core::cstrLen("-2.47E-43 1.00001E15") + 1),
                "-2.47E-43 1.00001E15"
            },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    // Fixed
    {
        TestCase cases[] = {
            {
                "{} {:f.4}",
                { 0.f, 1.234567 },
                i32(core::cstrLen("0 1.2346")),
                "0 1.2346",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {:f.4}",
                { 0.f, 1.234567 },
                i32(core::cstrLen("0 1.2346") + 1),
                "0 1.2346"
            },
            {
                "{} {:f.2}",
                { 0.f, 2.5555 },
                i32(core::cstrLen("0 2.56")),
                "0 2.56",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {:f.2}",
                { 0.f, 2.5555 },
                i32(core::cstrLen("0 2.56") + 1),
                "0 2.56"
            },
            {
                "{:f.2} {}",
                {15.123f, 0 },
                i32(core::cstrLen("15.12 0")),
                "15.12 0",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{:f.2} {}",
                {15.123f, 0 },
                i32(core::cstrLen("15.12 0") + 1),
                "15.12 0"
            },
            {
                "{} {:f.4}",
                { 0.f, 99999.9999999 },
                i32(core::cstrLen("0 100000.0000")),
                "0 100000.0000",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {:f.4}",
                { 0.f, 99999.9999999 },
                i32(core::cstrLen("0 100000.0000") + 1),
                "0 100000.0000"
            },
            {
                "{} {:f.8}",
                { 0.f, -0.000000201 },
                i32(core::cstrLen("0 -0.00000020")),
                "0 -0.00000020",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {:f.8}",
                { 0.f, -0.000000201 },
                i32(core::cstrLen("0 -0.00000020") + 1),
                "0 -0.00000020"
            },
            {
                "{} {:f.2}",
                { 0.f, -1.0E-8 },
                i32(core::cstrLen("0 -0.00")),
                "0 -0.00",
                core::FormatError::OUT_BUFFER_OVERFLOW
            },
            {
                "{} {:f.2}",
                { 0.f, -1.0E-8 },
                i32(core::cstrLen("0 -0.00") + 1),
                "0 -0.00"
            }
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    // Invalid
    {
        TestCase cases[] = {
            { "{:h} {}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{:H} {}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{:b} {}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{asd12} {}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{} {02:}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{} {02:f.2}", { 1.0e+8f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{:f.-2}", { 1.234567f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{:f.xyz}", { 1.234567f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },

            // Placeholders should not allow numbers that are more than 5 digits. Probably..
            { "{:f.10000}", { 1.234567f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
            { "{:f.99999999}", { 1.234567f, 1.234567 }, 1000, "", core::FormatError::INVALID_PLACEHOLDER },
        };
        CT_CHECK(runTestCases(cases) == 0);
    }

    // Regression 1 - allowing float parsing.
    {
        auto fmtRes = core::format(buff, BUFFER_LEN,  "{:f.2}", 1.234f);
        const char* expected = "1.23";
        CT_CHECK(fmtRes.hasValue());
        CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(expected));
        buff[fmtRes.value()] = '\0';
        CT_CHECK(core::sv(expected).eq(buff));
    }

    // Regression 2 - 0 at the end of the buffer
    {
        {
            auto fmtRes = core::format(buff, 4,  "21{}",  f64(0));
            const char* expected = "210";
            CT_CHECK(fmtRes.hasValue());
            CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(expected));
            buff[fmtRes.value()] = '\0';
            CT_CHECK(core::sv(expected).eq(buff));
        }
        {
            auto fmtRes = core::format(buff, 4,  "21{}",  f32(0));
            const char* expected = "210";
            CT_CHECK(fmtRes.hasValue());
            CT_CHECK(addr_size(fmtRes.value()) == core::cstrLen(expected));
            buff[fmtRes.value()] = '\0';
            CT_CHECK(core::sv(expected).eq(buff));
        }
    }

    // Regression 3 - edge case - largest possbile fixed point
    {
        char biggerBuff[20000] = {};

        constexpr const char* fmt = "{:f.9999}";
        constexpr addr_size expectedSize = core::cstrLen("0.") + 9999;

        {
            auto fmtRes = core::format(biggerBuff, expectedSize, fmt,  f32(0));
            CT_CHECK(fmtRes.hasErr());
            CT_CHECK(fmtRes.err() == core::FormatError::OUT_BUFFER_OVERFLOW, "Must leave space for the null terminator");
        }
        {
            auto fmtRes = core::format(biggerBuff, expectedSize + 1, fmt,  f32(0));
            CT_CHECK(fmtRes.hasValue());
            biggerBuff[fmtRes.value()] = '\0';
            CT_CHECK(core::cstrLen(biggerBuff) == expectedSize, "Must have written exactly as much");
        }
    }

    return 0;
}

constexpr i32 edgeCasesTest() {
    {
        auto res = core::format(nullptr, 15, "");
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        auto res = core::format({ nullptr, 15 }, "");
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        auto res = core::format( nullptr, 15, "", 1, 2);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        auto res = core::format({ nullptr, 15 }, "", 1, 2);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), nullptr);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), nullptr, 1, 2);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_ARGUMENTS);
    }
    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "123456", 1, 2);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
    }

    {
        char buff[4] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "");
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 0);
        CT_CHECK(buff[0] == '\0');
    }
    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", "");
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 0);
        CT_CHECK(buff[0] == '\0');
    }

    {
        char buff[8];
        constexpr addr_size buffSize = CORE_C_ARRLEN(buff);

        {
            auto res = core::format(buff, buffSize, "12345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }

        {
            auto res = core::format(buff, buffSize, "abc={}", i8(-123));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "abcd={}", u8(123));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "ab={}", i16(-1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "abc={}", u16(1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "ab={}", i32(-1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "abc={}", u32(1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "ab={}", i64(-1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "abc={}", u64(1234));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }

        {
            auto res = core::format(buff, buffSize, "abc={}", true);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "ab={}", false);
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }

        {
            auto res = core::format(buff, buffSize, "{}", "12345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "1{}", "2345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "12{}", "345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "123{}", "45678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "1234{}", "5678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "12345{}", "678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "123456{}", "78");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "1234567{}", "8");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "12345678{}", "");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}", "12345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}1", "2345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}12", "345678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}123", "45678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}1234", "5678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}12345", "678");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}123456", "78");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}1234567", "8");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "{}12345678", "");
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }

        {
            auto res = core::format(buff, buffSize, "ab={}", f32(6.8750f));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
        {
            auto res = core::format(buff, buffSize, "ab={}", f64(3.1250));
            CT_CHECK(res.hasErr());
            CT_CHECK(res.err() == core::FormatError::OUT_BUFFER_OVERFLOW);
        }
    }

    {
        char buff[5];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} {}", 12);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::TOO_FEW_ARGUMENTS);
    }
    {
        char buff[10];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} {} {}", 12, 123);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::TOO_FEW_ARGUMENTS);
    }
    {
        // This should technically say that there are too few arguments provided, but I'll live with the existing
        // implementation. It's fine to work basically as a memcopy with length checks.
        char buff[3] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}");
        CT_CHECK(res.hasValue());
        CT_CHECK(core::memcmp(buff, core::cstrLen(buff), "{}", core::cstrLen("{}")) == 0);
    }

    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "", 12);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::TOO_MANY_ARGUMENTS);
    }
    {
        char buff[4];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}", 12, 123);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::TOO_MANY_ARGUMENTS);
    }
    {
        char buff[8];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}{}", 12, 123, 56);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::TOO_MANY_ARGUMENTS);
    }

    // Padding number overflow maximum
    {
        char buff[64];
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{b891283098:}", 5);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
    }

    return 0;
}

constexpr i32 escapedBracketTest() {
    {
        char buff[2] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{");
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 1);
        CT_CHECK("{"_sv.eq(buff));
    }
    {
        char buff[2] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "}");
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 1);
        CT_CHECK("}"_sv.eq(buff));
    }

    {
        char buff[core::cstrLen("{ 1") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{ {}", 1);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
    }
    {
        char buff[core::cstrLen("{ 1") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{{ {}", 1);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("{ 1"));
        CT_CHECK("{ 1"_sv.eq(buff));
    }
    {
        char buff[core::cstrLen("} 2") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "} {}", 2);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("} 2"));
        CT_CHECK("} 2"_sv.eq(buff));
    }
    {
        char buff[core::cstrLen("3 {") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} {", 3);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
    }
    {
        char buff[core::cstrLen("4 }") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} }", 4);
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == core::FormatError::INVALID_PLACEHOLDER);
    }

    {
        char buff[core::cstrLen("{}1") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{{}}{}", 1);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("{}1"));
        CT_CHECK("{}1"_sv.eq(buff));
    }
    {
        char buff[core::cstrLen("1{}") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}{{}}", 1);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("1{}"));
        CT_CHECK("1{}"_sv.eq(buff));
    }

    return 0;
}

} // namespace

i32 runFormatTestsSuite() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo();
    tInfo.name = FN_NAME_TO_CPTR(basicFormatTest);
    if (runTest(tInfo, basicFormatTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(booleanFormattingTest);
    if (runTest(tInfo, booleanFormattingTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(intFormattingTest);
    if (runTest(tInfo, intFormattingTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(floatFormattingTest);
    if (runTest(tInfo, floatFormattingTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(strFormattingTest);
    if (runTest(tInfo, strFormattingTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(pointerFormattingTest);
    if (runTest(tInfo, pointerFormattingTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(edgeCasesTest);
    if (runTest(tInfo, edgeCasesTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(escapedBracketTest);
    if (runTest(tInfo, escapedBracketTest) != 0) return -1;

    return 0;
}

constexpr i32 runCompiletimeFormatTestSuite1() {
    RunTestCompileTime(basicFormatTest);
    RunTestCompileTime(booleanFormattingTest);
    RunTestCompileTime(intFormattingTest);
    RunTestCompileTime(floatFormattingTest);
    RunTestCompileTime(strFormattingTest);
    RunTestCompileTime(edgeCasesTest);
    RunTestCompileTime(escapedBracketTest);

    return 0;
}
