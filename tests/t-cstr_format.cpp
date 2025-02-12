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
    auto res = core::format(c.out.data(), u32(c.out.len()), c.fmt, c.input);
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
        CT_CHECK(res.value() == core::cstrLen("No arguments test"));
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
        auto res = core::format(buffer, BUFFER_SIZE,
                                "f32 a = {}, i8 b = {}, u64 c = {}, char* d = {}",
                                a, b, c, d);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 88);

        constexpr auto wanted = "f32 a = 7.8456116E1, i8 b = -41, u64 c = 21512351232245, char* d = some text for testing"_sv;
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
        char buff[4];
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
        CT_CHECK(res.err() == core::FormatError::TOO_FEW_ARGUMENTS);
    }
    {
        char buff[core::cstrLen("{ 1") + 1] = {};
        auto res = core::format(buff, CORE_C_ARRLEN(buff), "{{ {}", 1);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("{ 1"));
        CT_CHECK("{ 1"_sv.eq(buff));
    }
    // {
    //     char buff[core::cstrLen("} 2") + 1] = {};
    //     auto res = core::format(buff, CORE_C_ARRLEN(buff), "} {}", 2);
    //     CT_CHECK(res.hasValue());
    //     CT_CHECK(res.value() == core::cstrLen("} 2"));
    //     CT_CHECK("} 2"_sv.eq(buff));
    // }
    // {
    //     char buff[core::cstrLen("3 {") + 1] = {};
    //     auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} {", 3);
    //     CT_CHECK(res.hasValue());
    //     CT_CHECK(res.value() == core::cstrLen("3 {"));
    //     CT_CHECK("3 {"_sv.eq(buff));
    // }
    // {
    //     char buff[core::cstrLen("4 }") + 1] = {};
    //     auto res = core::format(buff, CORE_C_ARRLEN(buff), "{} }", 4);
    //     CT_CHECK(res.hasValue());
    //     CT_CHECK(res.value() == core::cstrLen("4 }"));
    //     CT_CHECK("4 }"_sv.eq(buff));
    // }

    // {
    //     char buff[core::cstrLen("{}1") + 1] = {};
    //     auto res = core::format(buff, CORE_C_ARRLEN(buff), "{{}}{}", 1);
    //     CT_CHECK(res.hasValue());
    //     CT_CHECK(res.value() == core::cstrLen("{}1"));
    //     CT_CHECK("{}1"_sv.eq(buff));
    // }

    // {
    //     char buff[core::cstrLen("1{}") + 1] = {};
    //     auto res = core::format(buff, CORE_C_ARRLEN(buff), "{}{{}}", 1);
    //     CT_CHECK(res.hasValue());
    //     CT_CHECK(res.value() == core::cstrLen("1{}"));
    //     CT_CHECK("1{}"_sv.eq(buff));
    // }

    return 0;
}

} // namespace

i32 runFormatTestsSuite() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(basicFormatTest);
    if (runTest(tInfo, basicFormatTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(edgeCasesTest);
    if (runTest(tInfo, edgeCasesTest) != 0) return -1;
    tInfo.name = FN_NAME_TO_CPTR(escapedBracketTest);
    if (runTest(tInfo, escapedBracketTest) != 0) return -1;

    return 0;
}

constexpr i32 runCompiletimeFormatTestSuite() {
    // FIXME: uncomment later
    // RunTestCompileTime(basicFormatTest);
    // RunTestCompileTime(edgeCasesTest);
    // RunTestCompileTime(escapedBracketTest);

    return 0;
}
