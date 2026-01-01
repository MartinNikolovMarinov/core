#include "core_str_view.h"
#include "t-index.h"
#include "testing/testing_framework.h"

namespace {

constexpr i32 trimWhiteSpaceLeftTest() {
    struct TestCase {
        core::StrView input;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), core::sv() },
        { core::sv(""), core::sv("") },
        { core::sv("abce"), core::sv("abce") },
        { core::sv("   abce"), core::sv("abce") },
        { core::sv("  abce   "), core::sv("abce   ") },
        { core::sv("   "), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("trimWhiteSpaceLeft failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto trimmed = core::trimWhiteSpaceLeft(tc.input);

        CT_CHECK(trimmed.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(trimmed.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(trimmed.len() == tc.expected.len(), cErr);
        CT_CHECK(trimmed.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimWhiteSpaceRightTest() {
    struct TestCase {
        core::StrView input;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), core::sv() },
        { core::sv(""), core::sv("") },
        { core::sv("abce"), core::sv("abce") },
        { core::sv("abce   "), core::sv("abce") },
        { core::sv("  abce   "), core::sv("  abce") },
        { core::sv("   "), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("trimWhiteSpaceRight failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto trimmed = core::trimWhiteSpaceRight(tc.input);

        CT_CHECK(trimmed.eq(tc.expected), cErr);
        CT_CHECK(trimmed.empty() == tc.expected.empty(), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(trimmed.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(trimmed.len() == tc.expected.len(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimTest() {
    struct TestCase {
        core::StrView input;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), core::sv() },
        { core::sv(""), core::sv("") },
        { core::sv("abce"), core::sv("abce") },
        { core::sv("abce   "), core::sv("abce") },
        { core::sv(" abce"), core::sv("abce") },
        { core::sv("  abce   "), core::sv("abce") },
        { core::sv("   "), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("trim failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto trimmed = core::trim(tc.input);

        CT_CHECK(trimmed.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(trimmed.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(trimmed.len() == tc.expected.len(), cErr);
        CT_CHECK(trimmed.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimWhiteSpaceTest() {
    struct TestCase {
        core::StrView input;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), core::sv() },
        { core::sv(""), core::sv() },
        { core::sv("abcd"), core::sv("abcd") },
        { core::sv("   abcd"), core::sv("abcd") },
        { core::sv("   abcd "), core::sv("abcd") },
        { core::sv("   "), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("trimWhiteSpace failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto trimmed = core::trim(tc.input);

        CT_CHECK(trimmed.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(trimmed.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(trimmed.len() == tc.expected.len(), cErr);
        CT_CHECK(trimmed.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimSpecificCharTest() {
    struct TestCase {
        core::StrView input;
        char trimChar;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), '/', core::sv() },
        { core::sv(""), '/', core::sv("") },
        { core::sv("////path"), '/', core::sv("path") },
        { core::sv("no-prefix"), '/', core::sv("no-prefix") },
        { core::sv("....."), '.', core::sv() },
    };

    i32 ret = core::testing::executeTestTable("trimSpecificCharTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto trimmed = core::trim(tc.input, tc.trimChar);

        CT_CHECK(trimmed.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(trimmed.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(trimmed.len() == tc.expected.len(), cErr);
        CT_CHECK(trimmed.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 cutTest() {
    struct TestCase {
        core::StrView input;
        char c;
        bool keepDelim;
        core::StrView expectedCut;
        core::StrView expectedReturn;
    };

    constexpr TestCase cases[] = {
        { "-bcde"_sv, '-', false, core::sv(), "bcde"_sv },
        { "a-cde"_sv, '-', false, "a"_sv, "cde"_sv },
        { "ab-de"_sv, '-', false, "ab"_sv, "de"_sv },
        { "abc-e"_sv, '-', false, "abc"_sv, "e"_sv },
        { "abcd-"_sv, '-', false, "abcd"_sv, core::sv() },
        { "abcde"_sv, '-', false, core::sv(), core::sv() },

        { "-bcde"_sv, '-', true, "-"_sv, "bcde"_sv },
        { "a-cde"_sv, '-', true, "a-"_sv, "cde"_sv },
        { "ab-de"_sv, '-', true, "ab-"_sv, "de"_sv },
        { "abc-e"_sv, '-', true, "abc-"_sv, "e"_sv },
        { "abcd-"_sv, '-', true, "abcd-"_sv, core::sv() },
        { "abcde"_sv, '-', true, core::sv(), core::sv() },

        // Simulating real use case
        { "abc\ndef\n"_sv, '\n', false, "abc"_sv, "def\n"_sv },
        { "def\n"_sv, '\n', false, "def"_sv, core::sv() },

        { "abc\ndef\n"_sv, '\n', true, "abc\n"_sv, "def\n"_sv },
        { "def\n"_sv, '\n', true, "def\n"_sv, core::sv() },

        // Edge cases
        { "?"_sv, '?', false, core::sv(), core::sv() },
        { ""_sv, '-', false, core::sv(), core::sv() },
        { "abc"_sv, '-', false, core::sv(), core::sv() },
        { "-"_sv, '+', false, core::sv(), core::sv() },
        { core::sv(), 'x', false, core::sv(), core::sv() },

        { "?"_sv, '?', true, "?"_sv, core::sv() },
        { ""_sv, '-', true, core::sv(), core::sv() },
        { "abc"_sv, '-', true, core::sv(), core::sv() },
        { "-"_sv, '+', true, core::sv(), core::sv() },
        { core::sv(), 'x', true, core::sv(), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("cutTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        core::StrView gotCut;
        core::StrView gotReturn = core::cut(tc.input, tc.c, gotCut, tc.keepDelim);

        CT_CHECK(gotCut.eq(tc.expectedCut), cErr);
        CT_CHECK(gotCut.empty() == tc.expectedCut.empty(), cErr);
        if (tc.expectedCut.data() == nullptr) {
            CT_CHECK(gotCut.data() == tc.expectedCut.data(), cErr);
        }

        CT_CHECK(gotReturn.eq(tc.expectedReturn), cErr);
        CT_CHECK(gotReturn.empty() == tc.expectedReturn.empty(), cErr);
        if (tc.expectedReturn.data() == nullptr) {
            CT_CHECK(gotReturn.data() == tc.expectedReturn.data(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 splitTest() {
    struct TestCase {
        core::StrView input;
        char delim;
        addr_size outLen;
        bool expectedOk;
        addr_size expectedCount;
        core::StrView expected[5];
    };

    constexpr TestCase cases[] = {
        { "a,b,c"_sv, ',', 3, true, 3, { "a"_sv, "b"_sv, "c"_sv, core::sv(), core::sv() } },
        { "a,,c"_sv, ',', 3, true, 3, { "a"_sv, ""_sv, "c"_sv, core::sv(), core::sv() } },
        { ",a,"_sv, ',', 3, true, 3, { ""_sv, "a"_sv, ""_sv, core::sv(), core::sv() } },
        { "abc"_sv, ',', 2, true, 1, { "abc"_sv, core::sv(), core::sv(), core::sv(), core::sv() } },
        { core::sv(), ',', 0, true, 0, { core::sv(), core::sv(), core::sv(), core::sv(), core::sv() } },
        { "a,b,c"_sv, ',', 2, false, 2, { "a"_sv, "b"_sv, core::sv(), core::sv(), core::sv() } },
    };

    i32 ret = core::testing::executeTestTable("splitTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        core::StrView out[5] = {};
        addr_size outCount = 0;
        bool ok = core::split(tc.input, tc.delim, out, tc.outLen, outCount);

        CT_CHECK(ok == tc.expectedOk, cErr);
        if (ok) {
            CT_CHECK(outCount == tc.expectedCount, cErr);
            for (addr_size i = 0; i < tc.expectedCount; i++) {
                CT_CHECK(out[i].eq(tc.expected[i]), cErr);
                CT_CHECK(out[i].empty() == tc.expected[i].empty(), cErr);
            }
        }
        else {
            CT_CHECK(outCount == tc.expectedCount, cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 startsWithTest() {
    struct TestCase {
        core::StrView input;
        const char* prefix;
        bool expected;
    };

    constexpr TestCase cases[] = {
        { core::sv("abc"), "a", true },
        { core::sv("abc"), "ab", true },
        { core::sv("abc"), "abc", true },
        { core::sv("abcf"), "abce", false },

        // Edge cases
        { core::sv(), nullptr, false },
        { core::sv(static_cast<const char*>(nullptr), 10), nullptr, false },
        { core::sv("asdzxcas", 0), "asd", false },
        { core::sv(""), "", false },
        { core::sv("-"), "", false },
        { core::sv("-"), nullptr, false },
        { core::sv("-"), "--", false },
    };

    i32 ret = core::testing::executeTestTable("startsWithTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto res = core::startsWith(tc.input, tc.prefix);
        CT_CHECK(res == tc.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 endsWithTest() {
    struct TestCase {
        core::StrView input;
        const char* postfix;
        bool expected;
    };

    constexpr TestCase cases[] = {
        { core::sv("abc"), "c", true },
        { core::sv("abc"), "bc", true },
        { core::sv("abc"), "abc", true },
        { core::sv("abcf"), "nbcf", false },

        // Edge cases
        { core::sv(), nullptr, false },
        { core::sv(static_cast<const char*>(nullptr), 10), nullptr, false },
        { core::sv("asdzxcas", 0), "xcas", false },
        { core::sv(""), "", false },
        { core::sv("-"), "", false },
        { core::sv("-"), nullptr, false },
        { core::sv("-"), "--", false },
    };

    i32 ret = core::testing::executeTestTable("endsWithTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto res = core::endsWith(tc.input, tc.postfix);
        CT_CHECK(res == tc.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 literalOperatorTest() {
    auto v = "hello"_sv;
    CT_CHECK(v.len() == 5);
    CT_CHECK(v.data()[0] == 'h');
    CT_CHECK(v[1] == 'e');
    return 0;
}

} // namespace

i32 runStrViewTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo(sInfo);

    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceLeftTest);
    if (runTest(tInfo, trimWhiteSpaceLeftTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceRightTest);
    if (runTest(tInfo, trimWhiteSpaceRightTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimTest);
    if (runTest(tInfo, trimTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceTest);
    if (runTest(tInfo, trimWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimSpecificCharTest);
    if (runTest(tInfo, trimSpecificCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cutTest);
    if (runTest(tInfo, cutTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(splitTest);
    if (runTest(tInfo, splitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(startsWithTest);
    if (runTest(tInfo, startsWithTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(endsWithTest);
    if (runTest(tInfo, endsWithTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(literalOperatorTest);
    if (runTest(tInfo, literalOperatorTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeMemTestsSuite() {
    RunTestCompileTime(trimWhiteSpaceLeftTest);
    RunTestCompileTime(trimWhiteSpaceRightTest);
    RunTestCompileTime(trimTest);
    RunTestCompileTime(trimWhiteSpaceTest);
    RunTestCompileTime(trimSpecificCharTest);
    RunTestCompileTime(cutTest);
    RunTestCompileTime(splitTest);
    RunTestCompileTime(startsWithTest);
    RunTestCompileTime(endsWithTest);
    RunTestCompileTime(literalOperatorTest);

    return 0;
}
