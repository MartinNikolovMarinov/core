#include "t-index.h"

[[nodiscard]]
i32 sbIsUninitialized(const core::StrBuilder& sb) {
    CT_CHECK(sb.empty());
    CT_CHECK(sb.len() == 0);
    CT_CHECK(sb.cap() == 0);
    CT_CHECK(sb.byteLen() == 0);
    CT_CHECK(sb.byteCap() == 0);
    CT_CHECK(sb.view().len() == 0);
    CT_CHECK(sb.view().data() == nullptr);
    return 0;
};

i32 initializeStrBuilderTest() {
    using core::StrBuilder;
    using value_type = core::StrBuilder::value_type;

    {
        // StrBuilder();
        StrBuilder sb;
        CT_CHECK(sbIsUninitialized(sb) == 0);
    }

    {
        // StrBuilder(size_type cap);
        constexpr addr_size N = 5;
        StrBuilder sb(N);
        CT_CHECK(sb.empty());
        CT_CHECK(sb.len() == 0);
        CT_CHECK(sb.cap() == N + 1);
        CT_CHECK(sb.byteLen() == 0);
        CT_CHECK(sb.byteCap() == (N + 1) * sizeof(value_type));
        CT_CHECK(sb.view().len() == 0);
        CT_CHECK(sb.view().data() != nullptr);
    }

    {
        // StrBuilder(size_type len, const value_type& val);
        constexpr addr_size N = 5;
        StrBuilder sb(N, 'a');
        CT_CHECK(!sb.empty());
        CT_CHECK(sb.len() == N);
        CT_CHECK(sb.cap() == N + 1);
        CT_CHECK(sb.byteLen() == N * sizeof(value_type));
        CT_CHECK(sb.byteCap() == (N + 1) * sizeof(value_type));
        CT_CHECK(sb.view().len() == N);
        CT_CHECK(core::memcmp(sb.view().data(),
                 core::cstrLen(sb.view().data()), // intentionally useing cstrLen here.
                 "aaaaa", N) == 0);
    }

    {
        // StrBuilder(const value_type* cstr);
        // explicit StrBuilder(const StrView& view);

        constexpr const char* vv = "bbbbbb";
        constexpr addr_size vvlen = core::cstrLen("bbbbbb");

        auto check = [](StrBuilder& sb) {
            CT_CHECK(!sb.empty());
            CT_CHECK(sb.len() == vvlen);
            CT_CHECK(sb.cap() == vvlen + 1);
            CT_CHECK(sb.byteLen() == vvlen * sizeof(value_type));
            CT_CHECK(sb.byteCap() == (vvlen + 1) * sizeof(value_type));
            CT_CHECK(sb.view().len() == vvlen);
            CT_CHECK(core::memcmp(sb.view().data(),
                    core::cstrLen(sb.view().data()), // intentionally useing cstrLen here.
                    vv, vvlen) == 0);
            CT_CHECK(sb.eq(core::sv(vv)));
            return 0;
        };

        StrBuilder sb1(core::sv(vv)); check(sb1);
        StrBuilder sb2(core::sv(vv)); check(sb1);
    }

    return 0;
}

