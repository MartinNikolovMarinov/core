#include "t-index.h"

// ----------------------------------------------- Begin Helpers -------------------------------------------------------


template <typename M, typename K, typename V>
i32 __test_verifyKeyVal(const M& m, const core::tuple<K, V>& kv) {
    auto& key = kv.v1;
    auto& val = kv.v2;

    auto a = m.get(key);
    CT_CHECK(a != nullptr, "Failed to get data for key");
    CT_CHECK(*a == val, "Value at key is incorrect");

    i32 found = 0;
    m.keys([&](const auto& k) -> bool {
        if (core::eq(k, key)) { found++; }
        return true;
    });
    CT_CHECK(found == 1, "Key should be found exactly once");

    found = 0;
    m.values([&](const auto& v) -> bool {
        if (v == val) { found++; }
        return true;
    });
    CT_CHECK(found > 0, "Value should be found at least once");

    return 0;
}

// test verify many key values with variadic
template <typename M, typename... KV>
i32 __test_verifyKeyVals(const M& m, const KV&... kv) {
    return (__test_verifyKeyVal(m, kv), ...);
}

template <typename M, typename K>
i32 __test_verifyKey(const M& m, const K& key) {
    auto a = m.get(key);
    CT_CHECK(a != nullptr, "Failed to get data for key");

    i32 found = 0;
    m.keys([&](const auto& k) -> bool {
        if (core::eq(k, key)) { found++; }
        return true;
    });
    CT_CHECK(found == 1, "Key should be found exactly once");

    return 0;
}

template <typename M, typename... K>
i32 __test_verifyKeys(const M& m, const K&... keys) {
    return (__test_verifyKey(m, keys), ...);
}

template <typename M>
i32 __test_verifyNoKeys(const M& m) {
    bool noKeys = true;
    m.keys([&](const auto&) -> bool { noKeys = false; return true; });
    CT_CHECK(noKeys, "Hash map should not have keys");
    return 0;
}

template <typename M>
i32 __test_verifyNoValues(const M& m) {
    bool noValues = true;
    m.values([&](const auto&) -> bool { noValues = false; return true; });
    CT_CHECK(noValues, "Hash map should not have values");
    return 0;
}

// ----------------------------------------------- End Helpers ---------------------------------------------------------

i32 initializeHashMapTest() {
    {
        core::HashMap<i32, i32> m;
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.cap() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<core::StrView, core::StrView> m;
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.cap() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<i32, i32> m(7);
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.cap() > 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<core::StrView, core::StrView> m(7);
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.cap() > 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }

    return 0;
}

i32 basicPushToHashMapTest() {
    using namespace core::testing;
    using core::sv;
    using core::createTuple;

    {
        core::HashMap<i32, i32> m;

        m.put(1, 1);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(1, 1)) == 0);

        i32 a = 2;
        m.put(a, 1);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 1)) == 0);

        i32 b = 9;
        m.put(2, b);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 9)) == 0);

        m.put(b, a);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 9),
                                         createTuple(9, 2)) == 0);
    }

    {
        core::HashMap<i32, f32> m;

        m.put(1, 1.0f);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(1, 1.0f)) == 0);

        f32 a = 2.0f;
        m.put(2, a);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 2.0f)) == 0);

        f32 b = 9.0f;
        m.put(2, b);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 9.0f)) == 0);

        i32 c = 9;
        m.put(c, a);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 9.0f),
                                         createTuple(9, 2.0f)) == 0);
    }

    {
        core::HashMap<core::StrView, core::StrView> m;

        m.put(sv("1"), "abc"_sv);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(sv("1"), sv("abc"))) == 0);

        auto a = "def"_sv;
        m.put(sv("2"), a);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "def"_sv)) == 0);

        auto b = "2"_sv;
        m.put(b, sv("ghi"));
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "ghi"_sv)) == 0);

        m.put(a, b);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "ghi"_sv),
                                         createTuple(sv("def"), "2"_sv)) == 0);
    }

    {
        defer { CT::resetAll(); };

        CT k1, k2, k3, k4;
        k1.a = 1; k2.a = 2; k3.a = 3; k4.a = 4;
        CT::resetAll(); // Reset the counters for the previous line.

        {
            core::HashMap<CT, i32> m;
            CT_CHECK(CT::noCtorsCalled());

            m.put(k1, 9);
            CT_CHECK(CT::copyCtorCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            m.put(k2, 10);
            CT_CHECK(CT::copyCtorCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            i32 counter = 0;
            i32 err = 0;
            m.values([&](const auto& v) -> bool {
                if (counter == 0 && v != 9) {
                    err = -1;
                    return false;
                }
                if (counter == 1 && v != 10) {
                    err = -2;
                    return false;
                }
                counter++;
                return true;
            });

            CT_CHECK(err == 0);
            CT_CHECK(CT::copyCtorCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 2);
        CT::resetAll();
    }

    return 0;
}

i32 getWhenHashMapIsFilledToCapacityTest() {
    core::HashMap<i32, i32> m(2);

    m.put(1, 1);
    m.put(2, 1);

    CT_CHECK(m.len() == 2);
    CT_CHECK(m.cap() > 2);
    CT_CHECK(!m.empty());

    CT_CHECK(m.get(1) != nullptr);
    CT_CHECK(m.get(2) != nullptr);
    CT_CHECK(m.get(3) == nullptr); // Make sure these don't loop forever.
    CT_CHECK(m.get(55) == nullptr);

    return 0;
}

constexpr i32 hashMapTraitsTest() {
    static_assert(std::is_standard_layout_v<core::HashMap<i32, i32>>,
                 "HashMap(i32, i32) should be standard layout");
    static_assert(std::is_standard_layout_v<core::HashMap<core::StrView, core::StrView>>,
                 "HashMap(StrView, StrView) should be standard layout");

    return 0;
}

i32 runHashMapTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;
        tInfo.useAnsiColors = g_useAnsi;

        tInfo.name = FN_NAME_TO_CPTR(initializeHashMapTest);
        if (runTest(tInfo, initializeHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(basicPushToHashMapTest);
        if (runTest(tInfo, basicPushToHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(getWhenHashMapIsFilledToCapacityTest);
        if (runTest(tInfo, getWhenHashMapIsFilledToCapacityTest) != 0) { retCode = -1; }

        // FIXME: Continue writing tests here.
        // FIXME: Implement the hash set as well.
        // FIXME: Implement a static sized hash set for the cases where a exactly sized map is needed.
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

constexpr i32 runCompiletimeHashMapTestsSuite() {
    RunTestCompileTime(hashMapTraitsTest);

    return 0;
}
