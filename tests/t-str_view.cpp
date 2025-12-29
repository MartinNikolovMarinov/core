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
        auto skipped = core::trim(tc.input);

        CT_CHECK(skipped.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(skipped.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(skipped.len() == tc.expected.len(), cErr);
        CT_CHECK(skipped.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 skipCharTest() {
    struct TestCase {
        core::StrView input;
        char skipChar;
        core::StrView expected;
    };

    constexpr TestCase cases[] = {
        { core::sv(), '/', core::sv() },
        { core::sv(""), '/', core::sv("") },
        { core::sv("////path"), '/', core::sv("path") },
        { core::sv("no-prefix"), '/', core::sv("no-prefix") },
        { core::sv("....."), '.', core::sv() },
    };

    i32 ret = core::testing::executeTestTable("skip failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto skipped = core::trim(tc.input, tc.skipChar);

        CT_CHECK(skipped.eq(tc.expected), cErr);
        if (tc.expected.data() == nullptr) {
            CT_CHECK(skipped.data() == tc.expected.data(), cErr);
        }
        CT_CHECK(skipped.len() == tc.expected.len(), cErr);
        CT_CHECK(skipped.empty() == tc.expected.empty(), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 cutTest() {
    struct TestCase {
        core::StrView input;
        char c;
        core::StrView expectedCut;
        core::StrView expectedReturn;
    };

    constexpr TestCase cases[] = {
        { "-bcde"_sv, '-', core::sv(), "bcde"_sv },
        { "a-cde"_sv, '-', "a"_sv, "cde"_sv },
        { "ab-de"_sv, '-', "ab"_sv, "de"_sv },
        { "abc-e"_sv, '-', "abc"_sv, "e"_sv },
        { "abcd-"_sv, '-', "abcd"_sv, core::sv() },
        { "abcde"_sv, '-', core::sv(), core::sv() },

        // Simulating real use case
        { "abc\ndef\n"_sv, '\n', "abc"_sv, "def\n"_sv },
        { "def\n"_sv, '\n', "def"_sv, core::sv() },

        // Edge cases
        { "?"_sv, '?', core::sv(), core::sv() },
        { ""_sv, '-', core::sv(), core::sv() },
        { "abc"_sv, '-', core::sv(), core::sv() },
        { "-"_sv, '+', core::sv(), core::sv() },
        { core::sv(), 'x', core::sv(), core::sv() },
    };

    i32 ret = core::testing::executeTestTable("cutTest failed at: ", cases, [](const auto& tc, const char* cErr) {
        core::StrView gotCut;
        core::StrView gotReturn = core::cut(tc.input, tc.c, gotCut);

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

i32 runStrViewTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceLeftTest);
    if (runTest(tInfo, trimWhiteSpaceLeftTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceRightTest);
    if (runTest(tInfo, trimWhiteSpaceRightTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimTest);
    if (runTest(tInfo, trimTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(trimWhiteSpaceTest);
    if (runTest(tInfo, trimWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(skipCharTest);
    if (runTest(tInfo, skipCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cutTest);
    if (runTest(tInfo, cutTest) != 0) { ret = -1; }
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
    RunTestCompileTime(skipCharTest);
    RunTestCompileTime(cutTest);
    RunTestCompileTime(startsWithTest);
    RunTestCompileTime(endsWithTest);
    RunTestCompileTime(literalOperatorTest);

    return 0;
}