i32 appendToStrBuilderTest() {
    using core::StrBuilder;
    using core::StrView;
    using value_type = core::StrBuilder::value_type;

    {
        StrBuilder a;

        for (value_type c = 'a'; c <= 'z'; ++c) {
            addr_size idx = addr_size(c - 'a');
            a.append(c);
            CT_CHECK(a.len() == idx + 1);
            CT_CHECK(a.cap() > idx + 1);
            CT_CHECK(a.view().len() == idx + 1);
            CT_CHECK(a.view().data()[idx] == c);
        }

        a.clear();
        CT_CHECK(a.len() == 0);
        CT_CHECK(a.view().len() == 0);
        // CT_CHECK(a.view().data() == nullptr); // NOTE: This is NOT guaranteed!

        for (value_type c = 'a'; c <= 'z'; ++c) {
            addr_size idx = addr_size(c - 'a');
            a.append(c);
            CT_CHECK(a.len() == idx + 1);
            CT_CHECK(a.cap() > idx + 1);
            CT_CHECK(a.view().len() == idx + 1);
            CT_CHECK(a.view().data()[idx] == c);
        }
    }

    {
        auto check = [](StrBuilder& sb, auto appendFn) {
            for (value_type c = 'a'; c <= 'z'; ++c) {
                addr_size idx = addr_size(c - 'a');
                appendFn(sb, c);
                CT_CHECK(sb.len() == idx + 1);
                CT_CHECK(sb.cap() > idx + 1);
                CT_CHECK(sb.view().len() == idx + 1);
                CT_CHECK(sb.view().data()[idx] == c);
            }

            return 0;
        };

        StrBuilder a1;
        check(a1, [](StrBuilder& sb, value_type c) { sb.append(c); });
        a1.clear();
        check(a1, [](StrBuilder& sb, value_type c) { sb.append(&c, 1); });

        StrBuilder a2;
        check(a2, [](StrBuilder& sb, value_type c) { sb.append(&c, 1); });
        a2.clear();
        check(a2, [](StrBuilder& sb, value_type c) { sb.append(c); });
    }

    {
        struct TestCase {
            const char* start;
            const char* in;
            StrView expected;
        };

        TestCase cases[] = {
            { nullptr, "123456789", "123456789"_sv },
            { "", "123456789", "123456789"_sv },
            { "1", "23456789", "123456789"_sv },
            { "12", "3456789", "123456789"_sv },
            { "123", "456789", "123456789"_sv },
            { "1234", "56789", "123456789"_sv },
            { "12345", "6789", "123456789"_sv },
            { "123456", "789", "123456789"_sv },
            { "1234567", "89", "123456789"_sv },
            { "12345678", "9", "123456789"_sv },
            { "123456789", "", "123456789"_sv },
            { "123456789", nullptr, "123456789"_sv },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            {
                StrBuilder sb (core::sv(c.start));
                sb.append(c.in, core::cstrLen(c.in));
                CT_CHECK(sb.eq(c.expected), cErr);
                CT_CHECK(sb.len() == c.expected.len(), cErr);
            }
            {
                StrBuilder sb;
                sb = core::sv(c.start);
                sb.append(c.in, core::cstrLen(c.in));
                CT_CHECK(sb.eq(c.expected), cErr);
                CT_CHECK(sb.len() == c.expected.len(), cErr);
            }
            {
                StrBuilder sb (core::sv(c.start));
                sb.append(core::sv(c.in, core::cstrLen(c.in)));
                CT_CHECK(sb.eq(c.expected), cErr);
                CT_CHECK(sb.len() == c.expected.len(), cErr);
            }

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 moveAndCopyStrBuilderTest() {
    using core::StrBuilder;
    using core::StrView;

    {
        static constexpr StrView view = "testing"_sv;
        static const StrBuilder checksb (view);

        auto check = [](const StrBuilder& sb) {
            CT_CHECK(sb.len() == view.len());
            CT_CHECK(sb.cap() > sb.len());
            CT_CHECK(sb.eq(view));
            CT_CHECK(sb.eq(checksb));
            return 0;
        };

        StrBuilder sb0; sb0.append("testing"_sv);       // StrBuilder();
        StrBuilder sb1 = StrBuilder(std::move(sb0)); // StrBuilder(StrBuilder&& other);
        StrBuilder sb3 = StrBuilder(view);           // StrBuilder(const StrView& view);
        StrBuilder sb4 = StrBuilder("testing"_sv);   // StrBuilder(StrView&& view);

        CT_CHECK(sbIsUninitialized(sb0) == 0);
        CT_CHECK(check(sb1) == 0);
        CT_CHECK(check(sb3) == 0);
        CT_CHECK(check(sb4) == 0);

        StrView movableView (view.data());

        StrBuilder sb5, sb7, sb8;
        sb5 = std::move(sb1);         // StrBuilder& operator=(StrBuilder&& other);
        sb7 = movableView;            // StrBuilder& operator=(const StrView& view);
        sb8 = std::move(movableView); // StrBuilder& operator=(StrView&& view);

        CT_CHECK(sbIsUninitialized(sb1) == 0);
        CT_CHECK(movableView.data() == nullptr);
        CT_CHECK(movableView.len() == 0);
        CT_CHECK(check(sb5) == 0);
        CT_CHECK(check(sb7) == 0);
        CT_CHECK(check(sb8) == 0);

        StrBuilder sb9 = sb8.copy();
        CT_CHECK(check(sb8) == 0);
        CT_CHECK(check(sb9) == 0);
    }

    {
        PRAGMA_WARNING_PUSH

        DISABLE_CLANG_WARNING(-Wself-move) // this is what I am testing here. I know it's wrong to do.

        StrBuilder sb("aaa"_sv);
        sb = std::move(sb);

        CT_CHECK(sb.len() == 3);
        CT_CHECK(sb.cap() > 3);
        CT_CHECK(sb.eq("aaa"_sv));

        PRAGMA_WARNING_POP
    }

    return 0;
}

i32 resetAndReleaseStrBuilderTest() {
    using core::StrBuilder;
    using core::StrView;
    using size_type = core::StrBuilder::size_type;
    using value_type = core::StrBuilder::value_type;

    {
        StrBuilder sb ("some string"_sv);

        size_type cap = 0;
        size_type len = 0;
        value_type* released = sb.release(len, cap);
        defer {
            core::free(released, cap, sizeof(value_type)); // memory leak will be detected if this function is not called;
        };

        CT_CHECK(sbIsUninitialized(sb) == 0);
    }

    {
        StrBuilder sb ("some string"_sv);

        size_type cap = 0;
        size_type len = 0;
        value_type* released = sb.release(len, cap);
        core::memset(released + len, 0, cap - len); // zero out the memory after len.

        CT_CHECK(sbIsUninitialized(sb) == 0);

        //  Giving the ownership back to the string builder should not leak memory!
        sb.reset(&released, len, cap);

        CT_CHECK(sb.len() == len);
        CT_CHECK(sb.cap() == cap);
        CT_CHECK(sb.eq("some string"_sv));
        CT_CHECK(released == nullptr);
    }

    return 0;
}

i32 ensureCapStrBuilderTest() {
    using core::StrBuilder;
    using size_type = core::StrBuilder::size_type;

    // This test checks internal details, like the exact capcity, which might change in the future.
    // It's still worth it to check that the growth algorithm is working as expected.
    {
        struct TestCase {
            size_type start;
            size_type in;
            size_type expected;
        };

        TestCase cases[] = {
            { 0, 0, 0 },
            { 0, 1, 1 },
            { 0, 2, 2 },
            { 0, 3, 3 },
            { 0, 4, 4 },

            { 1, 0, 2 },
            { 1, 1, 2 },
            { 1, 2, 2 },
            { 1, 3, 3 },
            { 1, 4, 4 },

            { 2, 0, 3 },
            { 2, 1, 3 },
            { 2, 2, 3 },
            { 2, 3, 3 },
            { 2, 4, 4 },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            auto check = [&](const StrBuilder& sb) -> i32 {
                CT_CHECK(sb.cap() == c.expected, cErr);
                CT_CHECK(sb.len() == 0, cErr);
                CT_CHECK(sb.view().len() == 0, cErr);
                return 0;
            };

            StrBuilder sb (c.start);

            sb.ensureCap(c.in);
            CT_CHECK(check(sb) == 0, cErr);

            sb.ensureCap(c.in);
            CT_CHECK(check(sb) == 0, cErr);

            sb.ensureCap(core::core_min(addr_size(c.in - 5), addr_size(0)));
            CT_CHECK(check(sb) == 0, cErr);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 runStrBuilderTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;

        tInfo.name = FN_NAME_TO_CPTR(initializeStrBuilderTest);
        if (runTest(tInfo, initializeStrBuilderTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(appendToStrBuilderTest);
        if (runTest(tInfo, appendToStrBuilderTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(moveAndCopyStrBuilderTest);
        if (runTest(tInfo, moveAndCopyStrBuilderTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(resetAndReleaseStrBuilderTest);
        if (runTest(tInfo, resetAndReleaseStrBuilderTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(ensureCapStrBuilderTest);
        if (runTest(tInfo, ensureCapStrBuilderTest) != 0) { retCode = -1; }
    };

    i32 ret = 0;
    runForAllGlobalAllocatorVariants(runTests, ret);

    {
        constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 3;
        char buf[BUFFER_SIZE];
        USE_STACK_BASED_BUMP_ALLOCATOR_FOR_BLOCK_SCOPE(buf, BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "STACK BASED BUMP Allocator", ret);
    }

    {
        constexpr u32 BUFFER_SIZE = 256; // intentially small to test overflow.
        USE_CUSTOM_ARENA_ALLOCATOR_FOR_FOR_BLOCK_SCOPE(BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "CUSTOM ARENA Allocator", ret);
    }

    return ret;
}
