#include "core_str_view.h"
#include "t-index.h"
#include "testing/testing_framework.h"

namespace {

constexpr i32 trimWhiteSpaceLeftTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        addr_size expectedOffset;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, 0, 0 },
        { "", 0, 0, 0 },
        { "abc", 3, 0, 3 },
        { "   abc", 6, 3, 3 },
        { "  abc   ", 8, 2, 6 },
        { "   ", 3, 3, 0 },
    };

    i32 ret = core::testing::executeTestTable("trimWhiteSpaceLeft failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        auto trimmed = core::trimWhiteSpaceLeft(view);

        const char* expectedPtr = tc.input ? tc.input + tc.expectedOffset : nullptr;
        CT_CHECK(trimmed.data() == expectedPtr, cErr);
        CT_CHECK(trimmed.len() == tc.expectedLen, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimWhiteSpaceRightTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, 0 },
        { "", 0, 0 },
        { "abc", 3, 3 },
        { "abc   ", 6, 3 },
        { "  abc   ", 8, 5 },
        { "   ", 3, 0 },
    };

    i32 ret = core::testing::executeTestTable("trimWhiteSpaceRight failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        auto trimmed = core::trimWhiteSpaceRight(view);

        CT_CHECK(trimmed.data() == tc.input, cErr);
        CT_CHECK(trimmed.len() == tc.expectedLen, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 trimTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        addr_size expectedOffset;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, 0, 0 },
        { "", 0, 0, 0 },
        { "abc", 3, 0, 3 },
        { "  abc   ", 8, 2, 3 },
        { "   surrounded   ", 16, 3, 10 },
        { "   ", 3, 3, 0 },
    };

    i32 ret = core::testing::executeTestTable("trim failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        auto trimmed = core::trim(view);

        const char* expectedPtr = tc.input ? tc.input + tc.expectedOffset : nullptr;
        CT_CHECK(trimmed.data() == expectedPtr, cErr);
        CT_CHECK(trimmed.len() == tc.expectedLen, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 skipWhiteSpaceTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        addr_size expectedOffset;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, 0, 0 },
        { "", 0, 0, 0 },
        { "abc", 3, 0, 3 },
        { "   abc", 6, 3, 3 },
        { "   ", 3, 3, 0 },
    };

    i32 ret = core::testing::executeTestTable("skipWhiteSpace failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        auto skipped = core::skipWhiteSpace(view);

        const char* expectedPtr = tc.input ? tc.input + tc.expectedOffset : nullptr;
        if (tc.expectedLen > 0) {
            CT_CHECK(skipped.data() == expectedPtr, cErr);
        }
        CT_CHECK(skipped.len() == tc.expectedLen, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 skipCharTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        char skipChar;
        addr_size expectedOffset;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, '/', 0, 0 },
        { "", 0, '/', 0, 0 },
        { "////path", 8, '/', 4, 4 },
        { "no-prefix", 9, '/', 0, 9 },
        { ".....", 5, '.', 5, 0 },
    };

    i32 ret = core::testing::executeTestTable("skip failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        auto skipped = core::skip(view, tc.skipChar);

        const char* expectedPtr = tc.input ? tc.input + tc.expectedOffset : nullptr;
        if (tc.expectedLen > 0) {
            CT_CHECK(skipped.data() == expectedPtr, cErr);
        }
        CT_CHECK(skipped.len() == tc.expectedLen, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 cutTest() {
    struct TestCase {
        const char* input;
        addr_size len;
        char delim;
        bool expectedFound;
        addr_size expectedOffset;
        addr_size expectedLen;
    };

    constexpr TestCase cases[] = {
        { nullptr, 0, ':', false, 0, 0 },
        { "", 0, ':', false, 0, 0 },
        { "key:value", 9, ':', true, 4, 5 },
        { ":leading", 8, ':', true, 1, 7 },
        { "a:b:c", 5, ':', true, 2, 3 },
        { "trailing:", 9, ':', false, 0, 0 },
        { "no_delim", 8, ':', false, 0, 0 },
        { ":::multiple", 11, ':', true, 1, 10 },
        { "multiple:::", 11, ':', true, 9, 2 },
    };

    i32 ret = core::testing::executeTestTable("cut failed at: ", cases, [](const auto& tc, const char* cErr) {
        auto view = core::sv(tc.input, tc.len);
        core::StrView out;
        bool found = core::cut(view, tc.delim, out);

        CT_CHECK(found == tc.expectedFound, cErr);

        if (tc.expectedFound) {
            const char* expectedPtr = tc.input ? tc.input + tc.expectedOffset : nullptr;
            if (tc.expectedLen > 0) {
                CT_CHECK(out.data() == expectedPtr, cErr);
            }
            CT_CHECK(out.len() == tc.expectedLen, cErr);
        }
        else {
            CT_CHECK(out.data() == nullptr && out.len() == 0, cErr);
        }

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
    tInfo.name = FN_NAME_TO_CPTR(skipWhiteSpaceTest);
    if (runTest(tInfo, skipWhiteSpaceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(skipCharTest);
    if (runTest(tInfo, skipCharTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(cutTest);
    if (runTest(tInfo, cutTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(literalOperatorTest);
    if (runTest(tInfo, literalOperatorTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeMemTestsSuite() {
    RunTestCompileTime(trimWhiteSpaceLeftTest);
    RunTestCompileTime(trimWhiteSpaceRightTest);
    RunTestCompileTime(trimTest);
    RunTestCompileTime(skipWhiteSpaceTest);
    RunTestCompileTime(skipCharTest);
    RunTestCompileTime(cutTest);
    RunTestCompileTime(literalOperatorTest);

    return 0;
}
